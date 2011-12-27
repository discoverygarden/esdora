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
    $form['dataset_name'] = array(
        '#type' => 'textfield',
        '#title' => t('Dataset name'),
        '#size'  => 48,
        '#description' => t('Please enter a unique name for this dataset.'),
        '#required' => TRUE,
    );
    
    $form['add_to_workflow'] = array(
        '#type' => 'checkbox',
        '#title' => t('Add to workflow'),
        '#description' => t('If checked, the dataset will not be publised until approval.'),
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
            //todo add to workflow if checked.
            $add_to_work_flow=$form_state['values']['add_to_workflow'];
            if($add_to_work_flow) {
                  module_load_include('inc','islandora_workflow','islandora_workflow');
                  $result = islandora_workflow_init_workflow($new_pid);
                  if($result){
                    drupal_set_message(t("Successfully added @pid to collection @collection_pid, and the object is ready for workflow",
                            array('@pid'=>$new_pid,'@collection_pid'=>$collection_pid)));
                    drupal_goto('/islandora_workflow'); //TODO correct tab?? landing to the pid?
                  } else {
                    drupal_set_message (t('Could not add object @pid to workflow',array('@pid'=>$new_pid)));
                  }                      
            }
            else {
                drupal_set_message(t("Successfully added @pid to collection @collection_pid, and the object is published",
                        array('@pid'=>$new_pid,'@collection_pid'=>$collection_pid)));
                drupal_goto('/fedora/repository/' . $new_pid);
            }
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
        //or even better, use a template to add one anyways? @TODO
        drupal_set_message("No file is uploaded. If the problem persists, please notify the administer of this problem");
    }
}

/**
 * creates a form on that we are currently calling from the history tab
 * @param array $form_state
 * @param string $pid
 * @return string
 */
function esdora_islandora_derivative_form(&$form_state, $pid) {
    if (!isset($pid)) {
        drupal_set_message(t("Can't create derivative form no pid supplied"));
        return;
    }
    module_load_include('inc', 'fedora_repository', 'api/fedora_item');
    $item = new Fedora_Item($pid);
    $txt = $item->get_datastream_dissemination('processing.txt');
    $rels = $item->get_datastream_dissemination('RELS-EXT');
    $derivatives_string = '<b>Derived from:</b> ';
    $src = new DOMDocument('1.0', 'utf-8');
    $src->formatOutput = true;
    $src->preserveWhiteSpace = false;

    //Loading extern file
    $src->loadXML($rels);
    $isDerivedFrom = $src->getElementsByTagName('isDerivedFrom');
    foreach ($isDerivedFrom as $derived) {
        $derivatives_string .= substr($derived->getAttribute('rdf:resource'), 12) . ', ';
    }
    //$form['redirect'] = $base_url.'fedora/repository/'.$pid;
    $form = array();
    $form['txt'] = array(
        '#type' => 'textarea',
        '#title' => t('Text'),
        '#default_value' => $txt,
        '#description' => t('Text contained in the processing stream'),
    );
    $form['related_pid'] = array(
        '#prefix' => $derivatives_string,
        '#type' => 'textfield',
        '#title' => t('Add Derived From relationship'),
        '#maxlength' => 64,
        '#description' => t('comma seperated list of pids. If this object is already marked as being derived from an object then you don\'t need to mention the object here.  This is for new relationships.'),
    );
    $form['pid'] = array(
        '#type' => 'hidden',
        '#value' => $pid,
    );
    $form['tab'] = array(//the tabs are built using ajax and expect a post varible of tab we know we are on the History tab so add it here
        '#type' => 'hidden',
        '#value' => 'History',
    );
    $form['submit'] = array('#type' => 'submit', '#value' => t('Submit'));
    return $form;
}


function esdora_add_to_workflow_form(&$form_state, $pid) {
    $form['confirm']['are you sure'] = array(
    '#type'  =>  'markup',
    '#value' => t('<strong>Are you sure?</strong> <br/> '),
    '#description' => t('Click OK will set the object as inactive untill the workflow is complete.'),
    );
    $form['confirm']['desc'] = array(
    '#type'  =>  'markup',
    '#value' => t('Click OK will set the object as inactive untill the workflow is complete. <br/>'),
    );
    $form['confirm']['pid'] = array(
        '#type' => 'hidden',
        '#value' => $pid,
    );

    $form['confirm']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('OK'),
    );
    $form['confirm']['cancel'] = array(
      '#type' => 'button',
      '#value' => t('Cancel'),  
      '#attributes' => array(
          'onclick' => '$.fancybox.close();',
          'onsubmit' => 'return false',
          ),
    );
    
    return $form;

}

function esdora_add_to_workflow_form_submit(&$form, &$form_state) {
    //add the pid to workflow....
    $pid =$form_state['values']['pid'];
    if(isset ($pid)) {
        module_load_include('inc','islandora_workflow','islandora_workflow');
        $result = islandora_workflow_init_workflow($pid);
        if($result){
            drupal_set_message(t("Successfully added @pid to workflow",
                            array('@pid'=>$pid) ));
            drupal_goto('/islandora_workflow'); //TODO correct tab?? landing to the pid?
        } else {
            drupal_set_message (t('Could not add object @pid to workflow',array('@pid'=>$pid)));
        }                      
    }
    
}
