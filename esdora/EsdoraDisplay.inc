<?php

/**
 * @file
 *
 */
module_load_include('inc', 'fedora_repository', 'api/fedora_utils');
module_load_include('inc', 'fedora_repository', 'api/fedora_item');
module_load_include('inc', 'fedora_repository', 'ObjectHelper');
module_load_include('inc', 'fedora_repository', 'ContentModel');

/**
 * Replaces the default islandora fedora/repository menu hook.
 */
function esdora_menu_alter_fedora_repository($pid = NULL, $dsid = NULL, $collection = NULL, $page_number = NULL, $limit = NULL) {
  // Display the root object by default.
  $pid = ($pid == NULL) ? variable_get('fedora_repository_pid', 'islandora:root') : $pid;
  // Verifiy that we can display the page.
  if(!esdora_menu_alter_fedora_repository_can_display($pid, $dsid)) {
    return '';
  }
  // Verifiy that we are allowed to display the page.
  if(!esdora_menu_alter_fedora_repository_allowed_to_display($pid, $dsid)) {
    drupal_access_denied();
    exit();
  }
  // If DSID is present return the datastream to the client.
  $download_datastream = ($dsid != NULL && $dsid != '-');
  if ($download_datastream) {
    return makeObject($pid, $dsid);
  }
  // Set Title/Breadcrumbs
  drupal_set_title(t('Repository'));
  esdora_menu_alter_fedora_repository_set_breadcrumbs($pid);
  // Display the Object as a Collection or as a Single Object.
  if(esdora_menu_alter_fedora_repository_display_as_collection($pid)) {
    return theme(THEME_ESDORA_ISLANDORA_COLLECTION, $pid, $page_number);
  }
  else if (esdora_menu_alter_fedora_repository_display_as_object($pid)) {
    return theme(THEME_ESDORA_ISLANDORA_OBJECT, $pid);
  }
  else { // Content Model wasn't properly defined.
    drupal_set_message(t('The object has no content model associated with it, please check the object details on Fedora Admin Application.'));
    return '';
  }
}

/**
 * Checks if the fedora/repository page can be rendered with the given arguments.
 *
 * @param string $pid
 *   The PID of the Fedora object to display.
 * @param string $dsid
 *   The DSID of the datastream to display, '-' if no datastream is to be displayed.
 * @return boolean
 *   TRUE if the page can be displayed, FALSE otherwise.
 */
function esdora_menu_alter_fedora_repository_can_display($pid, $dsid) {
  module_load_include('inc', 'fedora_repository', 'api/fedora_utils');
  if (!fedora_available()) {
    drupal_set_message(t('The Fedora repository server is currently unavailable. Please contact the site administrator.'), 'warning', FALSE);
    return FALSE;
  }
  if (!risearch_available()) {
    drupal_set_message(t('The Fedora resource index search is currently unavailable. Please contact the site administrator.'), 'warning', FALSE);
    return FALSE;
  }
  if (!valid_pid($pid)) {
    drupal_set_message(t("Invalid PID!"), 'error');
    return FALSE;
  }
  if (isset($dsid) && !valid_dsid($dsid)) {
    drupal_set_message(t("Invalid DSID!"), 'error');
    return FALSE;
  }
  $item = new fedora_item($pid);
  if (!$item->exists()) {
    drupal_set_message(t("Could not find the object: @pid!", array('@pid' => $pid)), 'error');
    return FALSE;
  }
  return TRUE;
}

/**
 * Checks if the user can view the fedora/repository with the given arguments.
 *
 * @param string $pid
 *   The PID of the Fedora object to display.
 * @param string $dsid
 *   The DSID of the datastream to display, '-' if no datastream is to be displayed.
 * @return boolean
 *   TRUE if the user has access to the given page, FALSE otherwise.
 */
function esdora_menu_alter_fedora_repository_allowed_to_display($pid, $dsid) {
  if (!fedora_repository_access(OBJECTHELPER::$OBJECT_HELPER_VIEW_FEDORA, $pid, $user)) {
    if (user_access('access administration pages')) {
      $msg = 'PIDs may be added to allowed namespaces, or all namespace restrictions removed @here';
      drupal_set_message(t($msg, array('@here' => l('here', 'admin/settings/fedora_repository'))), 'warning');
    }
    return FALSE;
  }
  // Not sure whats going on here...
  if (in_array(-1, module_invoke_all('file_download', "/fedora/repository/$pid"))) {
    return FALSE;
  }
  return TRUE;
}

/**
 * Checks if the given pid should be displayed as a Collection, this is determined by its content model.
 */
function esdora_menu_alter_fedora_repository_display_as_collection($pid) {
  return _esdora_menu_alter_fedora_repository_get_content_model_matching($pid, array(ESDORA_COLLECTION_PID)) !== NULL;
}

/**
 * Checks if the given pid should be displayed as a Object, this is determined by its content model.
 */
function esdora_menu_alter_fedora_repository_display_as_object($pid) {
  return _esdora_menu_alter_fedora_repository_get_content_model_matching($pid, array(ESDORA_COMPUND_DATA_PID, ESDORA_ATOMISTIC_DATA_PID)) !== NULL;
}

