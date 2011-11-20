<?php


     function displayContent($pid = NULL, $dsId = NULL, $collection = NULL, $page_number = NULL, $limit = NULL) {
        //copied from fedora_repository.module -> fedora_repository_get_items
        //need to 1) ask Islandora to refactory first portion and 2) refactory here accordingly
        module_load_include('inc', 'fedora_repository', 'CollectionClass');
        module_load_include('inc', 'fedora_repository', 'ObjectHelper');
        module_load_include('inc', 'fedora_repository', 'api/fedora_utils');
        module_load_include('inc', 'fedora_repository', 'api/fedora_item');
        module_load_include('inc', 'esdora_islandora', 'plugins/EsdoraCollection');
        module_load_include('inc', 'fedora_repository', 'ContentModel');
        module_load_include('inc', 'fedora_repository', 'plugins/FedoraObjectDetailedContent');

        global $user;

        if (!fedora_available()) {
            drupal_set_message(t('The Fedora repository server is currently unavailable. Please contact the site administrator.'), 'warning', FALSE);
            return '';
        }

        if (!risearch_available()) {
            drupal_set_message(t('The Fedora resource index search is currently unavailable. Please contact the site administrator.'), 'warning', FALSE);
            return '';
        }

        if ($pid == NULL) {
            $pid = variable_get('fedora_repository_pid', 'islandora:root');
        }

        $item = new fedora_item($pid);
        if (!$item->exists()) {
            drupal_not_found();
            exit();
        }

        if ($pid & !valid_pid($pid)) {
            drupal_set_message(t("Invalid PID!"), 'error');
            return ' ';
        }

        if ($dsId & !valid_dsid($dsId)) {
            drupal_set_message(t("Invalid dsID!"), 'error');
            return ' ';
        }
        if (!fedora_repository_access(OBJECTHELPER::$OBJECT_HELPER_VIEW_FEDORA, $pid, $user)) {
            if (user_access('access administration pages')) {
                drupal_set_message(t("PIDs may be added to allowed namespaces, or all namespace restrictions removed @here", array('@here' => l('here', 'admin/settings/fedora_repository'))), 'warning');
            }
            drupal_access_denied();
            exit;
            return ' ';
        }

        $objectHelper = new ObjectHelper();
        if ($pid == NULL) {
            $pid = variable_get('fedora_repository_pid', 'islandora:root');
        }
        $headers = module_invoke_all('file_download', "/fedora/repository/$pid");
        if (in_array(-1, $headers)) {
            drupal_access_denied();
            exit;
            return ' ';
        }
        if ($dsId != NULL && $dsId != '-') { //if we have a dsID return the stream otherwise query for a collection of objects
//probably should check pid as well here.
            return makeObject($pid, $dsId);
        }

        $content = '<div id="content-fedora">';

        module_load_include('inc', 'fedora_repository', 'CollectionClass');
        $collectionClass = new CollectionClass();
        module_load_include('inc', 'fedora_repository', 'ContentModel');
        module_load_include('inc', 'fedora_repository', 'plugins/FedoraObjectDetailedContent');
        $breadcrumbs = array();
        $objectHelper->getBreadcrumbs($pid, $breadcrumbs);
        drupal_set_breadcrumb(array_reverse($breadcrumbs));


        //Jerry Pan @ Nov 2011
        //now the major GUI refactoring structure here: 
        //
  //A - At Collection Level, if user has the permission to manage 
        //collection, put into a tabbed display with only two tabs on top:
        // 1- the same "View" that ESDORA uses by ESDORA Collection Model, and 2 - the "Manage" that has subtabs of all the functions in
        // the Islandora Collection Model (Add, Manage This Collection, Batch Ingest, Object Details)
        // B - At Dataset Level, if the uer has the permission to mange,
        // same two tabs display, the the "Manage" tab now would have DC editor, FGDC editor, and Object details (with adding datastreams) better 
        // to pull the add datastream functions out from Object details form
        // 
        //check the permission first, simplify to one permission only
        $hasManagePermission = false;
        if (user_access($objectHelper->MANAGE_COLLECTIONS)) {
            $hasManagePermission = true;
        }

        //$offset = $limit * $page_number; //this is never used ; @TODO
        $content_models = $objectHelper->get_content_models_list($pid);

        $esdora_collection_model = NULL;
        $esdora_data_model = NULL;
        foreach ($content_models as $content_model) {
            //is there a betterway to get the nature of the object (collection or data)?
            if ($content_model->pid == ESDORA_COLLECTION_PID) {
                $esdora_collection_model = $content_model;
            }
            if ($content_model->pid == ESDORA_COMPUND_DATA_PID || $content_model->pid == ESDORA_ATOMISTIC_DATA_PID) {
                $esdora_data_model = $content_model;
            }
        }

        if ($esdora_collection_model != NULL) { //collection object
            //esdora collection object
            //because this code dirctly get the management features,
            //we do not need IslandoraCollectionCmodel to be associated with collection object anymore ! yeah !
            //and hence the freedom to choose a subset to display and the way we want to display
            $esdoraCollection = new EsdoraCollection($pid);
            return $esdoraCollection->getContentForDisplay($esdora_collection_model, $hasManagePermission, $pid, $page_number);
        } else { //data object
            //TODO: need to find a place on GUI to put back management on object details
            //TODO: check permission
            $data_display = $esdora_data_model->displayExtraFieldset($pid, $page_number);
            if (!$hasManagePermission) {
                //this calls into Islandora content model so the model objects are still needed
                return $data_display;
            }
            //has the permission, show a tabbed display with View and Manage tabs

            $obj = new FedoraObjectDetailedContent($pid);
            $object_details = $obj->showFieldSets();

            $manage_view_add_component = array();
            $manage_view_add_component['component'] = array(
                '#type' => 'fieldset',
                '#title' => 'Add Component',
                '#collapsible' => TRUE,
                '#collapsed' => FALSE,
                '#value' => drupal_get_form('addstream_form', $pid),
                '#weight' => -2,
            );
            $manage_view_add_component['viewer'] = array(
                '#type' => 'fieldset',
                '#title' => 'Add Viewer',
                '#collapsible' => TRUE,
                '#collapsed' => FALSE,
                '#value' => drupal_get_form('esdora_addviewer_form', $pid),
                '#weight' => -1,
            );

            $manage_view['Components'] = array(
                '#type' => 'tabpage',
                '#title' => 'Add',
                '#selected' => TRUE,
                '#content' => drupal_render($add_component_content),
                '#weight' =>-100,
            );

//        $edit_metadata_urls = l('Edit DC', $base_url . base_path() . 'esdora/render/form/' . $this->pid . '/DC/DC/');
//        $edit_metadata_urls .= ' / ' . l(' Edit FGDC', $base_url . base_path() . 'esdora/render/form/' . $this->pid . '/FGDC.xml/fgdc/');
//        drupal_get_form('esdora_islandora_edit_form', $pid, $dsid, $form_name);
            $edit_metadata['fgdc'] = array(
                '#type' => 'fieldset',
                '#title' => t('Edit FGDC'),
                '#collapsible' => TRUE,
                '#collapsed' => FALSE,
                '#value' => drupal_get_form('esdora_islandora_edit_form', $pid, 'FGDC.xml', 'fgdc'),
                '#weight' => -1,
            );
            $edit_metadata['dc'] = array(
                '#type' => 'fieldset',
                '#title' => t('Edit DC'),
                '#collapsible' => TRUE,
                '#collapsed' => TRUE,
                '#value' => drupal_get_form('esdora_islandora_edit_form', $pid, 'DC', 'DC'),
                '#weight' => -2,
            );
            $manage_view['Metadata'] = array(
                '#type' => 'tabpage',
                '#title' => 'Edit',
                '#content' => drupal_render($edit_metadata),
            );

            $fieldSetDF = array(
                '#type' => 'fieldset',
                '#title' => t('Derivative Form'),
                '#collapsible' => TRUE,
                '#collapsed' => FALSE,
                '#value' => drupal_get_form('esdora_islandora_derivative_form', $pid),
            );
            $fieldSetcontent = theme('fieldset', $fieldSetDF);

            $manage_view['Lineage'] = array(
                '#type' => 'tabpage',
                '#title' => 'Lineage',
                '#content' => $fieldSetcontent,
            );
            $manage_view = array_merge($manage_view, $object_details);
            $esdora_tabs = array(
                '#type' => 'tabset',
            );
            $esdora_tabs['data_view'] = array(
                '#type' => 'tabpage',
                '#title' => 'View',
                '#content' => $data_display,
            );
            
            //experiment with lightbox
 
drupal_add_css("sites/all/libraries/fancybox/jquery.fancybox-1.3.4.css"); 
drupal_add_js("sites/all/libraries/fancybox/jquery.fancybox-1.3.4.js");  
drupal_add_js (        
    '$(document).ready(function(){
        $("div.manage_dataset a").fancybox({
            \'overlayShow\': true,
            \'titlePosition\' : \'inside\',
            \'transitionIn\' : \'none\',
            \'transitionOut\' : \'none\',
        });
     }); ',
    'inline');

