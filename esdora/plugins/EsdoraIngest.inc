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

  /**
   *
   * @param <type> $form_values
   */
  function handleZipIngestForm($form_values) {
    $pid = $form_values['pid'];
    $collection_pid = $form_values['collection_pid'];
    if (!isset($pid) || !isset($collection_pid)) {
      drupal_set_message("could not create a new object no pid specified", "error");
      return;
    }
    module_load_include('inc', 'fedora_repository', 'api/fedora_item');
    module_load_include('inc', 'fedora_repository', 'MimeClass');
    $tmpDir = tempnam(file_directory_path(), "esdora_");
    unlink($tmpDir); //we want a directory not a file, tempnam actually creates a file
    $test = mkdir($tmpDir, 0755); //create a directory
    $file = $form_values['ingest-file-location'];
    $fileName = $file;
    $file_list = array();
    $cmdString = "unzip -q -o -d $tmpDir \"$fileName\"";
    system($cmdString, $retVal); //unzip the files
    $dirs = array();
    array_push($dirs, $tmpDir);
    $files = scandir($tmpDir);
    foreach ($files as $file) {
      if (is_dir("$tmpDir/$file") && $file != '.' && $file != '..' && $file != '__MACOSX') {
        $new_object_label = $file; //also part of the file path as this is the top level directory
      }
    }
    global $user; //make the fedora object owned by the drupal user that creates it
    $foxml = Fedora_Item::create_object_FOXML($pid, 'A', $new_object_label, $user->name);
    $object = Fedora_Item::ingest_from_FOXML($foxml);
    if (!isset($object)) {
      drupal_set_message('Error creating object in fedora', 'error');
//TODO delete $tmp dir
      return;
    }
// add relationships to collection object and content model object
    $object->add_relationship('isMemberOfCollection', $collection_pid);
    $object->add_relationship('hasModel','esdora:defaultDataCModel', FEDORA_MODEL_URI);
    drupal_set_message(t("Successfully added $pid to collection $collection_pid"));
    $files = scandir("$tmpDir/$new_object_label/Metadata");
    foreach ($files as $file) {//metadata files
      $full_path = "$tmpDir/$new_object_label/Metadata/$file";
      if (!is_dir($full_path) && $file != '.' && $file != '..' && $file != '__MACOSX') {
        $mime = new MimeClass();
        $m_type = $mime->get_mimetype($file);
        $controlGroup = 'M';
        //if (strpos($file, 'FGDC') === 0) {
        //  $controlGroup = 'X';
        //}
        $stream_id = $file;//substr($file, 0, strrpos($file, '.'));
        if (is_numeric(substr($stream_id, 0, 1))) {//stream ids cannont start with a number
          $stream_id = 'n' . $stream_id; //if it starts with a number prfix the id with an n?
        }
        $object->add_datastream_from_file($full_path, $stream_id, $file, $m_type, $controlGroup, 'Added metadata datastream at ingest');
        module_load_include('inc', 'esdora_islandora', 'plugins/esdora');
        $esdora = new Esdora($pid);
        $esdora->add_relationship_internal('isMetaDataFor', $pid, $stream_id, "http://esdora.ornl.gov/ontologies/relationships#");
      }
    }

    $files = scandir("$tmpDir/$new_object_label/Data");
    foreach ($files as $file) {//metadata files
      $full_path = "$tmpDir/$new_object_label/Data/$file";
      if (!is_dir($full_path) && $file != '.' && $file != '..' && $file != '__MACOSX') {
        $mime = new MimeClass();
        $m_type = $mime->get_mimetype($file);
        $controlGroup = 'M';
        $stream_id = $file;//substr($file, 0, strrpos($file, '.'));
        if (is_numeric(substr($stream_id, 0, 1))) {//stream ids cannont start with a number
          $stream_id = 'n' . $stream_id; //if it starts with a number prfix the id with an n?
        }
        $object->add_datastream_from_file($full_path, $stream_id, $file, $m_type, $controlGroup, 'Added data datastream at ingest');
      }
    }
    $viewers = file("$tmpDir/$new_object_label/viewers.txt"); //read the file each line is a member of an array
    foreach ($viewers as $viewer) {
      $viewer_parts = explode(',',$viewer);
      //TODO: add error checking for text processing
      $object->add_datastream_from_url($viewer_parts[2], $viewer_parts[0], $viewer_parts[1], '', $controlGroup = 'R','added viewer stream');
      $esdora->add_relationship_internal('isViewerFor', $pid, $viewer_parts[0], "http://esdora.ornl.gov/ontologies/relationships#");
    }
  }

  function read_dir_add_objects() {
//TODO: implement this function this put the for each loops here
  }

}

?>
