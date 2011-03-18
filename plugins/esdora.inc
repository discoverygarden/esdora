<?php

// $Id$

class Esdora {
/**
 *
 * @param <type> $pid
 */
  function __construct($pid = '') {
    module_load_include('inc', 'fedora_repository', 'api/fedora_item');
    module_load_include('inc', 'fedora_repository', 'CollectionClass');
    if (!empty($pid)) {
      $this->pid = $pid;
      $this->item = new Fedora_Item($pid);
    }
  }
/**
 *
 * @param <type> $fieldName
 * @param <type> $value
 * @return string
 */
  function htmlSnippet($fieldName, $value) {
    $html = '</td></tr>';
    $html .='<tr><td valign="top" align="right" class="mods_field"><strong>' . $fieldName . '</strong></td><td>';
    if (!empty($value)) {
      foreach ($value as $va) {
        $html .= $va . '<br />';
      }
      return $html;
    }
    else {
      return '';
    }
  }
/**
 *
 * @param <type> $xmlString
 * @return string
 */
  function getFdgcAsHtml($xmlString) {
    if(empty($xmlString)){
      return "";
    }
    $fdgcXml = new SimpleXMLElement($xmlString);
    $title = $fdgcXml->xpath('//title');
    $links = $fdgcXml->xpath('//onlink');
    $resdesc = $fdgcXml->xpath('//resdesc');
    $contactPerson = $fdgcXml->xpath('//cntper');
    $html = '';

    $html .= '<table align="left"><tr><td valign="top" align="right" class="fedora_mods_field" width="15%">';
    $html .='<strong>Title</strong></td><td>';
    foreach ($title as $t) {
      $html .= $t . '<br />';
    }
    $html .= '</td></tr><tr><td valign="top" align="right"><strong>Links</strong></td><td>';
    foreach ($links as $t) {
      $html .= '<a href="'.$t.'" target="_blank"/>'.$t . '</a><br />';
    }
    $html .= $this->htmlSnippet("Contact", $contactPerson);
    $html .= $this->htmlSnippet("Description", $resdesc);
    $html .='</td></tr></table></div></div><br />';
    return $html;
  }

  //finds datastreams that have a certain mimetype
  //excludes ones that contain any strings in the array $exclude
  /**
   *
   * @global <type> $base_url
   * @param <type> $mimeTypeToRetrieve
   * @param <type> $exclude
   * @return string
   */
  function getListOfStreams($mimeTypeToRetrieve, $exclude=null) {
    // $list_of_streams = array();
    global $base_url;
    $html = '';
    $this->datastreams_list = $this->item->get_datastreams_list_as_SimpleXML();
    if ($this->datastreams_list != FALSE) {
      foreach ($this->datastreams_list->datastreamDef as $ds) {
        if ($mimeTypeToRetrieve == $ds->MIMEType || $mimeTypeToRetrieve == '*') {
          // $list_of_streams[] = $ds;
          $include = TRUE;
          if (isset($exclude)) {
            foreach ($exclude as $value) {
              //var_dump($value);var_dump($ds->ID);exit();
              if (!empty($value[0]) && stristr($ds->ID, $value[0])) {
                $include = false;
              }
            }
          }
          if ($include) {
            $label = $ds->label;
            if (empty($label)) {
              $label = $ds->ID;
            }
            $html .= '<a href = "' . $base_url . '/fedora/repository/' . $this->pid . '/' . $ds->ID . '/' . drupal_urlencode($label) . '">' . $ds->label . '</a><br />';
          }
        }
      }
    }
    //return $list_of_streams;
    return $html;
  }

  //finds datastreams that have a certain mimetype
  //excludes ones that contain any strings in the array $exclude
  //returns an array of streams we are interested in
  /**
   *
   * @global  $base_url
   * @param <type> $mimeTypeToRetrieve
   * @param <type> $exclude
   * @return string
   */
  function getListOfStreamsRaw($mimeTypeToRetrieve, $exclude=null) {
    $list_of_streams = array();
    global $base_url;
    $html = '';

    if ($this->datastreams_list != FALSE) {
      foreach ($this->datastreams_list->datastreamDef as $ds) {
        if ($mimeTypeToRetrieve == $ds->MIMEType || $mimeTypeToRetrieve == '*') {
          // $list_of_streams[] = $ds;
          $include = TRUE;
          if (isset($exclude)) {
            foreach ($exclude as $value) {
              if (stristr($ds->ID, $value[0])) {
                $include = false;
              }
            }
          }
          if ($include) {
            $label = $ds->label;
            if (empty($label)) {
              $label = $ds->ID;
            }
            $list_of_streams[] = $ds->ID . '/' . $label;
          }
        }
      }
    }

    return $list_of_streams;
  }
/**
 *
 * @param <type> $results
 * @return <type> 
 */
  public function getListOfStreamsFromItql($results) {
    $list_of_streams[] = array();
    try {
      $xml = new SimpleXMLElement($results);
    } catch (Exception $e) {
      drupal_set_message(t('Error getting rels-int for list of metadata !e', array('!e' => $e->getMessage())), 'error');
      return;
    }
    foreach ($xml->results->result as $result) {
      $a = $result->datastream->attributes();
      $temp = $a['uri'];
      $dsid = substr($temp, strrpos($temp, '/') + 1);
      $mimeType = $result->mimeType;
      $dsInfo = $this->item->get_datastream_info($dsid);
      $list_of_streams[] = array($dsid, (string) $mimeType,$dsInfo->datastream->controlGroup);
    }
    return $list_of_streams;
  }

