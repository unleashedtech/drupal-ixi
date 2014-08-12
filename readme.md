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
node id of the group node that was created during the upload. The hook should
return the id of the new node that was created or `FALSE` if there was an
error and no new node was created.
