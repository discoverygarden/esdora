<?php

/**
 * @file
 *
 *  Deals with the menu callbacks for displaying object tabs.
 */
module_load_include('inc', 'esdora_islandora', 'esdora.utils');

define('PAGER_ESDORA_ITEMS_ID', 0);
define('PAGER_ESDORA_ITEMS_LIMIT', 10);

/**
  * Implements template_preprocess_hook().
 */
function template_preprocess_esdora_object_tabs(array &$variables) {
  $item = &$variables['item'];
  $variables['pid'] = $item->pid;
  $variables['active_tab'] = esdora_get_tab($item);
  $variables['can_manage'] = user_access('edit fedora meta data');
  $variables['has_viewer'] = count(esdora_get_viewers($item));
}

/**
 * Renders the requested tab, or the error tab if the request is invalid.
 *
 * @param Fedora_Item $item
 *   The fedora object used to display this tab.
 * @param string $tab
 *   The tab to display.
 *
 * @return string
 *   A html repersentation of the requested tab.
 */
function esdora_get_tab(Fedora_Item $item, $tab = NULL) {
  module_load_include('inc', 'fedora_repository', 'api/fedora_item');
  $valid_tabs = array('items', 'metadata', 'description', 'history', 'viewers', 'manage');
  $tab = isset($tab) ? $tab : 'items';
  $valid_tab = array_search($tab, $valid_tabs) !== FALSE;
  $tab = (array_search($tab, $valid_tabs) !== FALSE) ? $tab : 'error';
  return call_user_func("esdora_get_{$tab}_tab", $item, esdora_get_datastreams_info($item));
}

/**
 * Render the items tab for the given object.
 *
 * @param Fedora_Item $item
 *   The fedora object used to display this tab.
 * @param array $datastreams
 *   An array of datastreams as defined by esdora_get_datastreams_info used to build the display for the tab.
 *
 * @return string
 *   The html repersentation of the requested tab.
 */
function esdora_get_items_tab(Fedora_Item $item, array $datastreams) {
  $datastreams = esdora_get_items_tab_datastreams($item, $datastreams);
  $table = esdora_get_items_tab_datastream_table($item, $datastreams);
  $pager = theme_pager(array(), PAGER_ESDORA_ITEMS_LIMIT,  PAGER_ESDORA_ITEMS_ID);
  return '<p id="item-display"/><hr/>' . $table . $pager;
}

/**
 * Gets the datastreams to display in the items tab.
 *
 * @param Fedora_Item $item
 *   The fedora object used to fetch the datastreams.
 * @param array $datastreams
 *   An array of datastreams as defined by esdora_get_datastreams_info used to build the display for the tab.
 *
 * @return array
 *   A filtered array of datastreams to be displayed in the items tab, with some additional information.
 */
function esdora_get_items_tab_datastreams(Fedora_Item $item, array $datastreams) {
  $exclude = array('DC', 'RELS-INT', 'RELS-EXT', 'islandora_workflow');
  $datastreams = array_diff_key($datastreams, array_fill_keys($exclude, NULL));
  $datastreams = array_filter($datastreams, function($o) { return empty($o['metadata_for']) && empty($o['viewer_for']); });
  $page = esdora_configure_pager(PAGER_ESDORA_ITEMS_ID, count($datastreams), PAGER_ESDORA_ITEMS_LIMIT);
  return array_slice($datastreams, $page *  PAGER_ESDORA_ITEMS_LIMIT,  PAGER_ESDORA_ITEMS_LIMIT); // Split out the number to be rendered.
}

/**
 * Gets the datastream table to be displayed in the items tab.
 *
 * @param Fedora_Item $item
 *   The fedora object used to display this tab.
 * @param array $datastreams
 *   An array of datastreams as defined by esdora_get_datastreams_info to be rendered in the table.
 *
 * @return string
 *   An html table repersentation of the given datastreams for display in the items tab.
 */
