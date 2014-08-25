# IXI: XML Importer

## Installation

It is always necessary to include and install the main IXI and the IXI Uploader modules. The main module contains all of the configuration options and menus, and the uploader module handles uploading and extracting the archive. Also included are two reference implementation modules: the main implementation module which contains all of the code that needs to be implemented for a custom solution, and the feature module which simply creates content types that work with supplied sample files and the reference implementation code and does not need to be implemented.

Upon installation the module will attempt to create the directory `private://ixi_archives` so that we do not expose uploaded archives to the public. If installation fails, check the permissions of the private files directory.

It is also required to have the zip extension and domdocument class available.

## Configuration

The 'group' configuration options specify the content type and the fields that should be created when uploading a new archive of XML files. This node is passed to the parse functions so that everything can be associated with the group node. Note that the fields are generated as if they were in the new content form.

You can specify which images are automatically extracted and passed to the parse hook (files are dumped into the public files directory), as well as the extensions of files that should be passed to the hook for processing inside hook. If you would prefer that no files are automatically extracted you can combine these two options: set no image extensions and then either include them in the extra extensions or leave the extra extensions blank. This will result in all images being passed through the files array. See the `hook_ixi_upload_parse_xml_file` hook for more information. Note that you could also configure these options to automatically extract *some* images and pass others through the files array.

## Hooks

### `hook_ixi_upload_group_node_alter` *optional*

You can call this hook to modify the group node before it is saved to the database. `$node` is passed by reference and the `$form_state` is also available in case you need other data from the form.

#### Parameters

* `$node`: The node object of the new node that we are creating along with the upload.
* `$form_state`: The form state of the submitted form so that you can access other values if needed.

### `hook_ixi_upload_xml_alter` *optional*

You can call this optional hook if you would like to modify the XML that is stored to the database when an archive is uploaded. There are various reasons you might wish to do this, such as substituting some tags for others or to do some kind of pre-processing of the XML. Please note that this hook receives the raw XML string, not an object: you can convert it to an object if you need to, but you must convert it back to a string when you are finished.

#### Parameters

* `$xml`: The raw string of the xml.

### `hook_ixi_upload_parse_xml_file` *required*

You must implement this hook or no new nodes will be created when processing the files. The hook takes an SimpleXML object of the XML, an array of the images keyed by original name with paths on the server as their values, an array of additional files in the same folder as the XML file (these can be retrieved by using `ixi_extract_to` and `ixi_extract_data` and an array of useful ids: the id of the queue, the upload id and the node id of the group node that was created during the upload. The overrides array contains an array of field names and the overridden value. The hook should return the id of the new node that was created or `FALSE` if there was an error and no new node was created.

#### Parameters

* `$xml`: a SimpleXML object of the XML.
* `$images`: An associative array with keys equal to the original filenames of images in the archive and values equal to the paths on the server (in the case of files being renamed due to conflicts).
* `$files`: An array of other files in the same folder as the XML file. You can use the `ixi_extract_*` functions (see below).
* `$ids`: This is an array of ids that you may find useful: the id of the file in the queue table, the xid of the archive in the upload table and the nid of the new group node that was created. Please note that the `ixi_extract_*` functions require the xid to be passed as the first argument.
* `$overrides`: An array of fields (as defined by the developer when implementing their module) that have been overridden. Before saving fields to the node from the XML you can check to see if they have been overridden in this array. For example: `$somevar = (array_key_exists('some_field', $overrides)) ? $overrides['some_field'] : $xml->sometag;`.

#### Return

The function's return value is not very important unless there was an error. The return value is not used later, however one option is to return the value of e.g., the nid of a new node that was created. If there was an error then you should return `FALSE`.

### `hook_ixi_upload_preview_fields` *required*

You must implement this hook in order to show fields when clicking the Preview link before processing XML. The hook gets passed a SimpleXML object representing the XML and an array of overrides similar to the parse_xml_file hook from above. The hook must return an associative two-dimensional array with the keys being the fields. You can choose whichever name you wish, they do not need to correspond to fields in XML or in the node. Each key corresponds to a row in the table on the preview page. The value of each key has three values: field, value, and override. The field is the human readable name of the field, value is its value (You probably want to check for an override here, if enabled), and override is a boolean denoting whether it should be possible to overwrite the field before processing.

#### Parameters

* `$xml`: a SimpleXML object of the XML.
* `$overrides`: and array of the overridden fields (same from `hook_ixi_upload_parse_xml_file`).

#### Return

This function needs to return an array of rows in the preview fields table. They are keyed by the field that you would like to create which *does not* need to correspond to a Drupal field, xml tag or anything else (think creating a date field from two different xml tags, for exaple). Each value is an array with three items: a 'field' which is the human readable name of the field, a 'value' which is the value (you'll want to check the overrides to see if it's been overridden if enabled), and 'override' which is a boolean whether the field can be overridden or not. Example:

```
array(
  'title' => array(
    'field' => t('Title'),
    'value' => (array_key_exists('title', $overrides)) ? $overrides['title'] : $xml->title,
    'override' => TRUE,
  ),
  'subtitle' => array(
    'field' => t('Subtitle'),
    'value' => $xml->subtitle,
    'override' => FALSE,
  ),
);
```

## Functions

### `ixi_extract_to`

You can call this function with an xid and a filename from the associated archive. The default destination is the root of the public files directory, however the third parameter allows you to change this to whatever you'd like, such as `public://my_files` or `private://`. The fourth parameter, also optional determines whether to preserve the folder structure. If true, then files will be extracted in the same directories (appended to the destination) that they appear in in the archive. If false, then files will be uploaded directly to the root of the destination directory. This function returns the full path of the extracted file (including if if it was e.g., renamed).

#### Parameters

* `$xid`: The xid of the archive you want to extract from.
* `$filename`: The path and filename of the file you'd like to extract from the archive. Note that this is what is in the `$files` array in `hook_ixi_upload_parse_xml_file`.
* `$destination`: Where to extract the files. Defaults to `public://`, but could also be `private://` or a directory inside either the public or private folders.
* `$preserve_folder_structure`: A boolean (defaulting to false) whether to append the archive's internal file structure to the end of `$destination` when extracting the file.

#### Return

The function returns the path of the extracted file on the server (in case it was e.g., renamed to avoid conflicting with a file of the same name).

### `ixi_extract_data`

You can call this function if you would like the raw data stream of a file
from the archive. You just need to pass the xid, and the filename from the
archive (this can be retrieved from the files array inside the parse file
hook).

#### Parameters

* `$xid`: The xid of the archive you want to extract from.
* `$filename`: The path and filename of the file you'd like to extract from the archive. Note that this is what is in the `$files` array in `hook_ixi_upload_parse_xml_file`.

#### Return

The function returns a raw data stream of the extracted file. You can modify it, save it to the database as a binary blob, or save it to the filesystem yourself.
