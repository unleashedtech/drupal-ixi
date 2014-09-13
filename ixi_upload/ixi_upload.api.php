<?php

/**
 * @file
 * Hooks prvided by IXI: XML Importer Uploader
 */

/**
 * Alter the group node before saving it.
 *
 * @param object $node
 *   This is the new node object that we will be saving.
 *
 * @param array $form_state
 *   This is the submitted form_state so that you can access other fields if
 *   needed.
 */
function hook_ixi_upload_group_node_alter(&$node, $form_state) {
}

/**
 * Alter the raw XML string before it's saved to the database.
 *
 * @param string $xml
 *   This is the raw xml string, not an object. You can convert it if you
 *   need to, but it must be converted back before the function returns.
 */
function hook_ixi_upload_xml_alter(&$xml) {
}

/**
 * Process an XML file.
 *
 * You can do whatever you need to here: create a new node or taxonomy, for
 * example.
 *
 * @param object $xml
 *   A SimpleXml object representing the xml.
 *
 * @param array $images
 *   An array keyed by original filename in the archive with values of the file
 *   in the drupal filesystem.
 *
 * @param array $files
 *   An array of additonal files in the same folder as the XML file. These can
 *   be retrived by calling ixi_extract_to to extract the file to the public
 *   folder or ixi_extract_data to get a raw stream of the uncompressed file.
 *
 * @param array $ids
 *   An array with useful ids: 'id' is the xml queue id, 'xid' is the upload id
 *   and 'group_nid' is the node id of the group node that was created when
 *   the archive was uploaded.
 *
 * @param array $overrides
 *   An array of fields as defined by the developer as keys with overridden
 *   values (only if they have actually been overridden).
 *
 * @return mixed
 *   The return value isn't important (unless there was an error), as it isn't
 *   used for anything. One option would be to return the nid of the new node
 *   that was created. If there was an error you should return false.
 */
function hook_ixi_upload_parse_xml_file($xml, $images, $files, $ids, $overrides) {
}

/**
 * Return rows of a preview fields table with the field name and its value.
 *
 * @param object $xml
 *   A SimpleXml object representing the the xml.
 *
 * @param array $overrides
 *   An array of fields as defined by the developer as keys with overridden
 *   values (only if they have actually been overridden).
 *
 * @return array
 *   You should return a two dimensional array keyed by fields and values as
 *   arrays of options. Each field is given a row in the preview fields table,
 *   and the options array needs to have the following values: 'title' which
 *   is the human readable field name such as 'Title', 'value' which is the
 *   current value of the field (you should check the overrides array if
 *   overrides are enabled for the field), and 'override' which is a boolean
 *   denoting whether the field can be overridden.
 */
function hook_ixi_upload_preview_fields($xml, $overrides) {
}
