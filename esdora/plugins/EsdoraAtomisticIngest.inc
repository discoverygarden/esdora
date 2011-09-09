<?php
module_load_include('inc', 'esdora_islandora', 'plugins/EsdoraIngest');
class EsdoraAtomisticIngest extends EsdoraIngest{
 /**
   *
   * @param <type> $form_values
   */
 function handleZipIngestForm($form_values) {
    module_load_include('inc', 'fedora_repository', 'api/fedora_item');
    module_load_include('inc', 'fedora_repository', 'api/dublin_core');
    module_load_include('inc', 'fedora_repository', 'MimeClass');
    $pid = $form_values['pid'];
    $collection_pid = $form_values['collection_pid'];
    $fileName = $form_values['ingest-file-location'];
    $tmpDir = $this->unzip_file($fileName);
    $files = scandir($tmpDir);
    foreach ($files as $file) {
      if (is_dir("$tmpDir/$file") && $file != '.' && $file != '..' && $file != '__MACOSX') {
        $new_object_label = $file; //also part of the file path as this is the top level directory
      }
    }
    $object_array = $this->createSkeletonObject($new_object_label, $pid, $collection_pid,'esdora:defaultCollectionCModel');
    if (!$object_array) {
      drupal_set_message('Error creating object in fedora', 'error');
      return;
    }
    $new_object_label = $object_array[0];
    $object = $object_array[1];
    //TODO: add collection policy stream
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
        $stream_id = $file; //substr($file, 0, strrpos($file, '.'));
        if (is_numeric(substr($stream_id, 0, 1))) {//stream ids cannont start with a number
          $stream_id = 'n' . $stream_id; //if it starts with a number prfix the id with an n?
        }
        $object->add_datastream_from_file($full_path, $stream_id, $file, $m_type, $controlGroup, 'Added metadata datastream at ingest');
        module_load_include('inc', 'esdora_islandora', 'plugins/esdora');

        Esdora::add_relationship_internal('isMetaDataFor', $pid, $stream_id, "http://esdora.ornl.gov/ontologies/relationships#");
      }
    }

    $files = scandir("$tmpDir/$new_object_label/Data");
    foreach ($files as $file) {//metadata files
      $full_path = "$tmpDir/$new_object_label/Data/$file";
      if (!is_dir($full_path) && $file != '.' && $file != '..' && $file != '__MACOSX') {
        $object_array = $this->createSkeletonObject($file, Fedora_Item::get_next_PID_in_namespace('es:'), $pid,'esdora:simpleDataCModel');
      }
    }
    $this->add_viewer_streams("$tmpDir/$new_object_label/viewers.txt",$object);
    $text = file("$tmpDir/$new_object_label/DOI.txt");
    $this->add_doi_to_dc($object,$text[0]);//
    $this->deleteDirectory($tmpDir);
    unlink($fileName);
  }

}

?>
