<?php

/**
 * @file
 *
 *  Utility/Convenience functions for this module.
 */

/**
 * Gets the numbers of child objects the given collection has.
 *
 * This function is referenced in the data/xsl/sparcql_to_html.xsl to be used with this transform it must be registered though registerPHPFunctions().
 *
 * @see ObjectHelper.inc->parseContent($content, $pid, $dsid, $collection, $page_number = NULL)
 * @see XSLTProcessor
 *
 * @param string $collection_pid
 *   The PID of the collection object.
 */
function esdora_get_collection_child_count($collection_pid) {
  $query = "select count (select \$object from <#ri> where \$object <fedora-rels-ext:isMemberOfCollection> <info:fedora/$collection_pid>) from <#ri>
            where \$s \$p \$o and \$object <fedora-model:state> <info:fedora/fedora-system:def/model#Active>";
  $query = htmlentities(urlencode($query));
  $url = variable_get('fedora_repository_url', 'http://localhost:8080/fedora/risearch') . "?lang=itql&format=Simple&query={$query}";
  $content = do_curl($url);
  if (strlen($content) > 0) {
    $tokens = explode('"', trim($content));
    $value = $tokens[1];
    if ($value == 0) {
      return "Data Object"; // @todo: Fix this is wrong when create a new empty collection
    } else {
      return 'Collection Object (total items: ' . $value . ')';
    }
  }
  return $content;
}

/**
 * Get the datastreams info.
 *
 * @param Fedora_Item $item
 *   The object to fetch the datastream info.
 *
 * @return array
 *   An array describing each datastream.
 * @code
 *   array(
 *    'RELS-EXT' => array(
 *      'dsid' => 'RELS-EXT',
 *      'label' => 'RELS-EXT',
 *      'mime' => 'text/xml',
 *      'viewer_for' => NULL,
 *      'metadata_for' => NULL,
 *    )
 *   );
 */
function esdora_get_datastreams_info(Fedora_Item $item) {
  $info = esdora_get_datastreams_default_info($item);
  return array_merge_recursive($info, esdora_get_datastreams_rels_int_info($item));
}

/**
 * Get the datastreams info common to every datastream.
 *
 * @param Fedora_Item $item
 *   The object to fetch the datastream info.
 *
 * @return array
 *   An array describing each datastream.
 * @code
 *   array(
 *    'RELS-EXT' => array(
 *      'dsid' => 'RELS-EXT',
 *      'label' => 'RELS-EXT',
 *      'mime' => 'text/xml',
 *    )
 *   );
 */
function esdora_get_datastreams_default_info(Fedora_Item $item) {
  $info = array();
  $datastreams_list = $item->get_datastreams_list_as_SimpleXML();
  $datastreams = isset($datastreams_list->datastreamDef) ? $datastreams_list->datastreamDef : array();
  foreach ($datastreams as $datastream) {
    $info[$datastream->ID] = array(
      'dsid' => (string) $datastream->ID,
      'label' => (string) $datastream->label,
      'mime' => (string) $datastream->MIMEType,
    );
  }
  return $info;
}

/**
 * Get the datastreams info stored within the objects RELS-INT.
 *
 * @param Fedora_Item $item
 *   The object to fetch the datastream info.
 *
 * @return array
 *   An array describing each datastream.
 * @code
 *   array(
 *    'FGDC' => array(
 *      'metadata_for' => 'PID',
 *    )
 *   );
 */
function esdora_get_datastreams_rels_int_info(Fedora_Item $item) {
  $info = array();
  if (isset($item->datastreams['RELS-INT'])) {
    $xpath = esdora_get_rels_int_xpath($item);
    $metadata_for = esdora_get_rels_triples('ns:isMetaDataFor', $xpath);
    $viewer_for = esdora_get_rels_triples('ns:isViewerFor', $xpath);
    $info = array_map(function($o) { return array('metadata_for' => array_pop($o)); }, $metadata_for); // Assumes one triple per datastream
    $info = array_merge_recursive($info, array_map(function($o) { return array('viewer_for' => array_pop($o)); }, $viewer_for));  // Assumes one triple per datastream
  }
  return $info;
}

/**
 * Gets DOMXPath object for the given fedora objects RELS-INT datastream.
 *
 * @param Fedora_Item $item
 *   The fedora object to fetch the RELS-INT datastream from.
 *
 * @return DOMXPath
 *   An xpath object configured with the proper namespaces for the RELS-INT datastream of the given fedora object.
 */
function esdora_get_rels_int_xpath(Fedora_Item $item) {
  $rels_int = $item->get_datastream_dissemination('RELS-INT');
  $doc = new DOMDocument();
  $doc->loadXML($rels_int);
  $xpath = new DOMXPath($doc);
  $xpath->registerNamespace('ns', 'http://esdora.ornl.gov/ontologies/relationships#');
  return $xpath;
}

/**
 * Gets DOMXPath object for the given fedora objects AUDIT datastream.
 *
 * @param Fedora_Item $item
 *   The fedora object to fetch the RELS-INT datastream from.
 *
 * @return DOMXPath
 *   An xpath object configured with the proper namespaces for the AUDIT datastream of the given fedora object.
 */
