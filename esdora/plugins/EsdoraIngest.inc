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
    unlink($tmpDir); //we want a directory not a file, tempnam actually creates a file
    $test = mkdir($tmpDir, 0755); //create a directory
    $fileName = $file;
    $file_list = array();
    $cmdString = "unzip -q -o -d $tmpDir \"$file\"";
    system($cmdString, $retVal); //unzip the files this command seems to return an int of 0 on success
    if ($retVal === 0) {//
      return $tmpDir;
    }
    else {
      return FALSE;
    }
  }

  function createSkeletonObject($label, $pid, $collection_pid, $content_model) {
    global $user; //make the fedora object owned by the drupal user that creates it
    $foxml = Fedora_Item::create_object_FOXML($pid, 'A', $label, $user->name);
    $object = Fedora_Item::ingest_from_FOXML($foxml);
    if (!isset($object)) {
      $this->deleteDirectory($tmpDir);
      unlink($fileName);
      return FALSE;
    }
    $object->add_relationship('isMemberOfCollection', $collection_pid);
    $object->add_relationship('hasModel', $content_model, FEDORA_MODEL_URI);
    drupal_set_message(t("Successfully added @pid to collection @collection_pid",array('@pid'=>$pid,'@collection_pid'=>$collection_pid)));
    return array($label, $object);
  }

  function add_viewer_streams($file, $object) {
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
  function handleZipIngestForm($form_values) {
    module_load_include('inc', 'fedora_repository', 'api/fedora_item');
    module_load_include('inc', 'fedora_repository', 'MimeClass');
    module_load_include('inc', 'fedora_repository', 'api/dublin_core');
    $pid = $form_values['pid'];
    $collection_pid = $form_values['collection_pid'];
    $fileName = $form_values['esdora_full_path'];
    $tmpDir = $this->unzip_file($fileName);

    $files = scandir($tmpDir);
    foreach ($files as $file) {
      if (is_dir("$tmpDir/$file") && $file != '.' && $file != '..' && $file != '__MACOSX') {
        $new_object_label = $file; //also part of the file path as this is the top level directory
      }
    }

    $object = new Fedora_Item($pid); //$object_array[1];
    //var_dump($object);exit();
    $files = scandir("$tmpDir/$new_object_label/Metadata");
    foreach ($files as $file) {//metadata files
      $full_path = "$tmpDir/$new_object_label/Metadata/$file";
      if (!is_dir($full_path) && $file != '.' && $file != '..' && $file != '__MACOSX') {
        $mime = new MimeClass();
        $m_type = $mime->get_mimetype($file);
        $controlGroup = 'M';
        $stream_id = $file; //substr($file, 0, strrpos($file, '.'));
        if (strpos($file, 'FGDC') === 0) {
          //$controlGroup = 'X';
          //$datastream_url = drupal_urlencode($full_path); //using the form builder the fgdc.xml gets created as inline xml so we have to modify by value
          //$url = file_create_url($datastream_url);  // if this is not desired then we may have to remove the fgdc and readd.
          $content = file_get_contents($full_path);
          $object->modify_datastream_by_value($content, $file, $file, 'text/xml', $force = FALSE, $logMessage = 'Esdora updated FGDC.xml', $quiet = FALSE);
        }
        else {
          if (is_numeric(substr($stream_id, 0, 1))) {//stream ids cannont start with a number
            $stream_id = 'n' . $stream_id; //if it starts with a number prfix the id with an n?
          }
          $object->add_datastream_from_file($full_path, $stream_id, $file, $m_type, $controlGroup, 'Added metadata datastream at ingest');
          module_load_include('inc', 'esdora_islandora', 'plugins/esdora');
        }
        Esdora::add_relationship_internal('isMetaDataFor', $pid, $stream_id, "http://esdora.ornl.gov/ontologies/relationships#");
      }
    }

    $files = scandir("$tmpDir/$new_object_label/Data");
    foreach ($files as $file) {//data files
      $full_path = "$tmpDir/$new_object_label/Data/$file";
      if (!is_dir($full_path) && $file != '.' && $file != '..' && $file != '__MACOSX') {
        $mime = new MimeClass();
        $m_type = $mime->get_mimetype($file);
        $controlGroup = 'M';
        $stream_id = $file; //substr($file, 0, strrpos($file, '.'));
        if (is_numeric(substr($stream_id, 0, 1))) {//stream ids cannont start with a number
          $stream_id = 'n' . $stream_id; //if it starts with a number prfix the id with an n?
        }
        $object->add_datastream_from_file($full_path, $stream_id, $file, $m_type, $controlGroup, 'Added data datastream at ingest');
      }
    }
    $this->add_viewer_streams("$tmpDir/$new_object_label/viewers.txt", $object);
    $text = file("$tmpDir/$new_object_label/DOI.txt");
    //$this->add_doi_to_dc($object, $text[0]); //
    $this->update_dc($object,$text[0],$file);
    $this->deleteDirectory($tmpDir);
    unlink($fileName);
  }

  function read_dir_add_objects() {
//TODO: implement this function this put the for each loops here
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