function esdora_get_items_tab_datastream_table(Fedora_Item $item, array $datastreams) {
  $rows = array();
  foreach ($datastreams as $dsid => $info) {
    $label = empty($info['label']) ? $dsid : $info['label'];
    $additional_info = $item->get_datastream_info($dsid); // This isn't part of esdora_get_datastream_info for performance some objects have 100's of datastreams.
    $control_group = $additional_info->datastream->controlGroup;
    $url = ($control_group == 'E' || $control_group == 'R') ? $additional_info->datastream->location : "fedora/repository/{$item->pid}/{$dsid}";
    $rows[] = array(l($label, $url), $additional_info->datastream->checksum, $additional_info->datastream->checksumType);
  }
  $header = array(t('Data Item'), t('Checksum'), t('Checksum Type'));
  return theme_table($header, $rows);
}

/**
 * Render the metadata tab for the given object.
 *
 * @param Fedora_Item $item
 *   The fedora object used to display this tab.
 * @param array $datastreams
 *   An array of datastreams as defined by esdora_get_datastreams_info used to build the display for the tab.
 *
 * @return string
 *   The html repersentation of the requested tab.
 */
function esdora_get_metadata_tab(Fedora_Item $item, array $datastreams) {
  $datastreams = array_filter($datastreams, function($o) use($item) { return isset($o['metadata_for']) && ($o['metadata_for'] == $item->pid); });
  $table = esdora_get_metadata_tab_metadata_table($item, $datastreams);
  $edit_links = esdora_get_metadata_tab_edit_links($item, $datastreams);
  return '<p id="item-display"/><hr/>' . $table . $edit_links;
}

/**
 * Gets the metadata table describing this object.
 *
 * @todo Fix, as it only supports one metadata table at the moment, and its hacky and hard coded.
 *
 * @param Fedora_Item $item
 *   The fedora object used to display this tab.
 * @param array $datastreams
 *   An array of datastreams as defined by esdora_get_datastreams_info used to build the display for the tab.
 *
 * @return string
 *   The html repersentation of the metadata table.
 */
function esdora_get_metadata_tab_metadata_table(Fedora_Item $item, array $datastreams) {
  if (count($datastreams) > 0) {
    $valid_dsids = array('fgdc', 'fgdc.xml');
    $datastream = array_pop($datastreams);
    if (array_search(strtolower($datastream['dsid']), $valid_dsids) !== FALSE) { // @todo implment a better system for determining what should render.
      $fgdc = $item->get_datastream_dissemination($datastream['dsid']);
      $fdgc = new SimpleXMLElement($fgdc);
      $titles = $fdgc->xpath('//title');
      $links = $fdgc->xpath('//onlink');
      $citation = $fdgc->xpath('//resdesc');
      $contact_persons = $fdgc->xpath('//cntperp/cntper');
      $rows[] = array(t('Title'), implode('<br/>', $titles));
      $rows[] = array(t('Links'), implode('<br/>', array_map(function($o) { return l($o, $o); }, $links)));
      $rows[] = array(t('Contact'), implode('<br/>', $contact_persons));
      $rows[] = array(t('Citation'), implode('<br/>', $citation));
      return theme_table(array(), $rows);
    }
  }
  else {
    // @todo Render DC fields by Default.
    return t('DC Table');
  }
}

/**
 * Gets edit metadata links for all the datastreams that have a isMetadataFor relationships with the object.
 *
 * @todo Filter this list on what forms exist so we don't render bad links.
 *
 * @param Fedora_Item $item
 *   The fedora object used to display this tab.
 * @param array $datastreams
 *   An array of datastreams as defined by esdora_get_datastreams_info used to build the display for the tab.
 *
 * @return string
 *   The html repersentation of all the edit metadata links.
 */
function esdora_get_metadata_tab_edit_links(Fedora_Item $item, array $datastreams) {
  module_load_include('inc', 'fedora_repository', 'ObjectHelper');
  if (user_access(ObjectHelper::$EDIT_FEDORA_METADATA)) {
    $edit_url = "esdora/render/form/{$item->pid}";
    $edit_links = array(l('Edit DC',  "$edit_url/DC/DC"));
    $dsids = array_keys($datastreams);
    $edit_links = array_merge($edit_links, array_map(function($o) use($edit_url) { return l("Edit {$o['label']}", "$edit_url/{$o['dsid']}/{$o['label']}"); }, $datastreams));
    return implode('<br/>', $edit_links);
  }
  return '';
}

