<?php

//All esdora forms should be moved here for easy management
function dataset_ingest_form(&$form_state, $content_models, $collection_pid) {
    //temp hack to get rid of collection model, only data model applies
    $model_options = array();
    foreach ($content_models as $model) {
        $cmodel_name = $model->getName();
        $cmodel_pid  = $model ->pid;
        if(!strpos($cmodel_pid, 'Collection')) {
            $model_options[$cmodel_pid] = $cmodel_name;
        }
    }
    $form = array();
    $form['#attributes'] = array('enctype' => 'multipart/form-data');
    $form['model'] = array(
        '#type' => 'select',
        '#title' => 'Choose a content model',
        '#options' => $model_options,
    );
    $form['file_upload'] = array(
        '#type' => 'file',
        '#title' => t('Upload a zipped package file'),
        '#size' => 48,
//        '#required' => TRUE,
        '#description' => t('Please create a zipped ingestion package with "Data" and "Metadata" subfolders. 
            Place FGDC.xml and other descriptive files in the "Metadata" subfolder; and place
            data files in the "Data" subfolder.'),
    );
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Add Dataset'),
    );
    $form['#redirect'] = "/fedora/repository/$collection_pid";
    $form['collection_pid'] = array(
        '#type' => 'hidden',
        '#value' => "$collection_pid"
    );
    return $form;
}

function dataset_ingest_form_validate(&$form, &$form_state) {
    if($form_state['values']['file_upload']==NULL) {
        drupal_set_message('File entry is required, please sellect a pre-prepared zip file to upload');
        return false;
    }
}

function dataset_ingest_form_submit(&$form, &$form_state) {
    $tmp_directory = file_directory_temp(); //drupals tmp directory could be the same as sys temp
    //$form_build_id = $form_state['values']['yui_form_build_id']; //the id of the form being submitted should match the directory name, we injected this into the form before we post the upload
    //$files_dir = "$tmp_directory/$form_build_id";
    if ($file = file_save_upload('file_upload', null, file_directory_path())) {
        module_load_include('inc', 'fedora_repository', 'ContentModel');
        $collection_pid = $form_state['values']['collection_pid'];
        $cmodel_pid = $form_state['values']['model'];
        assert(isset($cmodel_pid));
        assert(isset($collection_pid));
        
        //handle ingest from model generically
        $cmodel = ContentModel::loadFromModel($cmodel_pid);
        $handlers=$cmodel->xml->getElementsbyTagName("package_ingest_handler");
        if(!isset ($handlers)) {
            throw new Exception("Content model $cmodel_pid does not define a 'package_ingest_handler', which is required for this action");
        }
        $handler = $handlers->item(0); //Q - multiple handlers is possible here, but not needed for now
        //the following will call into EsdoraIngest.inc method handleZipIngestForm(), 
        //or something similar as defined by the model object
        $module = $handler->getAttribute('module');
        $class = $handler->getAttribute('class');
        $methodName = $handler->getAttribute('method');
        $moduleFile = $handler->getAttribute('file');
        $path = drupal_get_path('module', !empty($module) ? $module : 'esdora_islandora') . '/' . $method->getAttribute('file');
        if (!file_exists($path)) {
            drupal_set_message('Ingest handler file \'' . $path . '\' does not exist.', 'error');
            return;
        }
        @require_once($path);
        $className = $method->getAttribute('class');
        $methodName = ($method->getAttribute('method'));
        if (!class_exists($className)) {
            drupal_set_message('Ingest handler: class \'' . $className . ' does not exist.', 'error');
            return;
        }
        try {
            $class = new $className;
            $class->$methodName($collection_pid, $file);
        //example call:
        //    $esdora_ingest = new EsdoraIngest();
        //    $esdora_ingest->handleZipIngestForm($collection_pid, $file);                
        }
        catch (Exception $e) {
            Drupal_set_message('Error calling Ingest handler method \'' . $className . '->' . $methodName . $e->getMessage());
            throw $e;
       }
   }
}
?>