function esdora_get_audit_xpath(Fedora_Item $item) {
  $foxml = $item->export_as_foxml(); // Can only get the AUDIT trail via FOXML get datastream dissemination doesn't work for the AUDIT trail
  $doc = new DOMDocument();
  $doc->loadXML($foxml);
  $xpath = new DOMXPath($doc);
  $xpath->registerNamespace('audit', 'info:fedora/fedora-system:def/audit#');
  return $xpath;
}

/**
 * Gets the object/value from the triples defined by the given predicate.
 *
 * @param string $predicate
 *   The namespaced predicate to look for ex. ns:isMemberOfCollection
 * @param DOMXPath $xpath
 *   An DOMXPath for either rels datastream.
 *
 * @return array
 *   An array containing the object/value from the triple defined by the given predicate.
 */
function esdora_get_rels_triples($predicate, DOMXPath $xpath) {
  $ret = array();
  $results = $xpath->query("//$predicate/..");
  for ($i = 0; $i < $results->length; $i++) {
    $node = $results->item($i);
    $dsid = $xpath->evaluate('string(@rdf:about)', $node);
    $dsid = substr($dsid, strrpos($dsid, '/') + 1);
    $value = $xpath->evaluate("string($predicate/@rdf:resource)", $node);
    $value = substr($value, strrpos($value, '/') + 1);
    $ret[$dsid][] = $value;
  }
  return $ret;
}

/**
 * Gets the list of all decendant derivatives of the given object.
 *
 * @param Fedora_Item $item
 *   The fedora object used to query for decendant derivatives.
 *
 * @return array
 *   An array of arrays containg values each object that derivatives from the given object and any object that derives from that ad infinitum.
 */
function esdora_get_derivative_decendants(Fedora_Item $item) {
  $query = 'select $subject $object from <#ri> where walk(
            $subject <info:fedora/fedora-system:def/relations-external#isDerivedFrom> <info:fedora/' . $item->pid . '> and
            $subject <info:fedora/fedora-system:def/relations-external#isDerivedFrom> $object)';
  return esdora_itql_query($query);
}


/**
 * Gets the list of all derivative ancestor objects that the given object is derivatived from.
 *
 * @param Fedora_Item $item
 *   The fedora object used to query for derivative ancestors.
 *
 * @return array
 *   An array of arrays containg values each object that the given object is derivatived from ad infinitum.
 */
function esdora_get_derivative_ancestors(Fedora_Item $item) {
  $query = 'select  $subject $object from <#ri> where walk(
            <info:fedora/' . $item->pid . '> <info:fedora/fedora-system:def/relations-external#isDerivedFrom> $object and
            $subject <info:fedora/fedora-system:def/relations-external#isDerivedFrom> $object)';
  return esdora_itql_query($query);
}

/**
 * Simple ITQL risearch query which returns an array of values in the same order as the select expression of the query.
 * Also trims "info:fedora/" from the values, as it is rarely useful.
 *
 * @param string $query
 *   The itql query to execute in risearch.
 *
 * @return array
 *   An array of arrays containg values for each identifier in the select expression of the query.
 */
function esdora_itql_query($query) {
  module_load_include('inc', 'fedora_repository', 'api/fedora_utils');
  $risearch_url = variable_get('fedora_repository_url', 'http://localhost:8080/fedora/risearch');
  $risearch_url .= '?type=tuples&flush=true&format=csv&lang=itql&stream=on&query=' . urlencode($query);
  $content = do_curl($risearch_url);
  $rows = explode("\n", $content);
  array_shift($rows); // Remove subject/object line
  $rows = array_map(function($o) { return str_replace('info:fedora/', '', $o); }, $rows); // Strip out "info:fedora/"
  $rows = array_filter($rows, function($o) { return !empty($o); }); // Remove empty rows (trailing new lines).
  return array_map(function($o) { return explode(',', $o); }, $rows); // Convert into an array of values from the select expression.
}

/**
 * Configures the given pager using values from the request.
 *
 * @param int $pager_id
 *   The id of the pager to configure.
 * @param int $total
 *   The total number of things to page though.
 * @param int $limit
 *   The number of things to display on a single page.
 *
 * @return int
 *   The current page.
 */
function esdora_configure_pager($pager_id, $total, $limit = 10) {
  global $pager_page_array, $pager_total, $pager_total_items;
  $page = isset($_GET['page']) ? $_GET['page'] : '';
  $page = isset($_POST['page']) ? $_POST['page'] : $page; // Tabs send POST request they take precedent. @todo should all be GETS
  $pager_page_array = explode(',', $page); // Convert comma-separated $page to an array, used by other functions.
  $pager_total_items[$pager_id] = count($datastreams);
  $pager_total[$pager_id] = ceil($pager_total_items[$pager_id] / $limit);
  $pager_page_array[$pager_id] = max(0, min((int)$pager_page_array[$pager_id], ((int)$pager_total[$pager_id]) - 1));
  return $page;
}