/**
 * Render the description tab for the given object.
 *
 * @note Seems to only render datastreams which have an isMetaDataFor relationship with the object, kinda duplicates the metadata tab.
 *       Used to have a lot of code for rendering different mime types although they would never get executed.
 *
 * @param Fedora_Item $item
 *   The fedora object used to display this tab.
 * @param array $datastreams
 *   An array of datastreams as defined by esdora_get_datastreams_info used to build the display for the tab.
 *
 * @return string
 *   The html repersentation of the requested tab.
 */
function esdora_get_description_tab(Fedora_Item $item, array $datastreams) {
  $datastreams = array_filter($datastreams, function($o) use($item) { return isset($o['metadata_for']) && ($o['metadata_for'] == $item->pid); });
  $table = esdora_get_description_tab_table($item, $datastreams);
  return '<p id="item-display"/><hr/>' . $table;
}

/**
 * Fetches the description tab's table.
 *
 * @param Fedora_Item $item
 *   The fedora object used to display this tab.
 * @param array $datastreams
 *   An array of datastreams as defined by esdora_get_datastreams_info used to build the display for the tab.
 *
 * @return string
 *   The html repersentation of the requested table.
 */
function esdora_get_description_tab_table(Fedora_Item $item, array $datastreams) {
  $rows = array();
  $url = "fedora/repository/{$item->pid}";
  foreach ($datastreams as $dsid => $datastream) {
    $label = t($datastream['label']);
    $view = l('<button>' . t('View') . '</button>',  "$url/$dsid", array('html' => TRUE, 'absolute' => TRUE, 'attributes' => array('class' => 'fb_dynamic')));
    $download = l('<button>' . t('Download') . '</button>',  "$url/$dsid", array('html' => TRUE, 'attributes' => array('target' => '_blank')));
    $rows[] = array($label, $view, $download);
  }
  $header = array(t('Document Item'), t('Inline View'), t('Download'));
  return theme_table($header, $rows);
}

/**
 * Render the history tab for the given object.
 *
 * @param Fedora_Item $item
 *   The fedora object used to display this tab.
 * @param array $datastreams
 *   An array of datastreams as defined by esdora_get_datastreams_info used to build the display for the tab.
 *
 * @return string
 *   The html repersentation of the requested tab.
 */
function esdora_get_history_tab(Fedora_Item $item, array $datastreams) {
  $lineage = esdora_get_history_tab_lineage_fieldset($item, $datastreams);
  $audit_history = esdora_get_history_tab_audit_history_fieldset($item);
  return '<p id="item-display"/><hr/>' . $lineage . $audit_history;
}

/**
 * Get the lineage fieldset containing a table describing the AUDIT trail.
 *
 * @todo Use labels instead of PIDS.
 *
 * @param Fedora_Item $item
 *   The fedora object used to display this tab.
 *
 * @return string
 *   An html repersentation of the Audit history fieldset.
 */
function esdora_get_history_tab_lineage_fieldset(Fedora_Item $item) {
  if (module_exists('graphviz_filter')) {
    $content = esdora_get_history_tab_derivation_graph($item);
  } else {
    $content = esdora_get_history_tab_ancestors_table($item);
    $content .= esdora_get_history_tab_decendants_table($item);
  }
  return theme('fieldset', array(
      '#title' => t("Data History"),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#value' => $content,
    )
  );
}

/**
 * Get the ancestors table.
 *
 * @param Fedora_Item $item
 *   The fedora object used to display this table.
 *
 * @return string
 *   An html repersentation of the decendants table.
 */
function esdora_get_history_tab_ancestors_table(Fedora_Item $item) {
  $rows = esdora_get_derivative_ancestors($item);
  $header = array(t('Dataset'), t('Relation'), t('Origin'));
  if (count($rows)) {
    $rows = array_map(function($o) { array_splice($o, 1, 0, 'derived from'); return $o; }, $rows);
    return theme_table($header, $rows, array(), t('Ancestors'));
  }
  return '';
}

/**
 * Get the decendants table.
 *
 * @param Fedora_Item $item
 *   The fedora object used to display this table.
 *
 * @return string
 *   An html repersentation of the decendants table.
 */