$url_add_component = "#dataset_add_component";
$url_edit_fgdc = "#dataset_add_component";
$url_edit_dc = "#dataset_add_component";
$url_edit_lineage = "#dataset_add_component";
$url_object_details = "#dataset_add_component";

$my_manage_view = '<div class="manage_dataset">';
$my_manage_view .= '<fieldset><legend>Choose a management option</legend>';
$my_manage_view .= '<ul>';
$my_manage_view .= '<li><a href=' . $url_add_component . '>Add a component</a></li>';
$my_manage_view .= '<li><a href=' . $url_edit_fgdc . '>Edit Metadata (FGDC)</a></li>';
$my_manage_view .= '<li><a href=' . $url_edit_dc . '>Edit Metadta (DC) </a></li>';
$my_manage_view .= '<li><a href=' . $url_edit_lineage . '>Data Lineage</a></li>';
$my_manage_view .= '<li><a href=' . $url_object_details . '>Object Details</a></li></ul></fieldset></div>';

$my_manage_view .= '<div style="display: none;">                
        <div id="dataset_add_component" style="width:800px; height:650px;overflow:auto;">'
                . drupal_render($manage_view_add_component) .   
        '<p/><br/><a onclick="$.fancybox.close();" href="javascript:;"><center>Close</center></a></div>
        </div> ';

            $esdora_tabs['manage_view'] = array(
                '#type' => 'tabpage',
                '#title' => t('Manage Dataset'),
                '#content' => $my_manage_view,
            );
            return tabs_render($esdora_tabs);
        }
    }
    
    
    function addstream_form(&$form_state, $pid) {
            
    $form = array();

    $options = array(
        'data' => t('Data file'),
        'metadata' => t('Metadata file'),
    );
    $form['addstream']['choices'] = array(
      '#title' => t('Select a component type'), 
      '#type' => 'radios', 
      '#options' => $options,
      '#default_value' =>'data',
      );
    $form['#attributes'] = array('enctype' => 'multipart/form-data');
    $form['addstream']['add-stream-file-location'] = array(
        '#type' => 'file',
        '#title' => t('Upload file'),
        '#size' => 48,
        '#description' => t('Choose a file to upload.'),
        );
    
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Add Datastream'),
     );
    
   $form['#redirect'] = "/fedora/repositroy/$pid";

    $form['pid'] = array(
      '#type' => 'hidden',
      '#value' => "$pid"
    );

    return $form;
}

