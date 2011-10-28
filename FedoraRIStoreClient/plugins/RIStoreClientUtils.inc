<?php

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
class RIStoreClientUtils{
function __construct($pid = '') {
   if (!empty($pid)) {
      $this->pid = $pid;
    }
    else
    {
        $this->pid='es:top';
    }
  }

function get_child_pids_object($pid) {
    
        //      select $o $title from <#ri> 
        //where $o <fedora-model:hasModel> <info:fedora/esdora:esdora:defaultDataCModel> 
        //and $o <dc:title> $title 
        //and $o <info:fedora/fedora-system:def/relations-external#isMemberOfCollection>
        //        <info:fedora/es:top>
       
    
    module_load_include('inc', 'fedora_repository', 'api/fedora_utils');
    module_load_include('inc', 'fedora_repository', 'ObjectHelper');
    // Get pid, title and description for children of object $pid
    $query_string='select%20%24o%20%24title%20from%20%3C%23ri%3E%20%0Awhere%20%24o%20%3Cfedora-model%3AhasModel'.
        '%3E%20%3Cinfo%3Afedora%2Fesdora%3AdefaultDataCModel%3E%20%0Aand%20%24o%20%3Cdc%3Atitle%3E%20'.
            '%24title%20%0Aand%20%24o%20%3Cinfo%3Afedora%2Ffedora-system%3Adef%2Frelations-external%23'.
                'isMemberOfCollection%3E%3Cinfo%3Afedora%2F';
    $query_string.=htmlentities(urlencode($pid));
    $query_string.='%3E'; 
    $url = variable_get('fedora_repository_url', 'http://localhost:8080/fedora/risearch');
    $url .= "?type=tuples&flush=true&format=csv&limit=1000&lang=itql&stream=on&query=";
    //$url .= htmlentities(urlencode($query_string));
    $url.=$query_string;
    $content = do_Curl($url);

    $rows = explode("\n", $content);
    // Knock of the first heading row
    array_shift($rows);

    $child_pids = array();
    if (count($rows)) {
      // iterate through each row
      foreach ($rows as $row) {
        if ($row == "") {
          continue;
        }
        $fields = explode(',', $row);
        $child_pid = substr($fields[0], 12);
        $child_pids[$child_pid] = array('title' => $fields[1], 'description' =>$child_pid);
      }
    }

    return $child_pids;
  }
  function get_child_pids_collection($pid){    
      
     
        //      select $o $title from <#ri> 
        //where $o <fedora-model:hasModel> <info:fedora/esdora:defaultCollectionCModel> 
        //and $o <dc:title> $title 
        //and $o <info:fedora/fedora-system:def/relations-external#isMemberOfCollection>
        //        <info:fedora/es:top>
        
      
    module_load_include('inc', 'fedora_repository', 'api/fedora_utils');
    module_load_include('inc', 'fedora_repository', 'ObjectHelper');
    $query_string='select%20%24o%20%24title%20from%20%3C%23ri%3E%20%0Awhere%20%24o%20%3Cfedora-model%3AhasModel'.
        '%3E%20%3Cinfo%3Afedora%2Fesdora%3AdefaultCollectionCModel%3E%20%0Aand%20%24o%20%3Cdc%3Atitle%3E%20'.
            '%24title%20%0Aand%20%24o%20%3Cinfo%3Afedora%2Ffedora-system%3Adef%2Frelations-external%23'.
                'isMemberOfCollection%3E%3Cinfo%3Afedora%2F';
    $query_string.=htmlentities(urlencode($pid));   
    $query_string.='%3E';
    $url = variable_get('fedora_repository_url', 'http://localhost:8080/fedora/risearch');
    $url .= "?type=tuples&flush=true&format=csv&limit=1000&lang=itql&stream=on&query=";
    //$url .= htmlentities(urlencode($query_string));
    $url.=$query_string;
    $content = do_Curl($url);

    $rows = explode("\n", $content);
    // Knock of the first heading row
    array_shift($rows);
    $child_pids = array();
    if (count($rows)) {
      // iterate through each row
      foreach ($rows as $row) {
        if ($row == "") {
          continue;
        }
        $fields = explode(',', $row);
        $child_pid = substr($fields[0], 12);
        $child_pids[$child_pid] = array('title' => $fields[1], 'description' => $child_pid);
      }
     
    }

    return $child_pids;
  }
  
}
function get_data_objects()
{
    $querystring = 'select%20%24o%20%24title%20from%20%3C%23ri%3E%20%0A%20%20where%20%24o%20%3Cfedora-model%3AhasModel'.
        '%3E%20%3Cinfo%3Afedora%2Fesdora%3AdefaultDataCModel%3E%20%0A%20%20and%20%24o%20%3Cdc%3Atitle%3E%20%24title%20';
    module_load_include('inc', 'fedora_repository', 'api/fedora_utils');
    module_load_include('inc', 'fedora_repository', 'ObjectHelper');
    $url = variable_get('fedora_repository_url', 'http://localhost:8080/fedora/risearch');
    $url .= "?type=tuples&flush=true&format=csv&limit=1000&lang=itql&stream=on&query=";
    $url.=$query_string;
    $content = do_Curl($url);

    $rows = explode("\n", $content);
    // Knock of the first heading row
    array_shift($rows);
    $child_pids = array();
    if (count($rows)) {
      // iterate through each row
      foreach ($rows as $row) {
        if ($row == "") {
          continue;
        }
        $fields = explode(',', $row);
        $child_pid = substr($fields[0], 12);
        $child_pids[$child_pid] = array('title' => $fields[1], 'description' => $child_pid);
      }
     
    }

    return $child_pids;
}
?>
