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
  $form['group_node']['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#maxlength' => 255,
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

  $operations[] = array(
    '_ixi_upload_create_group_node',
    array($form_state),
  );

  $found_files = array();

  $zip = new ZipArchive();
  if (($zip_return = $zip->open(drupal_realpath($file->uri))) === TRUE) {
    for ($i = 0; $i < $zip->numFiles; $i++) {
      $zip_file = $zip->statIndex($i);
      if (strlen($zip_file['name']) < 8 || strcmp(substr($zip_file['name'], 0, 8), '__MACOSX')) {
        $pathinfo = pathinfo($zip_file['name']);
        if (isset($pathinfo['extension']) && !strcmp($pathinfo['extension'], 'xml')) {
          $operations[] = array(
            '_ixi_upload_unzip_xml_file',
            array($file->fid, $pathinfo['basename'], $pathinfo['dirname']),
          );
        }
      }
    }
    $zip->close();
  }
  else {
    drupal_set_message('Could not open zip (zip error code: ' . $zip_return . ')', 'error');
  }

  $batch = array(
    'title' => t('Unzipping archive...'),
    'operations' => $operations,
    'finished' => '_ixi_upload_upload_finished',
    'file' => drupal_get_path('module', 'ixi_upload') . '/ixi_upload.admin.inc',
  );

  batch_set($batch);

}

function _ixi_upload_create_group_node($form_state, &$context) {
  $context['results'][] = 'the new group node id';
  $context['message'] = t('Creating the group node.');
  sleep(2);
}

function _ixi_upload_unzip_xml_file($fid, $basename, $dirname, &$context) {
  $context['results'][] = $fid;
  $context['message'] = t('Checking XML file %xml_filename.', array('%xml_filename' => $basename));
  sleep(1);
}

function _ixi_upload_upload_finished($success, $results, $operations) {
  if ($success) {
    drupal_set_message(t('@count results processed.', array('@count' => count($results))));
    drupal_set_message(t('The final result was "%final"', array('%final' => end($results))));
  }
  else {
    $error_operation = reset($operations);
    drupal_set_message(
      t('An error occurred while processing @operation with arguments: @args',
        array(
          '@operation' => $error_operation[0],
          '@args' => print_r($error_operation[0], TRUE),
        )
      )
    );
  }
}

/**
 * Form constructor for processing a queue of XML files.
 */
function ixi_upload_process_form($form, &$form_state) {
  $form = array();
  return $form;
}
