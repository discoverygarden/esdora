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
  // @todo replace with a call to the fancybox module.
  // @note Should fancybox be an optional requirement?
  drupal_add_css("sites/all/libraries/fancybox/jquery.fancybox-1.3.4.css");
  drupal_add_js("sites/all/libraries/fancybox/jquery.fancybox-1.3.4.js");
  $path = drupal_get_path('module', 'esdora_islandora');
  drupal_add_css("$path/css/esdora.css");
  drupal_add_js("$path/js/esdora.js");
  drupal_add_js("$path/js/ajaxtabs_compound.js");
  drupal_add_css("$path/css/ajaxtabs.css");
  return theme(THEME_ESDORA_OBJECT_TABS, $item);
}
