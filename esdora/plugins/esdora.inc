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

    $path = drupal_get_path('module', 'esdora_islandora');
  	$path_compound=$path . '/ajaxtabs_compound.js';
  	drupal_add_js($path_compound); 
	$path_compound=$path . '/ajaxtabs.css';
  	drupal_add_js($path_compound);    
  }
    /**
   * this is function is referenced in the esdora_data_xxx_cmodel and creates the object level diplay
   *
   * This function iterates through the list of datastreams and creates fieldsets within
   * tabs to display the text and pdf streams.  another tab just lists all the datastreams
   *
   * @return returns a drupal tab tabset
   */
  public function showFieldSets() {
  	/*$path = drupal_get_path('module', 'esdora_islandora');
  	$path.='/ajaxtabs.js';
  	drupal_add_js($path);
  	$tabset['my_tabset'] = array(
      '#type' => 'tabset',
    );
  	 $tabset['my_tabset'][0] = array(
      // #type and #title are the minimum requirements.
      '#type' => 'tabpage',
      '#title' => t('Items'),
      // This will be the content of the tab.
      '#content' => $this->getItems(),
	);
    $tabset['my_tabset'][1] = array(
      '#type' => 'tabpage',
      '#title' => t('Metadata'),
      '#content' =>$this->getMetadata(),
    );

    $tabset['my_tabset'][2] = array(
      // #type and #title are the minimum requirements.
      '#type' => 'tabpage',
      '#title' => t('Description'),
      // This will be the content of the tab.
      '#content' =>$this->getDescription(),
    );
    $tabset['my_tabset'][3] = array(
      // #type and #title are the minimum requirements.
      '#type' => 'tabpage',
      '#title' => t('History'),
      // This will be the content of the tab.
      '#content' => $this->getHistory(),
    );   
    return tabs_render($tabset);
  */  
  	$items = $this->getItems();
	//$collectionClass = new CollectionClass(); 
  	//$viewerQuery = 'select $datastream  $mimeType from <#ri> where $datastream <http://esdora.ornl.gov/ontologies/relationships#isViewerFor>
     // <info:fedora/' . $this->pid . '> and $datastream <info:fedora/fedora-system:def/view#mimeType> $mimeType';
     
  	//$viewerResults = $collectionClass->getRelatedItems($this->pid, $viewerQuery, NULL, NULL);   
  	//$viewerStreams = $this->getListofStreamsFromItql($viewerResults);
  	$cont= '<button class="Items" style=" background-color: #D7D6DB;">Items</button>';
  	$cont.= '<button class="Metadata" style="background-color: #F4F4F4">Metadata</button>';
  	$cont.= '<button class="Description" style="background-color: #F4F4F4">Description</button>'; 
  	$cont.= '<button class="History" style="background-color: #F4F4F4">History</button>'; 
	$viewerContent = $this->getViewers();
		if($viewerContent!=null)
  	{
  		$cont.= '<button class="Viewers" style="background-color: #F4F4F4">Viewers</button>'; 			
  	
  	}  	
  	$cont.= '<div id="div1" >'.$items.'</div>';  	
  	$cont.='<input type="hidden" id="pid" value="'.$this->pid.'"/>';
	return $cont;
  }  
  
  
  //============ START: KEY TAB FUNCTIONS  ======================================================//
  
  function getViewers()
  {
  	$collectionClass = new CollectionClass(); 
  	$viewerQuery = 'select $datastream  $mimeType from <#ri> where $datastream <http://esdora.ornl.gov/ontologies/relationships#isViewerFor>
      <info:fedora/' . $this->pid . '> and $datastream <info:fedora/fedora-system:def/view#mimeType> $mimeType';
     
  	$viewerResults = $collectionClass->getRelatedItems($this->pid, $viewerQuery, NULL, NULL);   
  	$viewerStreams = $this->getListofStreamsFromItql($viewerResults);
  	$viewerContent = $this->createViewerContent($viewerStreams);
  	return $viewerContent;
  }
  function getItems() //Items tab
  {
  	global $base_url;
  	$collectionClass = new CollectionClass();
  	
  	$itqlViewerQuery='select $datastream  $mimeType from <#ri> where $datastream <http://esdora.ornl.gov/ontologies/relationships#isViewerFor>
      <info:fedora/' . $this->pid . '> and $datastream <info:fedora/fedora-system:def/view#mimeType> $mimeType';

    $itqlMetaDataQuery = 'select $datastream  $mimeType from <#ri> where $datastream <http://esdora.ornl.gov/ontologies/relationships#isMetaDataFor>
      <info:fedora/' . $this->pid . '> and $datastream <info:fedora/fedora-system:def/view#mimeType> $mimeType'; 
  	
    $itqlViwerResults = $collectionClass->getRelatedItems($this->pid, $itqlQuery, $limit = NULL, $offset = NULL);
    $itqlMetadataResults = $collectionClass->getRelatedItems($this->pid, $itqlMetaDataQuery, $limit = NULL, $offset = NULL);
  	
  	$ds_list_viewers = $this->getListOfStreamsFromItql($itqlViwerResults);
  	$ds_list_metadata = $this->getListOfStreamsFromItql($itqlMetadataResults);
  	
  	
  	$non_data_streams = array_merge($ds_list_viewers, $ds_list_metadata);
    $non_data_streams[] = array('DC', 'text/xml', 'X');
    $non_data_streams[] = array('RELS-EXT', 'text/xml', 'X');
    $non_data_streams[] = array('RELS-INT', 'text/xml', 'X');
    //drupal_set_message('<pre>'. print_r($non_data_streams, TRUE) .'</pre>');
    $streams = $this->getListOfStreams('*', $non_data_streams);
    
    return $streams;
  }
  
  function getMetadata() //metadata tab
  {
	$fdgcText;
        $fdgcAsHtml;

	try {
  	$fdgcText = $this->item->get_datastream_dissemination("FGDC.xml");
  	$fdgcAsHtml = $this->getFdgcAsHtml($fdgcText); 	
        return $fdgcAsHtml;
  	}
	catch (Exception $e) {
	  $msg = 'caught exception: ' . $e->getMessage();
	  $msg .= '; fgdc='.$fdgcText.'; html='.$fdgcAsHtml.'; pid='.$this->item->pid; 	
	  return $msg;	
	}

  	
  }
  
  function getDescription()
  {
	if(module_exists("fancybox"))
		return $this->getDescriptionWFB();
	else
		return $this->getDescriptionWOFB();
		
	
  }
  
  function getDescriptionWFB() //description tab
  {
  	
	
	global $base_url;
  	global $user;
  	$content = '<table>';
  	$collectionClass = new CollectionClass();
  	$itqlQuery = 'select $datastream  $mimeType from <#ri> where $datastream <http://esdora.ornl.gov/ontologies/relationships#isMetaDataFor>
    <info:fedora/' . $this->pid . '> and $datastream <info:fedora/fedora-system:def/view#mimeType> $mimeType'; //<info:fedora/Bigfoot:LAI_surfaces>
  	$itqlResults = $collectionClass->getRelatedItems($this->pid, $itqlQuery, $limit = NULL, $offset = NULL);
  	$metadataStreams = $this->getListOfStreamsFromItql($itqlResults);
    $config = array('indent' => TRUE,
      'output-xml' => TRUE,
      'wrap' => 200);
    foreach ($metadataStreams as $streamInfo) {
      if (!empty($streamInfo)) {
        $dsid = $streamInfo[0];
        $txtValue = $this->item->get_datastream_dissemination($dsid);
        $mimeType = $streamInfo[1];
        //$content.=$mimeType;
        if ($mimeType == 'application/pdf'||$mimeType == 'application/msword') 
        {
       		$url= 'http://docs.google.com/viewer?url='.$base_url.
       		'/fedora/repository/' . $this->pid .'/'. $dsid.'&embedded=TRUE';
       		$downloadURL = $base_url.'/fedora/repository/' . $this->pid.'/'. $dsid;
       		
        	//$pdf = '<div class="text"><a class="fb_dynamic" href='.$url.' >'.$dsid.'</a></div>';
        	$pdf = '<td>'.$dsid.'</td><td align="center"><a class="fb_dynamic" href='.$url.' ><button type="button">View</button></a></td>'.
        	'<td align="center"><a href='.$downloadURL.' target="_blank"><button type="button">Download</button></a></td>';
        	$content.='<tr>'.$pdf.'</tr>';
        }
        else if($mimeType == 'image/jpeg')
        {
        	$url= $base_url.'/fedora/repository/' . $this->pid .'/'. $dsid;
        	//$url="http://www.google.com";
        	//$img = '<div class="text"><a class="fb_dynamic" href='.$url.'>'.$dsid.'</a></div>';
        	$img = '<td>'.$dsid.'</td><td align="center"><a class="fb_dynamic" href='.$url.' ><button type="button">View</button></a></td>'.
        	'<td align="center"><a href='.$url.' target="_blank"><button type="button">Download</button></a></td>';
        	
        	$content.='<tr>'.$pdf.'</tr>';
        	
        }
        else if (!empty($txtValue)) {
          if ($streamInfo[2] === 'R' || $streamInfo[2] === 'E') {
          	$val = '<div class="text"><a href="' . $txtValue . '">' . $txtValue . '</a></div>';
          }
          else {
            if ($mimeType == 'text/xml'||$mimeType == 'text/plain') {
            //  $val = '<div class="text"><pre>' . tidy_repair_string($txtValue, $config, 'UTF8') . '</pre></div>';
              //  $val = '<div class="text"><pre>' . $txtValue . '</pre></div>';
             	$url= $base_url.'/fedora/repository/' . $this->pid .'/'. $dsid;
             	
        	//$url="http://www.google.com";
        		//$val = '<div class="text"><a class="fb_dynamic" href='.$url.' >'.$dsid.'</a></div>';
        		$val = '<td>'.$dsid.'</td><td align="center"><a class="fb_dynamic" href='.$url.' ><button type="button">View</button></a></td>'.
        	'<td align="center"><a href='.$url.' target="_blank"><button type="button">Download</button></a></td>';
        	
            }
            else {
              //$val = '<div class="text"><pre>' . $txtValue . '</pre></div>';
              $url= $base_url.'/fedora/repository/' . $this->pid .'/'. $dsid;
        	//$url="http://www.google.com";
        	  //$val = '<div class="text"><a href='.$url.'>'.$dsid.'</a></div>';
        	  $val = '<td>'.$dsid.'</td><td align="center"><a class="fb_dynamic" href='.$url.' ><button type="button">View</button></a></td>'.
        	'<td align="center"><a href='.$url.' target="_blank"><button type="button">Download</button></a></td>';
            }
          }
          $content.='<tr>'.$val.'</tr>';
        }
      }
    }
   return $content;
  }
