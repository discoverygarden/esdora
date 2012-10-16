<?php

module_load_include('inc', 'esdora_islandora', 'plugins/esdora');

class Esdora_Atomic extends Esdora {
	
	
	function __construct($pid='') {
		parent::__construct($pid);
		$path = drupal_get_path('module', 'esdora_islandora');
		$path_atomic=$path . '/ajaxtabs_atomic.js';
		drupal_add_js($path_atomic);
		$path_compound=$path . '/ajaxtabs.css';
		drupal_add_css($path_compound);  
	}

	/**
	* TODO: the exclusion list is obsolete for atomic model, may need to be deleted...
	* @global <type> $base_url
	* @param <type> $mimeTypeToRetrieve
	* @param <type> $exclude
	* @return string
	*/
	function getListOfStreams($mimeTypeToRetrieve, $exclude=null) {
		global $base_url;
		$html = '';
		$collectionClass = new CollectionClass();
		$memberObjectQuery ='select $object $title from <#ri>
                             where ($object <dc:title> $title
                             and $object <fedora-rels-ext:isMemberOf> <info:fedora/' . $this->pid . '>
	                         and $object <fedora-model:state> <info:fedora/fedora-system:def/model#Active>)
                             order by $title'; 
		//note: compared to the default query (see CollectionClass->getRelatedItems() 
		if($this->data_component_list == FALSE) {	
			$result = $collectionClass->getRelatedItems($this->pid, $memberObjectQuery, NULL, NULL);
			$result_xml= simplexml_load_string ($result);
			$member_list = array();
			foreach ($result_xml->results->result as $result) {   
				$pid = strval(($result->object['uri']));
				$pid = substr($pid, strpos($pid, "/") + 1, strlen($pid));
				$fedora_item = new Fedora_Item($pid);
				$member_list[]=array('subpid'=>$pid, 'content'=>$fedora_item->get_datastream_info('content'));
			}
			$this->data_component_list = $member_list;
		}
		
		if($this->data_component_list !=FALSE) {
			$html .='<table><tr><td>File</td><td>Checksum</td><td>Checksum Type</td></tr>';
			foreach ($this->data_component_list as $item) {
				$subpid=$item['subpid'];
				//dvm($subpid);
				$ds=$item['content'];
				//dvm($ds);
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
						$label = $ds->datastream->label;
						if (empty($label)) {
							$label = $subpid;
						}
						$html .= '<tr><td>';
						if ($ds->datastream->controlGroup == 'E' || $ds->datastream->controlGroup == 'R') {
						}
						else {
							$html .= '<a href = "' . $base_url . '/fedora/repository/' .$subpid . '/' . $ds->datastream->ID . '/' . drupal_urlencode($label) . '">' . $label. '</a></td><td> ' . $ds->datastream->checksum . '</td><td> ' . $ds->datastream->checksumType . '</td></tr>';
						}
					}
				}
			}
			return $html . '</table>';
		}

	}
}