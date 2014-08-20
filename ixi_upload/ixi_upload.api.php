<?php

/**
 * @file
 * Hooks prvided by IXI: XML Importer Uploader
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
 * @param $files
 *   An array of additonal files in the same folder as the XML file. These can
 *   be retrived by calling ixi_extract_to to extract the file to the public
 *   folder or ixi_extract_data to get a raw stream of the uncompressed file.
 *
 * @param $group_nid
 *   The node id of the group node attached to the upload.
 *
 * @param $overrides
 *   An array of fields as defined by the developer as keys with overridden
 *   values (only if they have actually been overridden).
 */
function hook_ixi_upload_parse_xml_file($xml, $images, $files, $group_nid, $overrides) {
}

/**
 * Return rows of a preview fields table with the field name and its value
 *
 * @param $xml
 *   A SimpleXml object representing the the xml.
 */
function hook_ixi_upload_preview_fields($xml) {
}