/**
 * Fetches the given object's content model if it matches one given in the $models argument.
 */
function _esdora_menu_alter_fedora_repository_get_content_model_matching($pid, array $models) {
  $object_helper = new ObjectHelper();
  $content_models = $object_helper->get_content_models_list($pid);
  foreach ($content_models as $content_model) {
    if (array_search($content_model->pid, $models) !== FALSE) {
      return $content_model;
    }
  }
  return NULL;
}

/**
 * Sets the breadcrumbs for the fedora/repository page.
 *
 * @param string $pid
 *   The PID of the object to display.
 */
function esdora_menu_alter_fedora_repository_set_breadcrumbs($pid) {
  $breadcrumbs = array();
  $object_helper = new ObjectHelper();
  $object_helper->getBreadcrumbs($pid, $breadcrumbs);
  drupal_set_breadcrumb(array_reverse($breadcrumbs));
}

/**
 * Implements theme_hook().
 */
function theme_esdora_islandora_collection($pid, $page_number) {
  module_load_include('inc', 'esdora_islandora', 'plugins/EsdoraCollection');
  $esdora_collection = new EsdoraCollection($pid);
  return $esdora_collection->getContentForDisplay(ContentModel::loadFromModel(ESDORA_COLLECTION_PID), user_access(ObjectHelper::$MANAGE_COLLECTIONS), $pid, $page_number);
}

/**
 * Implements theme_hook().
 */
function theme_esdora_islandora_object($pid, $page_number) {
  if(!user_access(ObjectHelper::$MANAGE_COLLECTIONS)) {
    $content_model = _esdora_menu_alter_fedora_repository_get_content_model_matching($pid, array(ESDORA_COMPUND_DATA_PID, ESDORA_ATOMISTIC_DATA_PID));
    return $content_model->displayExtraFieldset($pid, $page_number);
  }
  else {
    return theme(THEME_ESDORA_ISLANDORA_OBJECT_TABS, $pid);
  }
}

/**
 * Implements theme_hook().
 */
function theme_esdora_islandora_object_tabs($pid, $page_number) {
  $path = drupal_get_path('module', 'esdora_islandora');
  drupal_add_css("sites/all/libraries/fancybox/jquery.fancybox-1.3.4.css");
  drupal_add_js("sites/all/libraries/fancybox/jquery.fancybox-1.3.4.js");
  drupal_add_css($path . '/css/esdora.css');
  drupal_add_js($path . '/js/esdora.js');
  $content_model = _esdora_menu_alter_fedora_repository_get_content_model_matching($pid, array(ESDORA_COMPUND_DATA_PID, ESDORA_ATOMISTIC_DATA_PID));
  return tabs_render(array(
      '#type' => 'tabset',
      'data_view' => array(
        '#type' => 'tabpage',
        '#title' => t('View'),
        '#content' => $content_model->displayExtraFieldset($pid, $page_number)
      ),
      'manage_view' => array(
        '#type' => 'tabpage',
        '#title' => t('Manage Dataset'),
        '#content' => theme(THEME_ESDORA_ISLANDORA_OBJECT_TAB_MANAGE, $pid),
      )
    )
  );
}

/**
 * Implements template_preprocess_hook().
 */
