<?php

class EsdoraIngest {

  /**
   *
   * @param array $form
   * @param <type> $form_state
   * @return boolean
   */
  function buildDrupalForm(&$form, &$form_state) {
    $file = (isset($form_state['values']['ingest-file-location']) ? $form_state['values']['ingest-file-location'] : '');
    $form['#multistep'] = TRUE; // used so that it triggers a form rebuild every time.

    return $form;
  }

  function unzip_file($file) {
    $tmpDir = tempnam(file_directory_path(), "esdora_");
    unlink($tmpDir); //we want a directory not a file, tempnam actually creates a file there should be an easier way
    $test = mkdir($tmpDir, 0755); //create a directory
    //$fileName = $file;
    //$file_list = array();
    $cmdString = "unzip -q -o -d $tmpDir \"$file->filepath\"";
    system($cmdString, $retVal); //unzip the files this command seems to return an int of 0 on success
    if ($retVal === 0) {//
      return $tmpDir;
    }
    else {
      return FALSE;
    }
  }

  /**
   * potentially a method defined by Islandora and implement here
   * for now it is just a esdora function
   */
  function getNextPID($collection_pid, $dataset_name) {
      $temp=trim($dataset_name);
      $temp=str_replace(' ', '_', $temp);
      $temp=str_replace('.', '_', $temp);
      $temp=str_replace(':', '_', $temp);
      return $collection_pid . '.' . $temp; 
  }
  

  /**
   *
   * @global object $user
   * @param string $label
   * @param string $pid
   * @param string $collection_pid
   * @param string $content_model
   * @return array
   */
  function create_skeleton_object($label, $pid, $collection_pid, $content_model) {
    module_load_include('inc', 'esdora_islandora', 'plugins/esdora');
    global $user; //make the fedora object owned by the drupal user that creates it
    $foxml = Fedora_Item::create_object_FOXML($pid, 'A', $label, $user->name);
    $object = Fedora_Item::ingest_from_FOXML($foxml);
    if (!isset($object)) {
        drupal_set_message("Could not create new object: $pid, check fedora log messages for details.");
        return false;
    }
    $object->add_relationship('isMemberOfCollection', $collection_pid);
    $object->add_relationship('hasModel', $content_model, FEDORA_MODEL_URI);
    Esdora::add_oai_to_rels($object);
    return array($label, $object);
  }

  function add_viewer_streams($file, $object) {
    if(!is_file($file)) {
        return;
    }  
    $viewers = file($file); //read the file each line is a member of an array
    foreach ($viewers as $viewer) {
      $viewer_parts = explode(',', $viewer);
//TODO: add error checking for text processing
      $object->add_datastream_from_url($viewer_parts[2], $viewer_parts[0], $viewer_parts[1], '', $controlGroup = 'R', 'added viewer stream');
      Esdora::add_relationship_internal('isViewerFor', $object->pid, $viewer_parts[0], "http://esdora.ornl.gov/ontologies/relationships#");
    }
  }

  /**
   *
   * @param <type> $object
   * @param <type> $doi
   * @param <type> $title optionally set the title as well
   */
  function update_dc($object, $doi, $title = NULL) {
    $dc = new Dublin_Core($object);
    $dc->add_element('dc:identifier', $doi);
    if(isset($title)){
      $dc->set_element('dc:title',$title);
    }
    $dc->save();
  }

