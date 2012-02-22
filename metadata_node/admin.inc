<?php

function createAdminForm($form_state) {

    //module_load_include('module', 'islandora_content_model_forms');
    // Register the form with ahah_helper so we can use it. Also updates
    // $form_state['storage'] to ensure it contains the latest values that have
    // been entered, even when the form item has temporarily been removed from
    // the form. So if a form item *once* had a value, you *always* can retrieve
    // it.
    ahah_helper_register($form, $form_state);

    $formDefList = islandora_content_model_forms_get_form_names();

    if (count($formDefList) == 0) {
        drupal_set_message('No forms defined, both forms and content models must be defined to add new associations.', 'error');
        return;
    }
    $form = array();
    $form['admin'] = array(
        '#type' => 'fieldset',
        '#prefix' => '<div id="metadata-ahah-wrapper">', // This is our wrapper div
        '#suffix' => '</div>',
        '#tree' => TRUE,
    );

    $selectedFormDef = isset($form_state['values']['admin']['xml_form_def']) ?
            $form_state['values']['admin']['xml_form_def'] :
            key($formDefList);
    //select a list of form definitions
    $form['admin']['xml_form_def'] = array(
        '#type' => 'select',
        '#options' => $formDefList,
        '#default_value' => $selectedFormDef,
        '#title' => 'Choose a form definition',
        '#description' => 'If nothing to select, please use the form editor tool to create form definitions.',
        '#ahah' => array(
            //'path' => 'admin/content/model/forms/titles',
            'path' => ahah_helper_path(array(CONFIG_PATH)),
            'wrapper' => 'metadata-ahah-wrapper',
            'event' => 'change',
            'method' => 'replace',
        ),
    );
    
    $titleOptions = islandora_content_model_forms_get_title_options($selectedFormDef);
    $selectedTitleOption = variable_get('title_field', null);
    if(!isset ($selectedTitleOption)) {
        $selectedTitleOption = isset($form_state['values']['admin']['title_option']) ?
                $form_state['values']['admin']['title_option'] :
                key($titleOptions);
    }

    $form['admin']['title_option'] = array(
        '#type' => 'select',
        '#options' => $titleOptions,
        '#default_value' => $selectedTitleOption,
        '#title' => 'Choose a title field',
        '#description' => 'The value of this field is used to form the record name.',
        '#required' => TRUE,
    );

    $templateOptions = getTemplates();
    $templateOptions['No Tempalte'] = 'No Tempalte';
    $form['admin']['template_option'] = array(
        '#type' => 'select',
        '#options' => $templateOptions,
        '#default_value' => variable_get(METADATA_TEMPLATE_FILE, NULL),
        '#title' => 'Choose a template',
        '#description' => 'The template values are used as the starting values of a new record.',
    );

   $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Submit'),
     );
  
    return $form;
}

function getTemplates() {
    $path = drupal_get_path('module', 'metadata_node');
    $path = $path . '/templates';
    $fileList = scandir($path);
    $output = array();
    foreach($fileList as $file) {
        $fullName = $path . '/' . $file;
        if(is_file($fullName)) {
            $output[$fullName] = $file;
        }
    }
    return $output;
}