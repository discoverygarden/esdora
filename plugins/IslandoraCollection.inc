<?php

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

class IslandoraCollection {

  function __construct($pid = '') {
    module_load_include('inc', 'fedora_repository', 'api/fedora_item');
    module_load_include('inc', 'fedora_repository', 'CollectionClass');
    if (!empty($pid)) {
      $this->pid = $pid;
      $this->item = new Fedora_Item($pid);
    }
  }

  function showCollection() {
    $dc = $this->item->get_datastream_dissemination('DC');
    //return 'test';
    $xml = new SimpleXMLElement($dc);
    $returnValue = '';
    foreach ($xml->children('http://purl.org/dc/elements/1.1/') as $key => $entry) {
      if ($key == 'title' || $key == 'description') {//get the dc description and or title
        $returnValue .= ' ' . (string) $entry;
      }
    }
    return $returnValue;
  }

}

?>
