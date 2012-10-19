<?php

/**
 * @file
 *
 *  Deals with the display/navigation of collection objects.
 */
module_load_include('inc', 'esdora_islandora', 'esdora.utils');

define('PAGER_ESDORA_COLLECTION_ID', 1);
define('PAGER_ESDORA_COLLECTION_LIMIT', 10);

/**
 * Implements theme_hook().
 */
function theme_esdora_collection(Fedora_Item $item) {
  if (user_access('manage collections')) {
    return tabs_render(array(
        '#type' => 'tabset',
        'collection_view' => array(
          '#type' => 'tabpage',
          '#title' => 'View',
          '#content' => theme(THEME_ESDORA_COLLECTION_MEMBERS, $item)
        ),
        'manage_view' => array(
          '#type' => 'tabpage',
          '#title' => t('Manage Collection'),
          '#content' => theme(THEME_ESDORA_COLLECTION_MANAGE_TAB, $item),
        )
      )
    );
  }
  return theme(THEME_ESDORA_COLLECTION_MEMBERS, $item);
}

/**
 * Implements theme_hook().
 */
function theme_esdora_collection_members(Fedora_Item $item) {
  $query = esdora_get_collection_itql_query($item);
  $members = esdora_itql_query($query);
  $total = count($members);
  if ($total == 0) {
    drupal_set_message(t("No Objects in this collection or bad query."));
    return '';
  }
  $page = esdora_configure_pager(PAGER_ESDORA_COLLECTION_ID, $total, PAGER_ESDORA_COLLECTION_LIMIT);
  $offset = $page *  PAGER_ESDORA_COLLECTION_LIMIT;
  $header = array(t('Name'), t('Type'), t('# of Members'));
  $members = array_slice($members, $offset, PAGER_ESDORA_COLLECTION_LIMIT); // Split out the number to be rendered.
  $members = array_map(function($o) {
               return array(l($o[1], "fedora/repository/{$o[0]}"), $o[2], $o[2] == ESDORA_COLLECTION_PID ? $o[3] : '-');
             }, $members);
  $table = theme_table($header, $members, array(), t('Members'));
  $pager = theme_pager(array(), PAGER_ESDORA_COLLECTION_LIMIT,  PAGER_ESDORA_COLLECTION_ID);
  return $table . $pager;
}

/**
 * Implements template_preprocess_hook().
 */
function template_preprocess_esdora_collection_manage_tab(array &$variables) {
  module_load_include('inc', 'fedora_repository', 'CollectionPolicy');
  module_load_include('inc', 'fedora_repository', 'CollectionManagement');
  module_load_include('inc', 'esdora_islandora', 'esdora_forms');
  $item = &$variables['item'];
  $variables['pid'] = $item->pid;
  $policy = CollectionPolicy::loadFromCollection($item->pid, TRUE);
  $content_models = $policy->getContentModels();
  $variables['add'] = array(
    '#type' => 'fieldset',
    '#title' => t('Ingest Dataset'),
    '#value' => drupal_get_form('dataset_ingest_form', $content_models, $item->pid)
  );
  $variables['manage'] = array(
    '#type' => 'fieldset',
    '#title' => t('Manage Collection'),
    '#value' => drupal_get_form('collection_management_form', $item->pid, $content_models)
  );
  // @todo bring in code remove class.
  module_load_include('inc', 'fedora_repository', 'plugins/FedoraObjectDetailedContent');
  $obj = new FedoraObjectDetailedContent($item->pid);
  $variables['details'] = $obj->showFieldSets();
}
