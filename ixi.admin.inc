<?php

/**
 * @file
 * IXI: XML Importer file
 *
 * Administration page callbacks for IXI: XML Importer.
 */

/**
 * Form constructor for configuring the XML importer.
 */
function ixi_config_form($form, &$form_state) {
  $form['ixi_group_content_type'] = array(
    '#type' => 'select',
    '#title' => t('Group content type'),
    '#options'  => _ixi_get_content_types(),
    '#description' => t('What content type should be created when uploading new files?'),
    '#default_value' => variable_get('ixi_group_content_type'),
    '#required' => TRUE,
  );
  $form['ixi_group_fields'] = array(
    '#type' => 'select',
    '#title' => t('Group fields'),
    '#options' => _ixi_get_content_type_fields(variable_get('ixi_group_content_type')),
    '#default_value' => variable_get('ixi_group_fields'),
    '#multiple' => TRUE,
  );
  return system_settings_form($form);
}

function _ixi_get_content_types() {
  $content_types = array();
  foreach (node_type_get_types() as $key => $content_type) {
    $content_types[$key] = $content_type->name;
  }
  return $content_types;
}

function _ixi_get_content_type_fields($content_type) {
  if (is_null($content_type)) {
    return array();
  }
  else {
    $fields = array();
    foreach (field_info_instances('node', $content_type) as $key => $field) {
      $fields[$key] = $field['label'];
    }
    return $fields;
  }
}

function ixi_recreate_archive_directory_form() {
  return confirm_form(array(), t('Do you want to attempt to recreate the archives upload directory?'), 'admin/reports/status', t('This will attempt to recreate and make writable the upload archives directory: <em>%directory</em>.', array('%directory' => 'private://ixi_archives')), t('Recreate archive directory'), t('Cancel'));
}

function ixi_recreate_archive_directory_form_submit($form, &$form_state) {
  _ixi_recreate_archive_directory();
  $form_state['redirect'] = 'admin/reports/status';
}

function _ixi_recreate_archive_directory() {
  $directory = 'private://ixi_archives';
  if (!file_prepare_directory($directory, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS)) {
    drupal_set_message(t('Could not create the directory %directory.', array('%directory' => $directory)), 'error');
  }
}
