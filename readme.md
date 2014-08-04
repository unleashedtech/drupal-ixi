Drupal InDesign XML Importer
============================

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

Hooks
-----

### hook_ixi_upload_group_node_alter

You can call this hook to modify the group node before it is saved to the
database. $node is passed by reference and the $form_state is also available
in case you need other data from the form.
