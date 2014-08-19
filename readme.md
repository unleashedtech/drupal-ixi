IXI: XML Importer
=================

Installation
------------

Upon installation the module will attempt to create the directory
private://ixi_archives so that we do not expose uploaded archives to the
public. If installation fails, check the permissions of the private files
directory.

Configuration
-------------

The 'group' configuration options specify the content type and the fields that
should be created when uploading a new archive of XML files. Typically, this
is the 'volume' content type.

Note that the fields are generated as if they were in the new content form.

For now the module just dumps all images into the public files directory.

Hooks
-----

### hook_ixi_upload_group_node_alter

You can call this hook to modify the group node before it is saved to the
database. $node is passed by reference and the $form_state is also available
in case you need other data from the form.

### hook_ixi_upload_parse_xml_file

You must implement this hook or no new nodes will be created when processing
the files. The hook takes an SimpleXML object of the XML, an array of the
images keyed by original name with paths on the server as their values and the
node id of the group node that was created during the upload. The overrides
array contains an array of field names and the overridden value. The hook
should return the id of the new node that was created or `FALSE` if there was
an error and no new node was created.

### hook_ixi_upload_preview_fields

You must implement this hook in order to show fields when clicking the Preview
link before processing XML. The hook gets passed a SimpleXML object
representing the XML and an array of overrides similar to the parse_xml_file
hook from above. The hook must return an associative two-dimensional array
with the keys being the fields. You can choose whichever name you wish, they
do not need to correspond to fields in XML or in the node. Each key
corresponds to a row in the table on the preview page. The value of each key
has three values: field, value, and override. The field is the human readable
name of the field, value is its value (You probably want to check for an
override here, if enabled), and override is a boolean denoting whether it
should be possible to overwrite the field before processing.
