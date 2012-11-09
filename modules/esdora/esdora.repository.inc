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
 esdora_repository_page_set_breadcrumbs($item);
 esdora_repository_page_add_js_and_css();
 $is_collection = esdora_has_model($item, ESDORA_COLLECTION_PID);
 return $is_collection ? theme(THEME_ESDORA_COLLECTION, $item) : theme(THEME_ESDORA_OBJECT, $item);
}

/**
 * Include the required css/js for rendering the repository page.
 */
function esdora_repository_page_add_js_and_css() {
 // @todo replace with a call to the fancybox module.
 // @note Should fancybox be an optional requirement?
 $path = drupal_get_path('module', 'esdora_islandora');
 drupal_add_css("sites/all/libraries/fancybox/jquery.fancybox-1.3.4.css");
 drupal_add_js("sites/all/libraries/fancybox/jquery.fancybox-1.3.4.js");
 drupal_add_css("$path/css/esdora.css");
 drupal_add_js("$path/js/esdora.js");
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
  if (isset($dsid) && $dsid != '-' && !isset($item->datastreams[$dsid])) {
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
