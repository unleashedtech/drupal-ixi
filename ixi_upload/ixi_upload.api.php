<?php

/**
 * @file
 * Hooks prvided by InDesign XML Importer Uploader
 */

/**
 * Alter the group node before saving it.
 *
 * @param $node
 *   The new group node.
 *
 * @param $form_state
 *   The submitted form so we can modify based on other data that was part of
 *   the form.
 */
function hook_ixi_upload_group_node_alter(&$node, $form_state) {
}

/**
 * Create new node based on xml
 *
 * @param $xml
 *   A SimpleXml object representing the xml.
 *
 * @param $images
 *   An array keyed by original filename in the archive with values of the file
 *   in the drupal filesystem.
 *
 * @param $group_nid
 *   The node id of the group node attached to the upload.
 */
function hook_ixi_upload_parse_xml_file($xml, $images, $group_nid) {
}
