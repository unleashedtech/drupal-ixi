<?php

/**
 * @file
 * IXI: XML Importer Uploader file
 *
 * Administration page callbacks for XML Importer Uploader.
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
  $form['group_node']['group_content_type'] = array(
    '#type' => 'hidden',
    '#value' => $node_names[variable_get('ixi_group_content_type')],
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
  $file->status = FILE_STATUS_PERMANENT;
  file_save($file);

  if (!$file) {
   drupal_set_message('File was not uploaded.', 'error');
  }

  $operations[] = array(
    '_ixi_upload_create_group_node',
    array($form_state, $file->fid),
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

  $form_state['redirect'] = 'admin/content/add/xml/process';

}

function _ixi_upload_create_group_node($form_state, $fid, &$context) {
  global $user;
  $node = new stdClass();
  $node->type = variable_get('ixi_group_content_type');
  node_object_prepare($node);

  $node->language = LANGUAGE_NONE;
  $node->uid = $user->uid;

  $node->status = 0;
  $node->promote = 0;
  $node->sticky = 0;
  $node->comment = 1;

  if (module_exists('pathauto')){
    $node->path['pathauto'] = true;
  }

  $node->title = $form_state['values']['title'];

  foreach (variable_get('ixi_group_fields') as $field_name) {
    $node->$field_name = $form_state['values'][$field_name];
  }

  // call new hook to allow altering of node before it's saved
  drupal_alter('ixi_upload_group_node', $node, $form_state);

  if ($node = node_submit($node)) {
    node_save($node);

    $xid = db_insert('ixi_archive_uploads')
      ->fields(array(
        'fid' => $fid,
        'nid' => $node->nid,
      ))
      ->execute();
  }

  // store the new xid to the context array so we can reference it later
  $context['results'][] = $xid;

  $context['results'][] = t('New @content_type id: @nid.', array(
    '@content_type' => $form_state['values']['group_content_type'],
    '@nid' => $node->nid,
  ));
  $context['message'] = t('Creating the group node.');
}

function _ixi_upload_unzip_xml_file($fid, $basename, $dirname, &$context) {
  $file = file_load($fid);
  $zip = new ZipArchive();

  $images = array();
  if(($zip_return = $zip->open(drupal_realpath($file->uri))) === TRUE) {
    for ($i = 0; $i < $zip->numFiles; $i++) {
      $zipped_file = $zip->statIndex($i);
      if (strlen($zipped_file['name']) < 8 || strcmp(substr($zipped_file['name'], 0, 8), '__MACOSX')) {
        $pathinfo = pathinfo($zipped_file['name']);
        if (!strcmp($basename, $pathinfo['basename'])) {
          if (($fp = $zip->getStream($zipped_file['name'])) !== FALSE) {
            $xml_raw = '';
            while (!feof($fp)) {
              $xml_raw .= fread($fp, 2);
            }
            fclose($fp);
            $xml_checksum = md5($xml_raw);
            $xml = new DomDocument();
            $xml->recover = TRUE;
            $xml->loadXML($xml_raw);
            $xml->formatOutput = TRUE;
            $xml_new = $xml->saveXML();
            $xml_new_checksum = md5($xml_new);
            $old_path = 'temporary://ixi_xml_old.xml';
            $new_path = 'temporary://ixi_xml_new.xml';
            file_unmanaged_save_data($xml_raw, $old_path, FILE_EXISTS_REPLACE);
            file_unmanaged_save_data($xml_new, $new_path, FILE_EXISTS_REPLACE);
            $diff_array;
            exec('diff ' . escapeshellarg(drupal_realpath($old_path)) . ' ' . escapeshellarg(drupal_realpath($new_path)), $diff_array);
            $xml_diff = implode("\n", $diff_array);
            file_unmanaged_delete($old_path);
            file_unmanaged_delete($new_path);
            libxml_use_internal_errors(TRUE);
            if (simplexml_load_string($xml_new) === FALSE) {
              $xml_status = 0;
            }
            else {
              $xml_status = 1;
            }
          }
          else {
            drupal_set_message(t('Could not read the contents of the XML file.'), 'error');
          }
        }
        else {
          $parts = explode('/', $dirname);
          $folder = $parts[0];
          $parts = explode('/', $pathinfo['dirname']);
          if (!strcmp($parts[0], $folder) // must be in the same folder as the XML
              && count($parts) == 2 // must have folder/images
              && !strcmp($parts[1], 'images') // must have second part of path be "images"
              && isset($pathinfo['extension']) // must have a file extension
              && strcmp($pathinfo['basename'], '.DS_Store') // exclude Mac-specific file .DS_Store
          ) {
            $images[] = array($i => $zipped_file['name']);
          }
        }
      }
    }
  }
  else {
    drupal_set_message(t('Could not open the zip file. (zip error code: %code)', array('%code' => $zip_return)), 'error');
  }

  $id = db_insert('ixi_xml_queue')
    ->fields(array(
      'xid' => $context['results'][0],
      'xml_path' => $dirname,
      'xml_filename' => $basename,
      'xml' => $xml_new,
      'xml_diff' => $xml_diff,
      'checksum_old' => $xml_checksum,
      'checksum_new' => $xml_new_checksum,
      'images' => serialize($images),
      'status' => $xml_status,
    ))
    ->execute();

  $context['results'][] = $id;
  $context['message'] = t('Checking XML file %xml_filename.', array('%xml_filename' => $basename));
}

function _ixi_upload_upload_finished($success, $results, $operations) {
  if ($success) {
    drupal_set_message(t('@count files processed.', array('@count' => count($results) - 2)));
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
  $sql = "SELECT ixq.id, ixq.xid, ixq.status, iau.nid FROM {ixi_xml_queue} ixq JOIN {ixi_archive_uploads} iau WHERE ixq.xid = iau.xid AND ixq.processed = '0' ORDER BY iau.xid DESC, ixq.id ASC";
  $result = db_query($sql);
  $options = array();
  $exclude = array();
  if ($result) {
    while ($row = $result->fetchAssoc()) {
      $node = node_load($row['nid']);
      $options[$row['id']] = array(
        'xid' => $row['xid'],
        'nid' => l($node->title, 'node/' . $node->nid),
        'diff' => l(t('View XML diff'), 'admin/content/add/xml/process/diff/' . $row['id']),
        'xml' => l(t('Edit XML'), 'admin/content/add/xml/process/edit/' . $row['id']),
        'preview' => l(t('Preview'), 'admin/content/add/xml/process/preview/' . $row['id']),
        'status' => ($row['status']) ? t('Ready') : t('Bad XML'),
      );
    }
    $header = array(
      'xid' => t('Xid'),
      'nid' => t('Group Node'),
      'diff' => t('XML diff'),
      'xml' => t('Edit XML'),
      'preview' => t('Preview Fields'),
      'status' => t('Status'),
    );
    $form['process'] = array(
      '#type' => 'tableselect',
      '#header' => $header,
      '#options' => $options,
      '#empty' => t('No files to process.'),
    );
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Process selected files'),
    );
  }

  return $form;
}

function ixi_upload_process_form_submit($form, &$form_state) {
  $operations = array();
  foreach ($form_state['values']['process'] as $queue_id) {
    if ($queue_id != 0) {
      $operations[] = array(
        '_ixi_upload_process_queue',
        array($queue_id),
      );
    }
  }
  if (count($operations)) {
    $operations[] = array('_ixi_upload_cleanup_queue', array());
    $batch = array(
      'title' => t('Processing XML Queue...'),
      'operations' => $operations,
      'finished' => '_ixi_upload_queue_finished',
      'file' => drupal_get_path('module', 'ixi_upload') . '/ixi_upload.admin.inc',
    );
    batch_set($batch);
  }
  $form_state['redirect'] = 'admin/content/add/xml/process';
}

function _ixi_upload_view_diff($id) {
  $sql = "SELECT ixq.xml_diff FROM {ixi_xml_queue} ixq WHERE id = :id LIMIT 1";
  $result = db_query($sql, array(':id' => $id));
  $build['breadcrumb'] = array(
    '#markup' => '<p>' . t('Return to <a href="@link">XML Process</a>.', array('@link' => url('admin/content/add/xml/process'))) . '</p>',
  );
  if ($result) {
    $row = $result->fetchAssoc();
    $build['diff'] = array(
      '#markup' => '<pre>' . check_plain($row['xml_diff']) . '</pre>',
    );
  }
  drupal_set_title(t('View XML diff'));
  return $build;
}

function _ixi_upload_edit_xml_form($form, &$form_state, $id) {
  $sql = "SELECT ixq.xml FROM {ixi_xml_queue} ixq WHERE id = :id LIMIT 1";
  $result = db_query($sql, array(':id' => $id));
  if ($result) {
    $row = $result->fetchAssoc();
    $form['xml'] = array(
      '#type' => 'textarea',
      '#default_value' => $row['xml'],
    );
    $form['id'] = array(
      '#type' => 'hidden',
      '#value' => $id,
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Update XML',
  );
  return $form;
}

function _ixi_upload_edit_xml_form_submit($form, &$form_state) {
  libxml_use_internal_errors(TRUE);
  if (simplexml_load_string($form_state['values']['xml']) === FALSE) {
    $xml_status = 0;
  }
  else {
    $xml_status = 1;
  }
  db_update('ixi_xml_queue')
    ->fields(array(
      'xml' => $form_state['values']['xml'],
      'status' => $xml_status,
    ))
    ->condition('id', $form_state['values']['id'], '=')
    ->execute();

  $form_state['redirect'] = 'admin/content/add/xml/process';
}

function _ixi_upload_process_queue($id, &$context) {
  $sql = "SELECT ixq.id, ixq.xid, ixq.xml, ixq.status, ixq.images, iau.nid, iau.fid FROM {ixi_xml_queue} ixq JOIN {ixi_archive_uploads} iau WHERE ixq.xid = iau.xid AND ixq.id = :id LIMIT 1";
  $result = db_query($sql, array(':id' => $id));
  if ($result) {
    $row = $result->fetchAssoc();
    if ($row['status']) {
      $file = file_load($row['fid']);
      $zip = new ZipArchive();
      if (($zip_return = $zip->open(drupal_realpath($file->uri))) === TRUE) {
        $images = unserialize($row['images']);
        $saved_images = array();
        foreach ($images as $index => $image) {
          $image = array_pop($image);
          if (($fp = $zip->getStream($image)) !== FALSE) {
            $raw_data = '';
            while (!feof($fp)) {
              $raw_data .= fread($fp, 2);
            }
            fclose($fp);
            $saved_images[$image] = file_unmanaged_save_data($raw_data, 'public://' . basename($image), FILE_EXISTS_RENAME);
          }
        }
        $xml = simplexml_load_string($row['xml']);
        $context['results'][] = module_invoke_all('ixi_upload_parse_xml_file', $xml, $saved_images, $row['nid']);
        db_update('ixi_xml_queue')
          ->fields(array(
            'processed' => 1,
          ))
          ->condition('id', $row['id'])
          ->execute();
      }
      else {
        drupal_set_message(t('Could not open zip (zip error code: @code)', array('@code' => $zip_return)), 'error');
      }
    }
  }
  $context['message'] = t('Processing xml queue with id: @id.', array('@id' => $id));
}

function _ixi_upload_queue_finished($success, $results, $operations) {
  if ($success) {
    drupal_set_message(t('@count files processed.', array('@count' => count($results))));
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

function _ixi_upload_preview_fields($id) {
  $sql = "SELECT ixq.id, ixq.xid, ixq.xml, iau.nid FROM {ixi_xml_queue} ixq JOIN {ixi_archive_uploads} iau WHERE ixq.xid = iau.xid AND ixq.id = :id LIMIT 1";
  $result = db_query($sql, array(':id' => $id));
  if ($result) {
    $row = $result->fetchAssoc();
    $xml = simplexml_load_string($row['xml']);
    $header = array(
      t('Field'),
      t('Value'),
    );
    $rows = module_invoke_all('ixi_upload_preview_fields', $xml);
    $table = theme_table(array(
      'header' => $header,
      'rows' => $rows,
      'attributes' => array(),
      'caption' => NULL,
      'colgroups' => array(),
      'sticky' => TRUE,
      'empty' => t('No fields')
    ));
    return array('#markup' => $table);
  }
}

function _ixi_upload_cleanup_queue(&$context) {
  $sql = "SELECT iau.xid FROM {ixi_archive_uploads} iau";
  $result = db_query($sql);
  $cleaned = 0;
  if ($result) {
    foreach ($result as $row) {
      $r = db_query("SELECT ixq.id FROM {ixi_xml_queue} ixq WHERE ixq.xid = :xid AND ixq.processed = '0'", array(':xid' => $row->xid));
      if ($r->rowCount() == 0) {
        db_update('ixi_archive_uploads')
          ->fields(array(
            'processed' => 1,
          ))
          ->condition('xid', $row->xid)
          ->execute();
        $cleaned++;
      }
    }
  }
  $context['results'][] = $cleaned;
  $context['message'] = t('Cleaning up...');
}

function _ixi_upload_preview_override_form($form, &$form_state, $oid) {
  $sql = "SELECT io.value FROM {ixi_overrides} io WHERE io.oid = :oid LIMIT 1";
  $result = db_query($sql, array(':oid' => $oid));
  $form = array();
  if ($result) {
    $row = $result->fetchAssoc();
    $form['value'] = array(
      '#type' => 'textarea',
      '#default_value' => $row['value'],
    );
    $form['oid'] = array(
      '#type' => 'hidden',
      '#value' => $oid,
    );
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => 'Ovverride',
    );
  }
  return $form;
}

function _ixi_upload_preview_override_form_submit($form, &$form_state) {
  $sql = "SELECT io.id, io.value FROM {ixi_overrides} io WHERE io.oid = :oid LIMIT 1";
  $result = db_query($sql, array(':oid' => $form_state['values']['oid']));
  $row = $result->fetchAssoc();
  if (strcmp($row['value'], $form_state['values']['value'])) {
    db_update('ixi_overrides')
      ->fields(array(
        'value' => $form_state['values']['value'],
        'changed' => 1,
      ))
      ->condition('oid', $form_state['values']['oid'], '=')
      ->execute();
  }
  $form_state['redirect'] = 'admin/content/add/xml/process/preview/' . $row['id'];
}