function getDescriptionWOFB() //description tab
  {
  	global $base_url;
  	global $user;
  	$content = '<ul>';
  	$collectionClass = new CollectionClass();
  	$itqlQuery = 'select $datastream  $mimeType from <#ri> where $datastream <http://esdora.ornl.gov/ontologies/relationships#isMetaDataFor>
    <info:fedora/' . $this->pid . '> and $datastream <info:fedora/fedora-system:def/view#mimeType> $mimeType'; //<info:fedora/Bigfoot:LAI_surfaces>
  	$itqlResults = $collectionClass->getRelatedItems($this->pid, $itqlQuery, $limit = NULL, $offset = NULL);
  	$metadataStreams = $this->getListOfStreamsFromItql($itqlResults);
    $config = array('indent' => TRUE,
      'output-xml' => TRUE,
      'wrap' => 200);
    foreach ($metadataStreams as $streamInfo) {
      if (!empty($streamInfo)) {
        $dsid = $streamInfo[0];
        $txtValue = $this->item->get_datastream_dissemination($dsid);
        $mimeType = $streamInfo[1];
        //$content.=$mimeType;
        if ($mimeType == 'application/pdf'||$mimeType == 'application/msword') 
        {
       		$url= 'http://docs.google.com/viewer?url='.$base_url.
       		'/fedora/repository/' . $this->pid .'/'. $dsid.'&embedded=TRUE';
       		
        	$pdf = '<div class="text"><a href='.$url.' target="_blank">'.$dsid.'</a></div>';
        	$content.='<li>'.$pdf.'</li>';
        }
        else if($mimeType == 'image/jpeg')
        {
        	$url= $base_url.'/fedora/repository/' . $this->pid .'/'. $dsid;
        	//$url="http://www.google.com";
        	$img = '<div class="text"><a href='.$url.' target="_blank">'.$dsid.'</a></div>';
        	$content.='<li>'.$img.'</li>';
        	
        }
        else if (!empty($txtValue)) {
          if ($streamInfo[2] === 'R' || $streamInfo[2] === 'E') {
          	$val = '<div class="text"><a href="' . $txtValue . '">' . $txtValue . '</a></div>';
          }
          else {
            if ($mimeType == 'text/xml'||$mimeType == 'text/plain') {
            //  $val = '<div class="text"><pre>' . tidy_repair_string($txtValue, $config, 'UTF8') . '</pre></div>';
              //  $val = '<div class="text"><pre>' . $txtValue . '</pre></div>';
             	$url= $base_url.'/fedora/repository/' . $this->pid .'/'. $dsid;
        	//$url="http://www.google.com";
        		$val = '<div class="text"><a  href='.$url.' target="_blank">'.$dsid.'</a></div>';
            }
            else {
              //$val = '<div class="text"><pre>' . $txtValue . '</pre></div>';
              $url= $base_url.'/fedora/repository/' . $this->pid .'/'. $dsid;
        	//$url="http://www.google.com";
        	  $val = '<div class="text"><a href='.$url.' target="_blank">'.$dsid.'</a></div>';
            }
          }
          $content.='<li>'.$val.'</li>';
        }
      }
    }
   return $content;
  }

  function getHistory() //histroy tab
  {
	  	$fieldSetcontent = "<div>";
	  	$pid = $this->pid;
	  	if( module_exists('graphviz_filter'))
	  	{
	  		$lineage = $this->getLineageAsGraph($pid);
	  	}
	  	else 
			$lineage= $this->getLineage($pid);
		$versions = $this->getVersionTrails($pid);
	    if( $lineage!=null)
	    {
	    	$fieldSetP1 = array(
	          '#title' => t("Data History"),
	          '#collapsible' => TRUE,
	          '#collapsed' => TRUE,
	          '#value' => $lineage,
	    	);
	    	$fieldSetcontent.= theme('fieldset', $fieldSetP1);
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
		    $fieldSetcontent.= theme('fieldset', $fieldSetP2);	    
	    }
	    $fieldSetcontent.='</div>';
	   return $fieldSetcontent;	    
  }
  
  //============ END KEY TAB FUNCTIONS  ======================================================//
  
  
  
  
  //============ START HELP FUNCTIONS ========================================================//
  //functions added By Biva
  function getLineageAsGraph($pid) {
  	
  	global $base_url;
  	
    module_load_include('inc', 'fedora_repository', 'api/fedora_utils');
    $encodedPID = str_replace(":", "%3A", $pid);
     
   
   
    
    $query_string = "select%20%20%24subject'DerivedFrom'%20%24object%0Afrom%20%3C%23ri%3E%0Awhere%20walk(%0A%20%20%20%20%20%20%20%20%3Cinfo%3Afedora%2F";
    $query_string .= $encodedPID;
    $query_string .="%3E%20%3Cinfo%3Afedora%2Ffedora-system%3Adef%2Frelations-external%23isDerivedFrom%3E%20%24object%20%0A%20%20%20%20%20%20%20%20and%20%24subject%20%3Cinfo%3Afedora%2Ffedora-system%3Adef%2Frelations-external%23isDerivedFrom%3E%20%24object%20)";

    $url1 = variable_get('fedora_repository_url', 'http://localhost:8080/fedora/risearch');
    $url1 .= "?type=tuples&flush=true&format=csv&lang=itql&stream=on&query=";
    $url1 .= $query_string;
    $content = do_Curl($url1);
    $rows = explode("\n", $content);
    $lineage = null;
    $isnull = true;
    $html = '<div style="width:inherit;">';
    $process = array();
    $processName = array();
    $cnt = 0;
    $graph .= 'digraph G{rankdir=LR node[shape=ellipse,fontsize=10,style=filled,color=lightblue2,fontname = "Helvetica-Outline"];';
    if (count($rows) > 2) {
      $count = 0;
      $isnull = false;
      foreach ($rows as $row) 
      { 
      	
      	if ($count > 0 && $row != null) {
      		$entities = explode(",", $row);
      		 
      		 $name = 'process'.($cnt+1);
      		 $subject = str_replace("info:fedora/", "", $entities[0]);
      		 $object = str_replace("info:fedora/", "", $entities[2]);
      		
      		 $process[$cnt]=$subject;
			 $processName[$cnt]=$name.': '.$object.'->'.$subject;
      		 $subjects = explode(".",$subject);
	      	 $subject = $subjects[count($subjects)-1];
      		 $objects = explode(".",$object);
	      	 $object = $objects[count($objects)-1];
			 $graph .= $object.'->'.$subject;
			 $label='[label="'.$name.'",fontsize=10, fontname = "Helvetica-Outline"];';
			 
			 $graph.=$label;
			$cnt.=+1;
			 //$graph.=$row;
      	}
      	$count = $count + 1;
      }
      
    }
	
	//$lineage .= $graph;
    $query_string = "select%20%24subject%20'DerivedFrom'%20%24object%0Afrom%20%3C%23ri%3E%0Awhere%20walk(%0A%20%20%20%20%20%20%20%20%24subject%20%3Cinfo%3Afedora%2Ffedora-system%3Adef%2Frelations-external%23isDerivedFrom%3E%20%3Cinfo%3Afedora%2F";
    $query_string .= $encodedPID;
    //A%3A
    $query_string .="%3E%0A%20%20%20%20%20%20%20%20and%20%24subject%20%3Cinfo%3Afedora%2Ffedora-system%3Adef%2Frelations-external%23isDerivedFrom%3E%20%24object)";

    $url2 = variable_get('fedora_repository_url', 'http://localhost:8080/fedora/risearch');
    $url2 .= "?type=tuples&flush=true&format=csv&lang=itql&stream=on&query=";
    $url2 .= $query_string; 
    $content = do_Curl($url2);
    $rows = explode("\n", $content);
  	if (count($rows) > 2) 
  	{	      
      $count = 0;
      $isnull = false;
      foreach ($rows as $row) 
      { 
      	if ($count > 0 && $row != null) {
      		$name = 'process'.($cnt+1);
      		 $entities = explode(",", $row);
      		 $subject = str_replace("info:fedora/", "", $entities[0]);
      		  $object = str_replace("info:fedora/", "", $entities[2]);
      		
      		 $process[$cnt]=$subject;
			  $processName[$cnt]=$name.': '.$object.'->'.$subject;
      		 $subjects = explode(".",$subject);
      		 $subject = $subjects[count($subjects)-1];
      		 $objects = explode(".",$object);
      		 $object = $objects[count($objects)-1];
			 $graph .= $object.'->'.$subject;
			 $label='[label="'.$name.'",fontsize=10, fontname = "Helvetica-Outline"];';
			 $graph.=$label;
			 $cnt.=+1;
			 //$graph.=$row;
      	}
      	$count = $count + 1;
      }
     
    }
	$graph .='}';
	$lineage = graphviz_filter_process($graph);
	$html.=$lineage;
	$html.='</div><br><div>';
	$count = 0;
	foreach ($processName as $desc)
	{
		$link = $base_url.'/fedora/repository/' . $this->pid .'/PROCESS';
		if( module_exists('fancybox'))
	  	{
	  		$html.='<a class="fb_dynamic" style="font-size:10;font-family:Helvetica-Outline;font-style:italic;" href="'.$link.'"> '.$desc.'</a><br>';
	  	}
	  	else {
			$html.='<a style="font-size:10;font-family:Helvetica-Outline;font-style:italic;" href="'.$link.'" target="_blank"> '.$desc.'</a><br>';
	  		
	  	}
	}
	$html.='</div>';
	//$lineage = $graph;
	if( $isnull ==true )
	return null;
	else
    return $html;
  }
  
 function getLineage( $pid )
  {
 	 /*
 	    The query that gives the lineage information had been encoded. 
 	    An example of the real query for pid=es:MAST-DC.Biomass.Biomass_Modified to get decestral lineage is
 	    
 	    select $subject 'info:fedora/fedora-system:def/relations-external#isDerivedFrom' $object
		from <#ri> where walk(
		$subject <info:fedora/fedora-system:def/relations-external#isDerivedFrom> 
		<info:fedora/es:MAST-DC.Biomass.Biomass_Modified>
		and $subject <info:fedora/fedora-system:def/relations-external#isDerivedFrom> $object)
		
		 An example of the real query for pid=es:MAST-DC.Biomass.Biomass_Modified to get ancestral lineage is
 	    
 	    select $subject 'info:fedora/fedora-system:def/relations-external#isDerivedFrom' $object
		from <#ri> where walk(
		<info:fedora/es:MAST-DC.Biomass.Biomass_Modified>
		<info:fedora/fedora-system:def/relations-external#isDerivedFrom> $object
		and $subject <info:fedora/fedora-system:def/relations-external#isDerivedFrom> $object)
		
		
 	  */
  	 module_load_include('inc', 'fedora_repository', 'api/fedora_utils');  
     $encodedPID = str_replace(":", "%3A", $pid);
  	 $query_string = "select%20%24subject%20'DerivedFrom'%20%24object%0Afrom%20%3C%23ri%3E%0Awhere%20walk(%0A%20%20%20%20%20%20%20%20%24subject%20%3Cinfo%3Afedora%2Ffedora-system%3Adef%2Frelations-external%23isDerivedFrom%3E%20%3Cinfo%3Afedora%2F";
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
	     $lineage = '<table border="2" cellpadding="5px" style="margin-bottom:15;"><th>Children</th><th>Relation</th><th>Parent</th>';
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
		     		$entity = str_replace("DerivedFrom", "derived from", $entity);
		   			$label = preg_split("/:/", $entity);
		   			if ($label[1]!=null)
		   			$lineage.= $label[1];
		   			else
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
     
     $query_string = "select%20%20%24subject'DerivedFrom'%20%24object%0Afrom%20%3C%23ri%3E%0Awhere%20walk(%0A%20%20%20%20%20%20%20%20%3Cinfo%3Afedora%2F";
  	 $query_string .= $encodedPID;
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
		     		$entity = str_replace("DerivedFrom", "derived from", $entity);
		   			$label = preg_split("/:/", $entity);
		   			if ($label[1]!=null)
		   			$lineage.= $label[1];
		   			else
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
     return $lineage;
    
  }

  function getVersionTrails($pid) {
    $item = new Fedora_Item($pid);
    $foxml = $item->export_as_foxml();
    $audit_section = explode("<audit:auditTrail xmlns:audit=\"info:fedora/fedora-system:def/audit#\">", $foxml);
    $audit = explode("</audit:auditTrail>", trim($audit_section[1]));
    $sections = explode("</audit:record>", trim($audit[0]));
    $trails = '<table border="2" cellpadding="5px" float:left;">
    <th>Count</th><th>Timestamp</th><th>Component</th><th>Action</th><th>Justification</th>';
    $count = count($sections);
    $run = 1;


    foreach ($sections as $section) {
      $tags = explode("\n", trim($section));
      $timestamp = substr_replace("<audit:date>", "", $timestamp);
      $timestamp = substr_replace("</audit:date>", "", $timestamp);
      if (strcmp($section, $sections[$count]) != 0) {
        $trails .= "<tr><td>";
        $timestamp = $tags[5];
        $trails .= $run . "</td><td>";
        $trails .= $timestamp . "</td><td>";
        $componentId = $tags[3];
        $trails .= $componentId . "</td><td>";
        $action = $tags[2];
        $trails .= $action . "</td><td>";
        $Justification = $tags[6];
        $trails .= $Justification . "</td></tr>";
        $run+=1;
      }
    }
    return $trails;
  }
  /**
   *
   * @param <type> $xmlString
   * @return string
   */
  function getFdgcAsHtml($xmlString) {
/*    $fdgcXml = new SimpleXMLElement($xmlString);
    $title = $fdgcXml->xpath('//title');
    $links = $fdgcXml->xpath('//onlink');
    $resdesc = $fdgcXml->xpath('//resdesc');
    $contactPerson = $fdgcXml->xpath('//cntperp/cntper');
    $html = '';

    //$html .= '<table align="left"><tr><td valign="top" align="right" class="fedora_mods_field" width="15%">';
	$html .= '<table><tr><td align="right" valign="top" class="fedora_mods_field" width="15%">';
    $html .='<strong>Title</strong></td><td>';
    foreach ($title as $t) {
      $html .= $t . '<br />';
    }
    $html .= '</td></tr><tr><td valign="top" align="right"><strong>Links</strong></td><td>';
    foreach ($links as $t) {
      $html .= '<a href="' . $t . '" target="_blank"/>' . $t . '</a><br />';
    }
    $html .= $this->htmlSnippet("Contact", $contactPerson);
    $html .= $this->htmlSnippet("Citation", $resdesc);
    $html .='</td></tr></table></div></div><br />';
    return $html;
*/
    $fdgcXml = new SimpleXMLElement($xmlString);
    $title = $fdgcXml->xpath('//title');
    $links = $fdgcXml->xpath('//onlink');
    $resdesc = $fdgcXml->xpath('//resdesc');
    $contactPerson = $fdgcXml->xpath('//cntperp/cntper');
    $html = '';
   // $link;

    //$html .= '<table align="left"><tr><td valign="top" align="right" class="fedora_mods_field" width="15%">';
	$html .= '<table><tr><td align="right" valign="top" class="fedora_mods_field" width="15%">';
    $html .='<strong>Title</strong></td><td>';
    foreach ($title as $t) {
      $html .= $t . '<br />';
    }
    $html .= '</td></tr><tr><td valign="top" align="right"><strong>Links</strong></td><td>';
   foreach ($links as $t) {
      $html .= '<a class="links" href="' . $t . '" target="_blank" onClick="sendRequest('.$t.');">' . $t . '</a><br />';
      }
    $html .= $this->htmlSnippet("Contact", $contactPerson);
    $html .= $this->htmlSnippet("Citation", $resdesc);
    $html .='</td></tr></table>';
   // echo $html;
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
    if($this->datastreams_list==FALSE) { //used the cached copy if we have it
    	$this->datastreams_list = $this->item->get_datastreams_list_as_SimpleXML();
    }
    if ($this->datastreams_list != FALSE) {
      $html .='<table><tr><td>File</td><td>Checksum</td><td>Checksum Type</td></tr>';
      foreach ($this->datastreams_list->datastreamDef as $ds) {
        $ds_info = $this->item->get_datastream_info($ds->ID);
        if ($mimeTypeToRetrieve == $ds->MIMEType || $mimeTypeToRetrieve == '*') {
          // $list_of_streams[] = $ds;
          $include = TRUE;
          //drupal_set_message('<pre>'. print_r($exclude, TRUE) .'</pre>');
          if (isset($exclude)) {
            foreach ($exclude as $value) {
              if (!empty($value[0]) && stristr($ds->ID, $value[0])) {

              	
              	$include = FALSE;
              }
            }
          }
          if ($include) {
            $label = $ds->label;
            if (empty($label)) {
              $label = $ds->ID;
            }
            $html .= '<tr><td>';
            if ($ds_info->datastream->controlGroup == 'E' || $ds_info->datastream->controlGroup == 'R') {//link to external viewer
              //$txtValue = $this->item->get_datastream_dissemination($ds->ID);
              //$html .= '<a href = "' . $txtValue . '" target="_blank">' . $ds->label . '</a></td><td> ' . $ds_info->datastream->checksum . '</td><td> ' . $ds_info->datastream->checksumType . '</td></tr>';
              //don't do anything here as we don't want show external links here

            }
            else {
              $html .= '<a href = "' . $base_url . '/fedora/repository/' . $this->pid . '/' . $ds->ID . '/' . drupal_urlencode($label) . '">' . $ds->label . '</a></td><td> ' . $ds_info->datastream->checksum . '</td><td> ' . $ds_info->datastream->checksumType . '</td></tr>';
            }
          }
        }
      }
    }
    //return $list_of_streams;
    return $html . '</table>';
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
      $list_of_streams[] = array($dsid, (string) $mimeType, $dsInfo->datastream->controlGroup);
    }
    return $list_of_streams;
  }
  
  function createViewerContent($viewerStreams) {
    //var_dump($viewerStreams);exit();
    if (empty($viewerStreams)) {
      return;
    }
    global $base_url;
    $content = '';
    
    foreach ($viewerStreams as $viewer) {
      if (!empty($viewer)) {
       $txtValue = $this->item->get_datastream_dissemination($viewer[0]);
        $viewerUrl = '<div class="text"><iframe src="' . $txtValue . '" &embedded="TRUE" style="width:800px; height:800px;" frameborder="0"></iframe></div>';
        $fieldSet = array(
          '#title' => $viewer[0],
          '#attributes' => array(),
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
          '#value' => $viewerUrl,
        );
        $content .= theme('fieldset', $fieldSet);
      }
    }
//      	$url= $base_url.'/fedora/repository/' . $this->pid .'/'. $viewer[0];
//      	if(module_exists("fancybox"))
//        	$val = '<div class="text"><a href='.$url.' class="fb_dynamic"  target="_blank">'.$viewer[0].'</a></div>';
//        else 
//        	$val= '<div class="text"><a href='.$url.'  target="_blank">'.$viewer[0].'</a></div>';
//        $content.='<li>'.$val.'</li>';
//      }
//    }
//    if(!empty($content))
//    {
//    	$content = '<ul>'.$content.'</ul>';
//    }   
    return $content;
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
   * @param <type> $relationship
   * @param <type> $object
   * @param <type> $dsid
   * @param <type> $namespaceURI
   */
  static function add_relationship_internal($relationship, $object,$dsid, $namespaceURI) {
    module_load_include('inc', 'fedora_repository', 'api/fedora_item');
    $item = new Fedora_Item($object);
    $ds_list = $item->get_datastreams_list_as_array();
    if (empty($ds_list['RELS-INT'])) {
      $rels_int = '<rdf:RDF xmlns:fedora="info:fedora/fedora-system:def/relations-internal#" xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#" xmlns:rdfs="http://www.w3.org/2000/01/rdf-schema#"></rdf:RDF>';
      $item->add_datastream_from_string($rels_int, 'RELS-INT', 'Fedora internal relationship metadata', 'text/xml', 'X');
    }

    $rels = $item->get_datastream_dissemination('RELS-INT');
    
    if (substr($object, 0, 12) != 'info:fedora/') {
      $object = "info:fedora/$object";
    }

    $relsxml = new DomDocument();

    $relsxml->loadXML($rels);
    $rdf_element = $relsxml->getElementsByTagNameNS('http://www.w3.org/1999/02/22-rdf-syntax-ns#', 'RDF');

    /*this is what the xml looks like
     * <rdf:Description rdf:about="info:fedora/es:ISLSCP_II.TDS_C4_VEGETATION_PERCENTAGE/zero__historic_landcover_readme.txt">
    <myns:isMetaDataFor rdf:resource="info:fedora/es:ISLSCP_II.TDS_C4_VEGETATION_PERCENTAGE"></myns:isMetaDataFor>
  </rdf:Description>
     */

    // Create the new relationship node.
    $newrel = $relsxml->createElement('rdf:Description');
    $newrel->setAttribute('rdf:about', $object.'/'.$dsid);
    $newstmt = $relsxml->createElementNS($namespaceURI,'esdora:'.$relationship);
    $newstmt->setAttribute('rdf:resource', $object);
    $newrel->appendChild($newstmt);
    $rdf_element->item(0)->appendChild($newrel);
    $test = $relsxml->saveXML();
    $item->modify_datastream_by_value( $relsxml->saveXML(), 'RELS-INT', "internal Relationship Metadata", 'text/xml');
    
  }

  /**
   *
   * @param <type> $pid
   * @param <type> $dsid
   * @return <type>
   */
  static function remove_relationship_internal($pid,$dsid){
    module_load_include('inc', 'fedora_repository', 'api/fedora_item');
    $item = new Fedora_Item($pid);
    $ds_list = $item->get_datastreams_list_as_array();
    if (empty($ds_list['RELS-INT'])) {
     return TRUE; //nothing to remove as no RELS-INT but we don't want to flag an error so return success
    }

    $rels = $item->get_datastream_dissemination('RELS-INT');

    if (substr($object, 0, 12) != 'info:fedora/') {
      $pid = "info:fedora/$pid";
    }
    $attr = $pid.'/'.$dsid;//this is the value of the rdf:about attribute we are looking for
    $relsxml = simplexml_load_string($rels);
    $relsxml->registerXPathNamespace('rdf', "http://www.w3.org/1999/02/22-rdf-syntax-ns#");
    $nodes = $relsxml->xpath('//rdf:Description[@rdf:about="'.$attr.'"]');
    $nodes_to_remove = array();
    foreach($nodes as $node){
      //if there are issues when we have a one datastream in more then one rels-int statement we may have to look at the comment below
      //$nodes_to_remove[] = $node;//put the nodes in an array so we can remove them see this post http://stackoverflow.com/questions/262351/remove-a-child-with-a-specific-attribute-in-simplexml-for-php
      $dom=dom_import_simplexml($node);
      $dom->parentNode->removeChild($dom);
    }
    return $item->modify_datastream_by_value( $relsxml->saveXML(), 'RELS-INT', "internal Relationship Metadata", 'text/xml');

  }


}
