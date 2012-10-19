<?php

/**
 * @file
 *
 *  Deals with the display/manipulation of objects.
 */

/**
 *
 */
function theme_esdora_object(Fedora_Item $item) {
  module_load_include('inc', 'esdora_islandora', 'esdora.object.tabs');
  if (isset($_GET['tab'])) { // AJAX request to render a single tab.
    return drupal_json(esdora_get_tab($item, $_GET['tab']));
  }
  $path = drupal_get_path('module', 'esdora_islandora');
  drupal_add_js("$path/js/ajaxtabs_compound.js");
  drupal_add_css("$path/css/ajaxtabs.css");
  return theme(THEME_ESDORA_OBJECT_TABS, $item);
}
