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
      $html .= '<a href="'.$t.'"/>'.$t . '</a><br />';
    }
    $html .= $this->htmlSnippet("Contact", $contactPerson);
    $html .= $this->htmlSnippet("Citation", $resdesc);
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
	//lines added by Biva
	$lineage = $this->getLineage( $this->pid );
	$versions = $this->getVersionTrails($this->pid );
	// ends here
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
      // #type and #title are the minimum requirements.
      '#type' => 'tabpage',
      '#title' => t('Items'),
      // This will be the content of the tab.
      '#content' => $streams,
	);
    $tabset['my_tabset'][1] = array(
      '#type' => 'tabpage',
      '#title' => t('Metadata'),
      '#content' => $fdgcAsHtml,
    );

    $tabset['my_tabset'][2] = array(
      // #type and #title are the minimum requirements.
      '#type' => 'tabpage',
      '#title' => t('Description'),
      // This will be the content of the tab.
      '#content' => $content,
    );
    
   
	// Lines added by Biva
	$fieldSetcontent = "";
    if( $lineage!=null)
    {
    	$fieldSetP1 = array(
          '#title' => t("Data History"),
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
          '#value' => $lineage,
    	);
    	$fieldSetcontent = theme('fieldset', $fieldSetP1);
    	$fieldSetP2 = array(
	          '#title' => t("Audit History"),
	          '#collapsible' => TRUE,
	          '#collapsed' => TRUE,
	          '#value' => $versions,
	    );
	    $fieldSetcontent .= theme('fieldset', $fieldSetP2);
	    
    }
    else {
	    $fieldSetP2 = array(
	          '#title' => t("Audit History"),
	          '#collapsible' => TRUE,
	          '#collapsed' => FALSE,
	          '#value' => $versions,
	    );
	    $fieldSetcontent .= theme('fieldset', $fieldSetP2);	    
    }
    $tabset['my_tabset'][3]=array(
	   
	      '#type' => 'tabpage',      
	      '#title' => t('History'),    
	      '#content'=>$fieldSetcontent,
	    );
    // lines added ends here
    return tabs_render($tabset);
  }
  //functions added By Biva
   public function getLineage( $pid )
  {
 	 module_load_include('inc', 'fedora_repository', 'api/fedora_utils');  
     $encodedPID = str_replace(":", "%3A", $pid);
  	 $query_string = "select%20%24subject%20'isChildOf'%20%24object%0Afrom%20%3C%23ri%3E%0Awhere%20walk(%0A%20%20%20%20%20%20%20%20%24subject%20%3Cinfo%3Afedora%2Ffedora-system%3Adef%2Frelations-external%23isDerivedFrom%3E%20%3Cinfo%3Afedora%2F";
  	 $query_string .= $encodedPID;
  	 //A%3A
  	 $query_string .="%3E%0A%20%20%20%20%20%20%20%20and%20%24subject%20%3Cinfo%3Afedora%2Ffedora-system%3Adef%2Frelations-external%23isDerivedFrom%3E%20%24object)";

     $url1 = variable_get('fedora_repository_url', 'http://localhost:8080/fedora/risearch');
     $url1 .= "?type=tuples&flush=true&format=csv&lang=itql&stream=on&query=";
     $url1 .= $query_string;
     $content = do_Curl($url1);
     $rows = explode("\n", $content);
     $lineage = null;
     if( count($rows)>2 )
     {
	     $lineage = '<table border="2" cellpadding="5px" style="margin_bottom:5;"><th>Children</th><th>Relation</th><th>Parent</th>';
	     $count=0;
	     foreach ( $rows as $row )
	     {
	     	if ( $count > 0 && $row != null)
	     	{
		     	$lineage.= "<tr>";
		     	$entities = explode(",", $row);
		     	foreach ($entities as $entity)
		     	{
		      		$lineage.= "<td>";
		     		$entity = str_replace("info:fedora/", "", $entity);
		     		$lineage.= $entity;
		     		//echo "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
		     		$lineage.= "</td>";     		
		     	}
		        $lineage.= "</tr>" ;
	     	}
	     	$count = $count + 1;      
	     }
	     $lineage.= "</table>";
     }
     
     $query_string = "select%20%20%24object'isParentof'%20%24subject%0Afrom%20%3C%23ri%3E%0Awhere%20walk(%0A%20%20%20%20%20%20%20%20%3Cinfo%3Afedora%2F";
  	 $query_string .= $encodedPID;
  	 //A%3A
  	 $query_string .="%3E%20%3Cinfo%3Afedora%2Ffedora-system%3Adef%2Frelations-external%23isDerivedFrom%3E%20%24object%20%0A%20%20%20%20%20%20%20%20and%20%24subject%20%3Cinfo%3Afedora%2Ffedora-system%3Adef%2Frelations-external%23isDerivedFrom%3E%20%24object%20)";

     $url2 = variable_get('fedora_repository_url', 'http://localhost:8080/fedora/risearch');
     $url2 .= "?type=tuples&flush=true&format=csv&lang=itql&stream=on&query=";
     $url2 .= $query_string;
     $content = do_Curl($url2);
     $rows = explode("\n", $content);
     if( count($rows)>2 )
     {
	     $lineage.= '<table border="2" cellpadding="5px" float:left;"><th>Children</th><th>Relation</th><th>Parent</th>';
	     $count=0;
	     foreach ( $rows as $row )
	     {
	     	if ( $count > 0 && $row != null )
	     	{
		     	$lineage.= "<tr>";
		     	$entities = explode(",", $row);
		     	foreach ($entities as $entity)
		     	{
		      		$lineage.= "<td>";
		     		$entity = str_replace("info:fedora/", "", $entity);
		     		$lineage.= $entity;
		     		//echo "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
		     		$lineage.= "</td>";     		
		     	}
		        $lineage.= "</tr>" ;
	     	}
	     	$count = $count + 1;      
	     }
	     $lineage.= "</table>";
     }
      //curl_exec($ch);
     //drupal_set_message(t("Invalid dsID!"));
     return $lineage;
    
  }
  function getVersionTrails( $pid )
  {
  	$item = new Fedora_Item( $pid );
    $foxml = $item->export_as_foxml();
    $audit_section = explode("<audit:auditTrail xmlns:audit=\"info:fedora/fedora-system:def/audit#\">",$foxml);
    $audit = explode("</audit:auditTrail>" , trim($audit_section[1]));
    $sections = explode("</audit:record>",trim($audit[0]));
    $trails = '<table border="2" cellpadding="5px" float:left;">
    <th>Count</th><th>Timestamp</th><th>component</th><th>Action</th><th>Justification</th>';
    $count = count($sections);  
    $run = 1;  
    
    
    foreach($sections as $section)
    {
    	$tags = explode("\n",trim($section));
    	$timestamp = substr_replace("<audit:date>","",$timestamp);
    	$timestamp = substr_replace("</audit:date>","",$timestamp);
		if(strcmp($section,$sections[$count])!=0)
    	{
	    	$trails .= "<tr><td>";
	    	$timestamp = $tags[5];   	
    		$trails .= $run."</td><td>";    	
	    	$trails .= $timestamp."</td><td>";
	    	$componentId = $tags[3];
	    	$trails .= $componentId."</td><td>";
	    	$action = $tags[2];
	    	$trails .= $action."</td><td>";
	    	$Justification = $tags[6];
	    	$trails .= $Justification."</td></tr>";
	    	$run+=1;
    	}  
    }
    $trails .= "</table>"; 
    return $trails;
  }


}