<?php

function metadata_import_form() {
    $form = array();
    
    $form['file_import'] = array (
        '#type' => 'fieldset',
        '#title' => t('Import Metadata Record'),
    );    
    $form['file_import']['title'] = array(
        '#type' => 'markup', //??
        '#title' => t('Please choose file locally, or enter a remote URL:'),
    );
    
    $form['#attributes'] = array('enctype' => 'multipart/form-data');

    $form['file_import']['file_upload'] = array(
        '#type' => 'file',
        '#title' => t('Upload a metadata record'),
        '#size' => 48,
        '#description' => t('Choose a local file.'),
    );
    
    $form['file_import']['remote_url'] = array(
        '#prefix' => '<p/><strong>Or enter remote location:</strong><p/>',
        '#type' => 'textfield',
        '#title' => t('Remote URL'),
        '#size' => 48,
        '#description' => t('Choose a remote file.'),     
        //this is a test  @todo remove me
        '#autocomplete_path' => AUTOCOMPLETE_TEST,
    );
        
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Submit'),
    );
    return $form;
}


function metadata_import_form_validate(&$form, &$form_state) {
    $source = 'file_upload';
    $hasFileUpload = isset($_FILES['files']) && 
        $_FILES['files']['name'][$source] && 
        is_uploaded_file($_FILES['files']['tmp_name'][$source]);
    $url = $form_state['values']['remote_url'];
    if( !$hasFileUpload  && strlen($url)==0) {
        form_set_error("file_import", "Please upload a file or enter remote URL");
    }
}

function metadata_import_form_submit(&$form, &$form_state) {
    $file = file_save_upload('file_upload', null, file_directory_path());
    $xml_content = '';
    if($file) {
        //file uploaded, processing the file        
        $xml_content=file_get_contents($file->destination);
    }
    //so if file was uploaded, it is processed above and returned. only if no file
    //and a url is entered, we process the remote url here
    $url = $form_state['values']['remote_url'];
    if(!file && isset($url) && strlen($url>0)) {
        $xml_content = simplexml_load_file($url);
    }
    // TODO validate the file? How?, needs to be xml file, well-formed, and against schema?
    
    //now call the code to make a node and attach the file
   /*
    * 1) load the xml, get the title
    * 2) save the file with the title as the name to the correct location
    * 3) create the node with the link
    * 4) done, set a message
    */
   $doc = DOMDocument::loadXML($xml_content);

   if($doc) {
       $node = new stdClass();
       createUpdateNode($doc, $node);
   }
   drupal_goto(VIEW_METADATA_PATH);            
}