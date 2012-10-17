<?php

/**
 * @file
 *
 *  Deals with the menu callbacks for displaying object tabs.
 */
module_load_include('inc', 'esdora_islandora', 'esdora.utils');

/**
 * Menu callback for esdora/tab. Renders the requested tab, or the error tab if the request is invalid.
 *
 * Called via AJAX
 *
 * @return string
 *   A JSON object containing the html repersentation of the requested tab.
 */
function esdora_get_tabs() {
  module_load_include('inc', 'fedora_repository', 'api/fedora_item');
  $valid_tabs = array('items', 'metadata', 'description', 'history', 'viewers');
  $pid = isset($_GET['pid']) ? $_GET['pid'] : 'esdora:dataset-A'; //variable_get('fedora_repository_pid', 'islandora:root');
  $format = isset($_GET['format']) ? $_GET['format'] : 'compound';
  $tab = isset($_GET['tab']) ? $_GET['tab'] : 'items';
  $valid_tab = array_search($tab, $valid_tabs) !== FALSE;
  $item = new Fedora_Item($pid);
  $tab = ($valid_tab && $item->exists()) ? $tab : 'error';
  call_user_func("esdora_get_{$tab}_tab", $item);
  return '';
}

/**
 * Render the items tab for the given object.
 *
 * @param Fedora_Item $item
 *   The fedora object used to display this tab.
 *
 * @return string
 *   The html repersentation of the requested tab.
 */
function esdora_get_items_tab(Fedora_Item $item) {
  $exclude = array('DC', 'RELS-INT', 'RELS-EXT', 'islandora_workflow');
  $datastream_info = esdora_get_datastreams_info($item);
  $datastream_info = array_diff_key($datastream_info, array_fill_keys($exclude, NULL));
  $datastream_info = array_filter($datastream_info, function($o) { return empty($o['metadata_for']) && empty($o['viewer_for']); });
  // @todo render as a table
  return array();
  /*
  $collectionClass = new CollectionClass();
  $itqlViewerQuery = 'select $datastream  $mimeType from <#ri> where $datastream <http://esdora.ornl.gov/ontologies/relationships#isViewerFor>
<info:fedora/' . $this->pid . '> and $datastream <info:fedora/fedora-system:def/view#mimeType> $mimeType';
  $itqlMetaDataQuery = 'select $datastream  $mimeType from <#ri> where $datastream <http://esdora.ornl.gov/ontologies/relationships#isMetaDataFor>
      <info:fedora/' . $this->pid . '> and $datastream <info:fedora/fedora-system:def/view#mimeType> $mimeType';
  $itqlViwerResults = $collectionClass->getRelatedItems($this->pid, $itqlViewerQuery, $limit = NULL, $offset = NULL);
  $itqlMetadataResults = $collectionClass->getRelatedItems($this->pid, $itqlMetaDataQuery, $limit = NULL, $offset = NULL);
  $ds_list_viewers = $this->getListOfStreamsFromItql($itqlViwerResults);
  $ds_list_metadata = $this->getListOfStreamsFromItql($itqlMetadataResults);
  $non_data_streams = array_merge($ds_list_viewers, $ds_list_metadata);
  $non_data_streams[] = array('DC', 'text/xml', 'X');
  $non_data_streams[] = array('RELS-EXT', 'text/xml', 'X');
  $non_data_streams[] = array('RELS-INT', 'text/xml', 'X');
  $non_data_streams[] = array('islandora_workflow', 'text/xml', 'X');
  $streams = $this->getListOfStreams('*', $non_data_streams);
  $separater = "<p id='item-display' /> <hr/>";
  return $separater .  $streams;*/
}

/**
 * Render the metadata tab for the given object.
 *
 * @param Fedora_Item $item
 *   The fedora object used to display this tab.
 *
 * @return string
 *   The html repersentation of the requested tab.
 */
function esdora_get_metadata_tab(Fedora_Item $item) {

}

/**
 * Render the description tab for the given object.
 *
 * @param Fedora_Item $item
 *   The fedora object used to display this tab.
 *
 * @return string
 *   The html repersentation of the requested tab.
 */
function esdora_get_description_tab(Fedora_Item $item) {

}

/**
 * Render the history tab for the given object.
 *
 * @param Fedora_Item $item
 *   The fedora object used to display this tab.
 *
 * @return string
 *   The html repersentation of the requested tab.
 */
function esdora_get_history_tab(Fedora_Item $item) {

}

/**
 * Render the history tab for the given object.
 *
 * @param Fedora_Item $item
 *   The fedora object used to display this tab.
 * @return string
 *   The html repersentation of the requested tab.
 */
function esdora_get_viewers_tab(Fedora_Item $item) {

}

/**
 * Render the error tab for the given object.
 *
 * @param Fedora_Item $item
 *   The fedora object used to display this tab.
 *
 * @return string
 *   The html repersentation of the requested tab.
 */
function esdora_get_error_tab(Fedora_Item $item) {
  return t('An error occurred.');
}




function getListOfStreams($mimeTypeToRetrieve, $exclude=null) {
  // Build list of datastreams to be displayed.
  $this->datastreams_list = ($this->datastreams_list) ? $this->datastreams_list : $this->item->get_datastreams_list_as_SimpleXML();
  $datastreams = isset($this->datastreams_list->datastreamDef) ? $this->datastreams_list->datastreamDef : array();
  $allow_all_mime_types = $mimeTypeToRetrieve == '*';
  $datastreams = $allow_all_mime_types ? $datastreams : array_filter($datastreams, function($ds) use($mime_typeToRetrieve) { return $mimeTypeToRetrieve == $ds->MIMEType; });
  $exclude = empty($exclude) ? NULL : array_map(function($ex) { return isset($ex[0]) ? $ex[0] : NULL; }, $exclude);
  $datastreams = empty($exclude) ? $datastreams : array_filter($datastreams, function($ds) use($exclude) { return array_search($ds->ID, $exclude) === FALSE; });
  // Build list of Rows
  $page = esdora_configure_pager(0, count($datastreams), $limit = 10);
  $datastreams = array_slice($datastreams, $page * $limit, $limit); // Split out the number to be rendered.
  $rows = array();
  foreach ($datastreams as $ds) {
    $ds_info = $this->item->get_datastream_info($ds->ID);
    $cg = $ds_info->datastream->controlGroup;
    $label = empty($ds->label) ? $ds->ID : $ds->label;
    $url = ($cg == 'E' || $cg == 'R') ? $ds_info->datastream->location : "fedora/repository/{$this->pid}/{$ds->ID}";
    $rows[] = array(l($label, $url), $ds_info->datastream->checksum, $ds_info->datastream->checksumType);
  }
  $header = array(t('Data Item'), t('Checksum'), t('Checksum Type'));
  $table = theme_table($header, $rows);
  $q = $_GET['q'];
  $_GET['q'] = 'fedora/repository/' . $this->pid; // Hack for dealing with ajax tabs.
  $pager = theme_pager();
  $_GET['q'] = $q;
  return $table . $pager;
}