function esdora_get_history_tab_decendants_table(Fedora_Item $item) {
  $rows = esdora_get_derivative_decendants($item);
  $header = array(t('Dataset'), t('Relation'), t('Origin'));
  if (count($rows)) {
    $rows = array_map(function($o) { array_splice($o, 1, 0, 'derived from'); return $o; }, $rows);
    return theme_table($header, $rows, array(), t('Decendants'));
  }
  return '';
}

/**
 * Get the derivation graph.
 *
 * Removed the process stuff, didn't work anyways.
 *
 * @param Fedora_Item $item
 *   The fedora object used to display this table.
 *
 * @return string
 *   An html repersentation of the decendants table.
 */
function esdora_get_history_tab_derivation_graph(Fedora_Item $item) {
  $get_identifier = function($pid) {
    static $identifiers = array();
    if (isset($identifiers[$pid])) {
      return $identifiers[$pid];
    }
    return $identifiers[$pid] = count($identifiers);
  };
  $node_attributes = "shape=ellipse,style=filled,color=\"#D7D6DB\",fontname=\"Helvetica-Outline\",fontsize=10";
  $relations = array_merge(esdora_get_derivative_ancestors($item), esdora_get_derivative_ancestors($item));
  $graph = 'strict digraph G{rankdir=LR ';
  foreach ($relations as $relation) {
    list($subject, $object) = $relation;
    $subject_id = $get_identifier($subject);
    $object_id = $get_identifier($object);
    $graph .= "$object_id -> $subject_id ";
    $graph .= "$subject_id [label=\"$subject\",$node_attributes] ";
    $graph .= "$object_id [label=\"$object\",$node_attributes] ";
  }
  $graph .='}';
  return '<div style="width:inherit;">' . graphviz_filter_process($graph). '</div>';
}

/**
 * Get the audit history fieldset containing a table describing the AUDIT trail.
 *
 * @param Fedora_Item $item
 *   The fedora object used to display this tab.
 *
 * @return string
 *   An html repersentation of the Audit history fieldset.
 */
function esdora_get_history_tab_audit_history_fieldset(Fedora_Item $item) {
  $xpath = esdora_get_audit_xpath($item);
  $results = $xpath->query('//audit:record');
  $rows = array();
  for ($i = 0; $i < $results->length; $i++) {
    $node = $results->item($i);
    $timestamp = $xpath->evaluate('string(audit:date)', $node);
    $component = $xpath->evaluate('string(audit:componentID)', $node);
    $action = $xpath->evaluate('string(audit:action)', $node);
    $justification = $xpath->evaluate('string(audit:justification)', $node);
    $rows[] = array($i + 1, $timestamp, $component, $action, $justification);
  }
  $header = array(t('Count'), t('Timestamp'), t('Component'), t('Action'), t('Justification'));
  return theme('fieldset', array(
      '#type' => 'fieldset',
      '#title' => t("Audit History"),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#value' => theme_table($header, $rows),
    )
  );
}

/**
 * Render the history tab for the given object.
 *
 * @param Fedora_Item $item
 *   The fedora object used to display this tab.
 * @param array $datastreams
 *   An array of datastreams as defined by esdora_get_datastreams_info used to build the display for the tab.
 *
 * @return string
 *   The html repersentation of the requested tab.
 */
function esdora_get_viewers_tab(Fedora_Item $item, array $datastreams) {
  $datastreams = esdora_get_viewers($item);
  $content = '';
  foreach ($datastreams as $dsid) {
    $datastream = $item->get_datastream_dissemination($dsid);
    $content .= theme('fieldset', array(
                  '#title' => $dsid,
                  '#attributes' => array(),
                  '#collapsible' => TRUE,
                  '#collapsed' => TRUE,
                  '#value' => "<div class='text'><iframe src='$datastream' &embedded='TRUE' style='width:800px; height:800px;' frameborder='0'></iframe></div>",
                )
    );
  }
  return '<p id="item-display"/><hr/>' . $content;
}


/**
 * @todo Clean up rendering of the manage tab.
 */
function esdora_get_manage_tab(Fedora_Item $item) {
  return theme(THEME_ESDORA_OBJECT_MANAGE_TAB, $item->pid);
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
