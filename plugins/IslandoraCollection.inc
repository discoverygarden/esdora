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
      if ($key == 'description') {//get the dc description and or title
        $returnValue .= ' ' . (string) $entry;
      }
    }
    return $returnValue;
  }

  
  
  //TODO: I want to use this query instead, where I put in code, or in query ds of collection object?
  
  /*  	$itqlQuery = 'select $object $title $content from <#ri>
  	
                             where ($object <dc:title> $title
                             and $object <fedora-model:hasModel> $content
                             and ($object <fedora-rels-ext:isMemberOfCollection> <info:fedora/' . $pid . '>
                             
                             and $object <fedora-model:state> <info:fedora/fedora-system:def/model#Active>)
                             minus $content <mulgara:is> <info:fedora/fedora-system:FedoraObject-3.0>
                             order by $title'; //note: compared to the default query (see CollectionClass->getRelatedItems(), 
  											   //this query took out or $object <fedora-rels-ext:isMemberOf> <info:fedora/' . $pid . '>/)
  											   //because we want to preserve "isMemberOF" for atomic content model	
  	
  */
}

?>