function addstream_form_submit($form, &$form_state) {
   $validators = array();
   global $base_url;
  if (!empty($form_state['submit']) && $form_state['submit'] == 'OK') {
    $form_state['rebuild'] = TRUE;
    return;
  }
  module_load_include('inc', 'fedora_repository', 'MimeClass');
  module_load_include('inc', 'fedora_repository', 'ObjectHelper');
  module_load_include('inc', 'fedora_repository', 'api/fedora_item');

  $pid = $form_state['values']['pid'];
  $choices = $form_state['values']['choices'];
  $metadata = ($choices=='metadata');
  
  
// we dont use the following fields: file name is the dsid and label
//     $dsid = $form_state['values']['stream_id'];
//  $dsLabel = $form_state['values']['stream_label'] . substr($file, strrpos($file, '.')); // Add the file extention to the end of the label.;
  $fileObj = file_save_upload('add-stream-file-location', $validators, 'path', FILE_EXISTS_REPLACE);
  $file = $fileObj ->filepath;
  if(!$file) {
      throw new Exception("file not loaded correctly!");
  }
  $file_directory = dirname($file);
  $file_basename = basename($file);  
  $streamUrl = $base_url . '/' . $file_directory . '/' . drupal_urlencode($file_basename);
  $mimetype = new MimeClass();
  $dformat = $mimetype->getType($file);
  $controlGroup = "M";
  if ($dformat == 'text/xml') {
    $controlGroup = 'X';
  }
  $file_basename = trim($file_basename);
  $file_basename = str_replace(' ', '_', $file_basename);
  $dsid=$file_basename;
  $dsLabel=$file_basename;
  try {
    $item = new Fedora_Item($pid);
    //@TODO Checksum not working...
    $item->add_datastream_from_url($streamUrl, $dsid, $dsLabel, $dformat, $controlGroup, NULL, "MD5", '');
    //add the relationship: isMetadataFor if its metadata
    if($metadata) {
       $esdora = new Esdora($pid);
       $esdora->add_relationship_internal('isMetaDataFor', $pid, $dsid, "http://esdora.ornl.gov/ontologies/relationships#");
    }
    file_delete($file);
  } catch (exception $e) {
    drupal_set_message(t('@message', array('@message' => check_plain($e->getMessage()))), 'error');
    return;
  }
  drupal_set_message("Data component $file_basename has been added to object $pid.");
  $form_state['rebuild'] = TRUE;
}

