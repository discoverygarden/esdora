<?php

/**
 * @file
 *
 *  Deals with the display/navigation of the fedora repository.
 */
module_load_include('inc', 'esdora_islandora', 'esdora.utils');

/**
 * Implements theme_hook();
 */
function theme_esdora_repository_page($pid = NULL, $dsid = NULL) {
 module_load_include('inc', 'fedora_repository', 'api/fedora_item');
 $pid = empty($pid) ? variable_get('fedora_repository_pid', 'islandora:root') : $pid;
 $item = new Fedora_Item($pid);
 if (!esdora_fedora_and_risearch_available() || !esdora_object_and_datastream_exist($item, $dsid)) {
   drupal_not_found();
   exit();
 }
 if (isset($dsid) && $dsid != '-') { // Download the given datasteam.
   return makeObject($pid, $dsid);
 }
 drupal_set_title(t('Repository'));
 esdora_repository_page_set_breadcrumbs($pid);
 $is_collection = esdora_has_model($item, ESDORA_COLLECTION_PID);
 $is_collection = FALSE;
 return $is_collection ? theme(THEME_ESDORA_COLLECTION, $item) : theme(THEME_ESDORA_OBJECT, $item);
}

/**
 * Checks if the fedora repository is accessible.
 *
 * @return boolean
 *   TRUE if theu are available, FALSE otherwise.
 */
function esdora_fedora_and_risearch_available() {
  module_load_include('inc', 'fedora_repository', 'api/fedora_utils');
  if (!fedora_available()) {
    $msg = t('The Fedora repository server is currently unavailable. Please contact the site administrator.');
    drupal_set_message($msg, 'warning', FALSE);
    return FALSE;
  }
  if (!risearch_available()) {
    $msg = t('The Fedora resource index search is currently unavailable. Please contact the site administrator.');
    drupal_set_message($msg, 'warning', FALSE);
    return FALSE;
  }
  return TRUE;
}

/**
 * Checks if the given object and datastream exist if specified.
 *
 * @return boolean
 *   TRUE if they exist, FALSE otherwise.
 */
function esdora_object_and_datastream_exist(Fedora_Item $item, $dsid) {
  if (!$item->exists()) {
    drupal_set_message(t("Object '@pid' does not exist!", array('@pid' => $item->pid)), 'error');
    return FALSE;
  }
  if (isset($dsid) && $dsid != '-' && isset($item->datastreams[$dsid])) {
    drupal_set_message(t("Datastream '@dsid' does not exist!", array('@dsid' => $dsid)), 'error');
    return FALSE;
  }
  return TRUE;
}

/**
 * Sets the breadcrumbs for the fedora/repository page.
 *
 * @param Fedora_Item $item
 *   The object to display.
 */
function esdora_repository_page_set_breadcrumbs(Fedora_Item $item) {
  module_load_include('inc', 'fedora_repository', 'ObjectHelper');
  $breadcrumbs = array();
  $object_helper = new ObjectHelper();
  $object_helper->getBreadcrumbs($item->pid, $breadcrumbs);
  drupal_set_breadcrumb(array_reverse($breadcrumbs));
}