function template_preprocess_esdora_islandora_object_manage_tab(array &$variables) {
  module_load_include('inc', 'fedora_repository', 'plugins/FedoraObjectDetailedContent');
  module_load_include('inc', 'esdora_islandora', 'esdora_forms');
  $pid = $variables['pid'];
  $obj = new FedoraObjectDetailedContent($pid);
  $variables['object_details'] = $object_details = $obj->showFieldSets();
  $variables['manage_view_add_component'] = array(
    'component' => array(
      '#type' => 'fieldset',
      '#title' => t('Add Component'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#value' => drupal_get_form('addstream_form', $pid),
      '#weight' => -2,
    ),
    'viewer' => array(
      '#type' => 'fieldset',
      '#title' => t('Add Viewer'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#value' => drupal_get_form('esdora_addviewer_form', $pid),
      '#weight' => -1,
    )
  );
  $variables['manage_view_edit_fgdc'] = array(
    '#type' => 'fieldset',
    '#title' => t('Edit FGDC'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#value' => drupal_get_form('esdora_islandora_edit_form', $pid, 'FGDC.xml', 'fgdc'), // @TODO remove hard coded references.
  );
  $variables['manage_view_edit_dc'] = array(
    '#type' => 'fieldset',
    '#title' => t('Edit DC'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#value' => drupal_get_form('esdora_islandora_edit_form', $pid, 'DC', 'DC'), // @TODO remove hard coded references.
  );
  $variables['manage_view_lineage'] = array(
    '#type' => 'fieldset',
    '#title' => t('Derivative Form'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#value' => drupal_get_form('esdora_islandora_derivative_form', $pid),
  );
  $variables['add_to_workflow'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add To Workflow'),
    '#collapsible' => FALSE,
    '#value' => drupal_get_form('esdora_add_to_workflow_form', $pid),
  );
}

/**
 * Add datastream form.
 *
 * @param array $form_state
 *   The drupal form state.
 * @param string $pid
 *   The PID of the object to add the datastream to.
 *
 * @return array
 *   The form definition.
 */
function addstream_form(array &$form_state, $pid) {
  return array(
    '#attributes' => array('enctype' => 'multipart/form-data'),
    '#redirect' => "/fedora/repository/$pid",
    'addstream' => array(
      'choices' => array(
        '#title' => t('Select a component type'),
        '#type' => 'radios',
        '#default_value' => 'data',
        '#options' => array(
          'data' => t('Data file'),
          'metadata' => t('Metadata file')
        ),
      ),
      'add-stream-file-location' => array(
        '#type' => 'file',
        '#size' => 48,
        '#title' => t('Upload file'),
        '#description' => t('Choose a file to upload.')
      )
    ),
    'pid' => array(
      '#type' => 'hidden',
      '#value' => $pid
    ),
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Add Datastream')
    )
  );
}

/**
 * Submit handler for the add datastream form.
 *
 * @param array $form
 *   The drupal form.
 * @param array $form_state
 *   The drupal form state.
 */
function addstream_form_submit(array $form, array &$form_state) {
  module_load_include('inc', 'fedora_repository', 'MimeClass');
  global $base_url;
  if (!empty($form_state['submit']) && $form_state['submit'] == 'OK') { // What the hell is this on about?
    $form_state['rebuild'] = TRUE;
    return;
  }
  $values = $form_state['values'];
  $pid = $values['pid'];
  $choices = $values['choices'];
  $metadata = ($choices == 'metadata');
  $file = file_save_upload('add-stream-file-location', array(), 'path', FILE_EXISTS_REPLACE);
  $filepath = $file->filepath;
  if (!$filepath) {
    throw new Exception(t('File not loaded correctly!'));
  }
  $file_directory = dirname($filepath);
  $file_basename = basename($filepath);
  $streamUrl = $base_url . '/' . $file_directory . '/' . drupal_urlencode($file_basename);
  $mime_class = new MimeClass();
  $mime_type = $mime_class->getType($filepath);
  $control_group = "M";
  if ($mime_type == 'text/xml') {
    $control_group = 'X';
  }
  $file_basename = trim($file_basename);
  $file_basename = str_replace(' ', '_', $file_basename);
  $dsid = $file_basename;
  $ds_label = $file_basename;
  try {
    $item = new Fedora_Item($pid);
    $checksum_type = 'DISABLED';
    $checksum = 'none';
    if (!$metadata) {
      $checksum_type = "MD5";
      $checksum = md5_file($filepath);
    }
    $item->add_datastream_from_url($streamUrl, $dsid, $ds_label, $mime_type, $control_group, NULL, $checksum_type, $checksum);
    if ($metadata) {
      module_load_include('inc', 'esdora_islandora', 'plugins/esdora');
      $esdora = new Esdora($pid);
      $esdora->add_relationship_internal('isMetaDataFor', $pid, $dsid, "http://esdora.ornl.gov/ontologies/relationships#");
    }
    file_delete($filepath);
  } catch (Exception $e) {
    drupal_set_message(t('@message', array('@message' => check_plain($e->getMessage()))), 'error');
    return;
  }
  drupal_set_message(t('Data component @file has been added to object @pid.', array('@file' => $file_basename, '@pid' => $pid)));
}

/**
 * Add viewer form.
 *
 * @param array $form_state
 *   The drupal form state.
 * @param string $pid
 *   The PID of the object to add the viewer to.
 *
 * @return array
 *   The form definition.
 */
function esdora_addviewer_form(array &$form_state, $pid) {
  return array(
    'viewer_url' => array(
      '#type' => 'textfield',
      '#title' => t('The URL for the viewer'),
      '#description' => t("Enter full URL, http://your.viwer.here"),
      '#required' => TRUE
    ),
    'dsid' => array(
      '#type' => 'textfield',
      '#title' => t('Enter an data stream ID for the viewer'),
      '#description' => t("A unique identifer for this viewer."),
      '#required' => TRUE
    ),
    'pid' => array(
      '#type' => 'hidden',
      '#value' => $pid
    ),
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Add Viewer')
    )
  );
}

/**
 * Submit handler for the add viewer form.
 *
 * @param array $form
 *   The drupal form.
 * @param array $form_state
 *   The drupal form state.
 */
function esdora_addviewer_form_submit(array $form, array &$form_state) {
  module_load_include('inc', 'esdora_islandora', 'plugins/esdora');
  $values = $form_state['values'];
  $pid = $values['pid'];
  $dsid = $values['dsid'];
  $url = $values['viewer_url'];
  $esdora = new Esdora($pid);
  $item = new Fedora_Item($pid);
  $item->add_datastream_from_url($url, $dsid, $dsid, NULL, 'R'); // Whats going on here?
  $esdora->add_relationship_internal('isViewerFor', $pid, $dsid, "http://esdora.ornl.gov/ontologies/relationships#");
  drupal_set_message(t("Successfully added viewer to @pid", array('@pid' => $pid)));
}