function esdora_addviewer_form($form_state, $pid) {
    $form['is_viewer'] = array();
    $form['is_viewer']['viewer_url'] = array(
        '#type' => 'textfield',
        '#title' => t('The URL for the viewer'),
        '#required' =>TRUE,
        '#description' =>t("enter full URL, http://your.viwer.here"),
    );
    $form['is_viewer']['stream_id'] = array(
        '#type' => 'textfield',
        '#title' => t('Enter an stream ID for the viewer'),
        '#required' =>TRUE,
        '#description' =>t("a unique identifer for this viewer."),
    );
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Add Viewer'),
     );
    
    $form['pid'] = array(
      '#type' => 'hidden',
      '#value' => "$pid"
    );

    return $form;
 }
 
 function esdora_addviewer_form_validate($form, &$form_state) {
     //TODO validate URL here     
 }
 
 function esdora_addviewer_form_submit($form, &$form_state) {
    module_load_include('inc', 'esdora_islandora', 'plugins/esdora');
    $esdora = new Esdora($form_state['values']['pid']);
    if (!empty($form_state['values']['viewer_url'])) {
        $viewer_url = $form_state['values']['viewer_url'];
        $filtered_url = $viewer_url; //filter_var($viewer_url, FILTER_VALIDATE_URL);
        if (!isset($filtered_url)) {
            drupal_set_message(t('You must enter a valid URL in the URL for Viewer textbox'));
            return ' ';
        }
        //module_load_include('inc', 'fedora_repository', 'plugins/fedora_item');
        $pid = $form_state['values']['pid'];
        $dsid = $form_state['values']['stream_id'];
        $dsLabel = $dsid;
        $item = new Fedora_Item($pid);
        $item->add_datastream_from_url($filtered_url, $dsid, $dsLabel, NULL, 'R');
        $esdora->add_relationship_internal('isViewerFor', $pid, $dsid, "http://esdora.ornl.gov/ontologies/relationships#");
        drupal_set_message(t('Successfully added viewer to @pid'),array('@pid'=>$pid));
        return ' '; //we don't want to do add an actual file as we have already added the datatstream here and it is externally managed by Fedora
    }
}

?>
