<?php

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

class EsdoraCollection extends CollectionClass {

  function __construct($pid = '') {
    module_load_include('inc', 'fedora_repository', 'api/fedora_item');
    module_load_include('inc', 'fedora_repository', 'CollectionClass');
    if (!empty($pid)) {
      $this->pid = $pid;
      $this->item = new Fedora_Item($pid);
    }
  }

  function showCollection() {
    global $base_url;
    global $user;
    $path = drupal_get_path('module', 'esdora_islandora');

    $dc = $this->item->get_datastream_dissemination('DC');
    //return 'test';
    $xml = new SimpleXMLElement($dc);
    $returnValue = '';
    $collectionName = '';
    foreach ($xml->children('http://purl.org/dc/elements/1.1/') as $key => $entry) {
      if ($key == 'description') {//get the dc description and or title
        $returnValue .= ' ' . (string) $entry;
      }
      if ($key == 'title') {//get the dc description and or title
        $collectionName = (string) $entry;
      }
    }
    
    $item = new Fedora_Item($this->pid);
    $query = NULL;
    if ($item->exists() && array_key_exists('QUERY', $item->datastreams)) {
      $query = $item->get_datastream_dissemination('QUERY');
    }
    
    $collectionList='';
    $content = $this->getRelatedItems($this->pid, $query);
    
    $xslContent = $this->getXslContent($this->pid, $path);

    if (isset($content) && $content != FALSE) {
      $input = new DomDocument();
      $input->loadXML(trim($content));
      $results = $input->getElementsByTagName('result');
      if ($results->length > 0) {
        try {
          $proc = new XsltProcessor();
          $proc->setParameter('', 'collectionPid', $this->pid);
          $proc->setParameter('', 'collectionTitle', $collectionName);
          $proc->setParameter('', 'baseUrl', $base_url);
          $proc->setParameter('', 'path', $base_url . '/' . $path);
          $proc->setParameter('', 'hitPage', $pageNumber);
          $proc->registerPHPFunctions();
          $xsl = new DomDocument();
          $xsl->loadXML($xslContent);
          $xsl = $proc->importStylesheet($xsl);
          $newdom = $proc->transformToDoc($input);

          $objectList = $newdom->saveXML(); //is the xml transformed to html as defined in the xslt associated with the collection object

          if (!$objectList) {
            throw new Exception("Invalid XML.");
          }
          $collectionList=$objectList;
        } catch (Exception $e) {
          drupal_set_message(t('!e', array('!e' => $e->getMessage())), 'error');
          return '';
        }
      }
    } else {
      drupal_set_message(t("No Objects in this collection or bad query."));
    }

    return $returnValue . $objectList;
  }

 function getXslContent($pid, $path, $canUseDefault = TRUE) {
    module_load_include('inc', 'fedora_repository', 'CollectionClass');
    $collectionClass = new CollectionClass();
    $xslContent = $collectionClass->getCollectionViewStream($pid);
    if (!$xslContent && $canUseDefault) { //no xslt so we will use the default sent with the module
      $xslContent = file_get_contents($path . '/xsl/sparql_to_html.xsl');
    }
    return $xslContent;
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
