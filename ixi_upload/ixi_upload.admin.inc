<?php

/**
 * @file
 * InDesign XML Importer Uploader file
 *
 * Administration page callbacks for InDesign XML Importer Uploader.
 */

/**
 * Form constructor for uploading an archive of XML files.
 *
 * @see http://www.alexweber.com.br/en/articles/how-manually-add-entity-field-forms-custom-drupal-form for
 * information on including field forms in the form.
 */
function ixi_upload_upload_form($form, &$form_state) {
  $form['#attributes']['enctype'] = 'multipart/form-data';
  $form['#parents'] = array(); // necessary for field_default_form() to not yell at us
  $entity_type = 'node';
  $bundle_name = variable_get('ixi_group_content_type');
  $entity = NULL;
  $langcode = LANGUAGE_NONE;

  $node_names = node_type_get_names();
  $form['group_node'] = array(
    '#type' => 'fieldset',
    '#title' => t('New @content_type', array('@content_type' => $node_names[variable_get('ixi_group_content_type')])),
  );

  foreach (variable_get('ixi_group_fields') as $field_name) {
    $items = NULL;
    $field = field_info_field($field_name);
    $instance = field_info_instance($entity_type, $field_name, $bundle_name);
    $field_form = field_default_form($entity_type, $entity, $field, $instance, $langcode, $items, $form, $form_state);
    $form['group_node'] += $field_form;
  }

  $form['archive'] = array(
    '#type' => 'fieldset',
    '#title' => t('Upload Archive'),
  );
  $form['archive']['fid'] = array(
    // '#title' => t('File'),
    '#type' => 'file',
    '#description' => t('Upload a zip archive of XML files.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Upload and Create'),
  );

  return $form;
}

function ixi_upload_upload_form_submit($form, &$form_state) {
  $filepath = 'private://ixi_archives/';
  $file = file_save_upload('fid', array('file_validate_extensions' => array('zip')), $filepath);
  file_save($file);

  if ($file) {
    drupal_set_message('File successfully uploaded.');
  }
  else {
   drupal_set_message('File was not uploaded.', 'error');
  }
}

/**
 * Form constructor for processing a queue of XML files.
 */
function ixi_upload_process_form($form, &$form_state) {
  $form = array();
  return $form;
}

