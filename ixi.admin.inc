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
function _ixi_config_form($form, &$form_state) {
  $form['ixi_group_content_type'] = array(
    '#type' => 'select',
    '#title' => t('Group content type'),
    '#options'  => _ixi_get_content_types(),
    '#description' => t('What content type should be created when uploading new files?'),
    '#default_value' => variable_get('ixi_group_content_type'),
    '#required' => TRUE,
    '#ajax' => array(
      'event' => 'change',
      'callback' => '_ixi_ajax_group_fields',
      'wrapper' => 'group-fields-wrapper',
    ),
  );

  // This allows us to update the list of group fields with ajax when the
  // group node is changed. This is the wrapper div that actually gets updated.
  $form['wrapper'] = array(
    '#prefix' => '<div id="group-fields-wrapper">',
    '#suffix' => '</div>',
  );

  // Here we want to check if the form state has a value, and update the fields
  // based on that. If not, then we check to see if there is already a saved
  // value and use the fields from that.
  $options = array();
  if (isset($form_state['values']['ixi_group_content_type'])) {
    $options = _ixi_get_content_type_fields($form_state['values']['ixi_group_content_type']);
  }
  elseif (!is_null(variable_get('ixi_group_content_type'))) {
    $options = _ixi_get_content_type_fields(variable_get('ixi_group_content_type'));
  }

  // This include the actual form item on the page; this concludes the ajax
  // stuff.
  $form['wrapper']['ixi_group_fields'] = array(
    '#type' => 'select',
    '#title' => t('Group fields'),
    '#options' => $options,
    '#default_value' => variable_get('ixi_group_fields'),
    '#multiple' => TRUE,
  );

  $form['ixi_image_extensions'] = array(
    '#type' => 'select',
    '#title' => t('Image Extesions'),
    '#description' => t('Image extensions to automatically process and extract to public folder.'),
    '#options' => array(
      'bmp' => 'bmp',
      'gif' => 'gif',
      'jpg' => 'jpg',
      'jpeg' => 'jpeg',
      'png' => 'png',
    ),
    '#default_value' => variable_get('ixi_image_extensions'),
    '#multiple' => TRUE,
  );
  $form['ixi_extra_extensions'] = array(
    '#type' => 'textfield',
    '#title' => t('Restrict File Extensions'),
    '#description' => t("Enter file extensions that you would like to restrict in the extra files array, separated by a ';'. Or you can leave it blank to get everything."),
    '#default_value' => variable_get('ixi_extra_extensions'),
    '#required' => FALSE,
  );
  $form['ixi_garbage_collect'] = array(
    '#type' => 'checkbox',
    '#title' => t('Garbage Collect'),
    '#description' => t("If checked we'll delete old uploaded archives and clean old tracking records from the database."),
    '#default_value' => variable_get('ixi_garbage_collect'),
  );

  return system_settings_form($form);
}

/**
 * Return an array of available content types.
 *
 * The array is keyed by machine name and the values are the human readable
 * names.
 */
function _ixi_get_content_types() {
  $content_types = array();
  foreach (node_type_get_types() as $key => $content_type) {
    $content_types[$key] = $content_type->name;
  }
  return $content_types;
}

/**
 * Return the associated fields of a given content type.
 */
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

/**
 * Ajax callback for the group node fields configuration.
 */
function _ixi_ajax_group_fields($form, &$form_state) {
  return $form['wrapper'];
}

/**
 * Form constructor for manually recreating the archive directory.
 */
function _ixi_recreate_archive_directory_form() {
  return confirm_form(
    array(),
    t('Do you want to attempt to recreate the archives upload directory?'),
    'admin/reports/status',
    t('This will attempt to recreate and make writable the upload archives directory: <em>%directory</em>.', array('%directory' => 'private://ixi_archives')), t('Recreate archive directory'),
    t('Cancel')
  );
}

/**
 * Form submission handler for manually recreating the archive directory.
 */
function _ixi_recreate_archive_directory_form_submit($form, &$form_state) {
  _ixi_recreate_archive_directory();
  $form_state['redirect'] = 'admin/reports/status';
}

/**
 * Attempt to manually recreate the archive directory.
 */
function _ixi_recreate_archive_directory() {
  $directory = 'private://ixi_archives';
  if (!file_prepare_directory($directory, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS)) {
    drupal_set_message(t('Could not create the directory %directory.', array('%directory' => $directory)), 'error');
  }
}