  /**
   *
   * @param <type> $form_values
   */
  function handleZipIngestForm($collection_pid, $uploaded_file, $content_model_pid, $dataset_name) {
    module_load_include('inc', 'fedora_repository', 'api/fedora_item');
    module_load_include('inc', 'fedora_repository', 'MimeClass');
    module_load_include('inc', 'fedora_repository', 'api/dublin_core');
    
    $pid = $this->getNextPID($collection_pid, $dataset_name);
    $label=$dataset_name;
    //create a new data object here, if the following call pass, pid will be a real pid
    $this->create_skeleton_object($label, $pid, $collection_pid, $content_model_pid);
    $object = new Fedora_Item($pid);

    $tmpDir = $this->unzip_file($uploaded_file);
    $top_folder_name = NULL;
    $files = scandir($tmpDir);
     foreach ($files as $file) {
      if (is_dir("$tmpDir/$file") && $file != '.' && $file != '..' && $file != '__MACOSX') {
        $top_folder_name = $file; //also part of the file path as this is the top level directory
      }
    }
    
    $meta_files = scandir("$tmpDir/$top_folder_name/Metadata");
    module_load_include('inc', 'esdora_islandora', 'plugins/esdora');
    foreach ($meta_files as $file) {//metadata files
      $full_path = "$tmpDir/$top_folder_name/Metadata/$file";
      if (!is_dir($full_path) && $file != '.' && $file != '..' && $file != '__MACOSX') {
        $mime = new MimeClass();
        $m_type = $mime->get_mimetype($file);
        $controlGroup = 'M';
        $stream_id = $file; //substr($file, 0, strrpos($file, '.'));
//        if (strpos($file, 'FGDC') === 0) {
          //$controlGroup = 'X';
          //$datastream_url = drupal_urlencode($full_path); //using the form builder the fgdc.xml gets created as inline xml so we have to modify by value
          //$url = file_create_url($datastream_url);  // if this is not desired then we may have to remove the fgdc and readd.
//          $content = file_get_contents($full_path);
//          $object->modify_datastream_by_value($content, $file, $file, 'text/xml', $force = FALSE, $logMessage = 'Esdora updated FGDC.xml', $quiet = FALSE);
//        }
//        else {
          if (is_numeric(substr($stream_id, 0, 1))) {//stream ids cannont start with a number
            $stream_id = 'n' . $stream_id; //if it starts with a number prfix the id with an n?
          }
          $object->add_datastream_from_file($full_path, $stream_id, $file, $m_type, $controlGroup, 'Added metadata datastream at ingest');
//        }
        Esdora::add_relationship_internal('isMetaDataFor', $pid, $stream_id, "http://esdora.ornl.gov/ontologies/relationships#");
      }
    }

    $files = scandir("$tmpDir/$top_folder_name/Data");
    foreach ($files as $file) {//data files
      $full_path = "$tmpDir/$top_folder_name/Data/$file";
      if (!is_dir($full_path) && $file != '.' && $file != '..' && $file != '__MACOSX') {
        $mime = new MimeClass();
        $m_type = $mime->get_mimetype($file);
        $controlGroup = 'M';
        $stream_id = $file; //substr($file, 0, strrpos($file, '.'));
        if (is_numeric(substr($stream_id, 0, 1))) {//stream ids cannont start with a number
          $stream_id = 'n' . $stream_id; //if it starts with a number prfix the id with an n?
        }
        $checksumType="MD5";
        $checksum = $checksum = md5_file($full_path);
        $object->add_datastream_from_file($full_path, 
                $stream_id, 
                $file, 
                $m_type, 
                $controlGroup, 
                'Added data datastream at ingest',
                $checksumType,
                $checksum);
      }
    }
        Esdora::add_oai_to_rels($object);
//    $object->modify_object($top_folder_name);//add the label property ??
    $this->add_viewer_streams("$tmpDir/$top_folder_name/viewers.txt", $object);
    $doi_file="$tmpDir/$top_folder_name/DOI.txt";
    if(is_file($doi_file)) {
        $text = file($doi_file);
        $this->update_dc($object,$text[0],$new_object_label);
    }
        
    //cleanup now
    $this->deleteDirectory($tmpDir);
    unlink($uploaded_file->filepath);
    return $pid;
  }

  function deleteDirectory($dir) {
    if (!file_exists($dir))
      return true;
    if (!is_dir($dir))
      return unlink($dir);
    foreach (scandir($dir) as $item) {
      if ($item == '.' || $item == '..')
        continue;
      if (!$this->deleteDirectory($dir . DIRECTORY_SEPARATOR . $item))
        return false;
    }
    return rmdir($dir);
  }

}
?>
