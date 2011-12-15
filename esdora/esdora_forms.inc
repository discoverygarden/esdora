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
    $form['model'] = array(
        '#type' => 'select',
        '#title' => 'Choose a content model',
        '#options' => $model_options,
    );
    $form['#attributes'] = array('enctype' => 'multipart/form-data');
    $form['file_upload'] = array(
        '#type' => 'file',
        '#title' => t('Upload a zipped package file'),
        '#size' => 48,
        //'#required' => TRUE, //TODO: why not???
        '#description' => t('Please create a zipped ingestion package with "Data" and "Metadata" subfolders. 
            Place FGDC.xml and other descriptive files in the "Metadata" subfolder; and place
            data files in the "Data" subfolder.'),
    );
    $form['dataset_name'] = array(
        '#type' => 'textfield',
        '#title' => t('Dataset name'),
        '#size'  => 48,
        '#description' => t('Please enter a name for this dataset (unique under the current collection'),
        '#required' => TRUE,
    );
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Add Dataset'),
    );
    //$form['#redirect'] = "/fedora/repository/$collection_pid";
    $form['collection_pid'] = array(
        '#type' => 'hidden',
        '#value' => "$collection_pid"
    );
    return $form;
}

function dataset_ingest_form_validate(&$form, &$form_state) {
    if (!($file = file_save_upload('file_upload', null, file_directory_path()))) {
        form_set_error('File entry is required, please sellect a pre-prepared zip file to upload');
        drupal_set_message('File entry is required, please sellect a pre-prepared zip file to upload');
        return FALSE;
    }
//    $dataset_name=$form_state['values']['dataset_name'];
//    if(!isset($dataset_name) || empty ($dataset_name)) {
//        form_set_error('Dataset name is required, please enter a dataset name');
//        return FALSE;        
//    }
    $form_state['rebuild'] = FALSE;
}

function dataset_ingest_form_submit(&$form, &$form_state) {
    $tmp_directory = file_directory_temp(); //drupals tmp directory could be the same as sys temp
    //$form_build_id = $form_state['values']['yui_form_build_id']; //the id of the form being submitted should match the directory name, we injected this into the form before we post the upload
    //$files_dir = "$tmp_directory/$form_build_id";
    if ($file = file_save_upload('file_upload', null, file_directory_path())) {
       try {    
            $collection_pid = $form_state['values']['collection_pid'];
            $cmodel_pid = $form_state['values']['model'];
            $dataset_name=$form_state['values']['dataset_name'];

            //handle ingest from model generically
            module_load_include('inc', 'esdora_islandora', 'EsdoraDataContentModel');
            $cmodel = EsdoraDataContentModel::loadFromModel($cmodel_pid);
            $new_pid=$cmodel -> ingestZippedPackage($collection_pid, $file, $dataset_name);
            drupal_set_message(t("Successfully added @pid to collection @collection_pid",array('@pid'=>$new_pid,'@collection_pid'=>$collection_pid)));
            drupal_goto('/fedora/repository/' . $new_pid);
        }  
        catch (Exception $e)   {
                //set message here
                drupal_set_message("Ingesting of zipped package encountered a problem, $e->getMessage()");
                return FALSE;
        }
        unlink($file->file_delete($file->filepath));
    }
    else {
        //file did not upload correct, set msg
        drupal_set_message("No file is uploaded. If the problem persists, please notify the administer of this problem");
    }
}