  /**
   * this is function is referenced in the bigfoot cmodel and creates the object level diplay
   *
   * This function iterates through the list of datastreams and creates fieldsets within
   * tabs to display the text and pdf streams.  another tab just lists all the datastreams
   *
   * @return returns a drupal tab tabset
   */
  public function showFieldSets() {
    global $base_url;
    $objectHelper = new ObjectHelper();
    //global $base_url;
    global $user;
    $fdgcText = $this->item->get_datastream_dissemination("FGDC.xml");
    $fdgcAsHtml = $this->getFdgcAsHtml($fdgcText);

    $tabset = array();

    $tabset['my_tabset'] = array(
      '#type' => 'tabset',
    );
    $itqlQuery = 'select $datastream  $mimeType from <#ri> where $datastream <http://esdora.ornl.gov/ontologies/relationships#isMetaDataFor>
      <info:fedora/' . $this->pid . '> and $datastream <info:fedora/fedora-system:def/view#mimeType> $mimeType'; //<info:fedora/Bigfoot:LAI_surfaces>
    $collectionClass = new CollectionClass();
    $itqlResults = $collectionClass->getRelatedItems($this->pid, $itqlQuery, $limit = NULL, $offset = NULL);
    global $user;
    $metadataStreams = $this->getListOfStreamsFromItql($itqlResults);
    $metadataStreams[]='DC';
    $metadataStreams[]='RELS-EXT';
    $metadataStreams[]='RELS-INT';
    $streams = $this->getListOfStreams('*',$metadataStreams);
    $content = '';
    foreach ($metadataStreams as $streamInfo) {
      if (!empty($streamInfo)) {
        $dsid = $streamInfo[0];
        $txtValue = $this->item->get_datastream_dissemination($dsid);
        $mimeType = $streamInfo[1];
        if ($mimeType == 'application/pdf') {
          $pdfUrl = '<div class="text"><iframe src="http://docs.google.com/viewer?url='.$base_url.'/fedora/repository/' .
              $this->pid . '/' . $dsid . '/preview.pdf' . '&embedded="TRUE" style="width:600px; height:500px;" frameborder="0"></iframe></div>';
          $fieldSet = array(
            '#title' => $dsid,
            '#attributes' => array(),
            '#collapsible' => TRUE,
            '#collapsed' => TRUE,
            '#value' => $pdfUrl,
          );
          $content .= theme('fieldset', $fieldSet);
        }
        else if (!empty($txtValue)) {
          if($streamInfo[2]==='R' || $streamInfo[2]==='E'){
            $val = '<div class="text"><a href="'.$txtValue.'">' . $txtValue . '</a></div>';
          } else {
            $val = '<div class="text"><pre>' . $txtValue . '</pre></div>';
          }
          $fieldSet = array(
            '#title' => $dsid,
            '#attributes' => array(),
            '#collapsible' => TRUE,
            '#collapsed' => TRUE,
            '#value' => $val
          );
          $content .= theme('fieldset', $fieldSet);
        }
      } 
    }
    $tabset['my_tabset'][0] = array(
      '#type' => 'tabpage',
      '#title' => t('Metadata'),
      '#content' => $fdgcAsHtml,
    );

    $tabset['my_tabset'][1] = array(
      // #type and #title are the minimum requirements.
      '#type' => 'tabpage',
      '#title' => t('Data Citation'),
      // This will be the content of the tab.
      '#content' => $content,
    );
    $tabset['my_tabset'][2] = array(
      // #type and #title are the minimum requirements.
      '#type' => 'tabpage',
      '#title' => t('Items'),
      // This will be the content of the tab.
      '#content' => $streams,
    );


    return tabs_render($tabset);
  }

}