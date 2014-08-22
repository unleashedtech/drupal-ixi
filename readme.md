IXI: XML Importer
=================

Installation
------------

Upon installation the module will attempt to create the directory
private://ixi_archives so that we do not expose uploaded archives to the
public. If installation fails, check the permissions of the private files
directory.

It is also required to have the zip extension and domdocument class available.

Configuration
-------------

The 'group' configuration options specify the content type and the fields that
should be created when uploading a new archive of XML files. Typically, this
is the 'volume' content type. Note that the fields are generated as if they
were in the new content form.

You can specify which images are automatically extracted and passed to the
parse hook (files are dumped into the public files directory), as well as the
extensions of files that should be passed to the hook for processing inside
hook. If you would prefer that no files are automatically extracted you can
combine these two options: set no image extensions and then either include
them in the extra extensions or leave the extra extensions blank. This will
result in all images being passed through the files array. See the
`hook_ixi_upload_parse_xml_file` hook for more information.

Hooks
-----

### hook_ixi_upload_group_node_alter

You can call this hook to modify the group node before it is saved to the
database. $node is passed by reference and the $form_state is also available
in case you need other data from the form.

### hook_ixi_upload_xml_alter
You can call this optional hook if you would like to modify the XML that is
stored to the database when an archive is uploaded. There are various reasons
you might wish to do this, such as substituting some tags for others or to do
some kind of pre-processing of the XML. Please note that this hook receives
the raw XML string, not an object: you can convert it to an object if you need
to, but you must convert it back to a string when you are finished.

### hook_ixi_upload_parse_xml_file

You must implement this hook or no new nodes will be created when processing
the files. The hook takes an SimpleXML object of the XML, an array of the
images keyed by original name with paths on the server as their values, an
array of additional files in the same folder as the XML file (these can be
retrieved by using `ixi_extract_to` and `ixi_extract_data` and an array of
useful ids: the id of the queue, the upload id and the node id of the group
node that was created during the upload. The overrides array contains an array
of field names and the overridden value. The hook should return the id of the
new node that was created or `FALSE` if there was an error and no new node was
created.

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

Functions
---------

### ixi_extract_to

You can call this function with an xid and a filename from the associated
archive. The default destination is the root of the public files directory,
however the third parameter allows you to change this to whatever you'd like,
such as `public://my_files` or `private://`. The fourth parameter, also
optional determines whether to preserve the folder structure. If true, then
files will be extracted in the same directories (appended to the destination)
that they appear in in the archive. If false, then files will be uploaded
directly to the root of the destination directory. This function returns the
full path of the extracted file (including if if it was e.g., renamed).

### ixi_extract_data

You can call this function if you would like the raw data stream of a file
from the archive. You just need to pass the xid, and the filename from the
archive (this can be retrieved from the files array inside the parse file
hook).
