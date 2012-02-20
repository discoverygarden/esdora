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
            'path' => ahah_helper_path(array('admin/settings/metadata_node')),
            'wrapper' => 'metadata-ahah-wrapper',
            'event' => 'change',
            'method' => 'replace',
        ),
    );

    $titleOptions = islandora_content_model_forms_get_title_options($selectedFormDef);
    $form['admin']['title_option'] = array(
        '#type' => 'select',
        '#options' => $titleOptions,
        '#title' => 'Choose a field to use as the title field of metadata records',
        '#description' => 'The value of this field is used to form the record name.',
        '#required' => TRUE,
    );
    
   $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Submit'),
     );
  
    return $form;
}