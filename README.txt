IXI: XML IMPORTER
=================


CONTENTS OF THIS FILE
---------------------
 * Introduction
 * Requirements
 * Recommended modules
 * Installation
 * Configuration
 * Maintainers


INTRODUCTION
------------
The IXI: XML Importer module allows for the upload of an entire archive of XML
files, with which you can do whatever you like, by implementing several hooks.
Afterwards, all of the files uploaded can be associated with one "group node"
that is created when the archive is uploaded.
 * For a full description of the module, visit eh project page:
   https://www.drupal.org/sandbox/super_mario/2328781


REQUIREMENTS
------------
No special requirements.


RECOMMENDED MODULES
-------------------
 * Devel (https://www.drupal.org/project/devel):
   You may find this module helpful when developing hooks to learn about the
   format of the XML that you are dealing with.


INSTALLATION
------------
 * Install as you would normally install a contributed drupal module. See:
   https://drupal.org/documentation/install/modules-themes/modules-7
   for further information.


CONFIGURATION
-------------
 * Configure user permissions in Administration » People » Permissions:
   - Administer XML Import
     This permission allows a user to change all of the configuration options
     of the importer.
   - Perform XML Import
     This permission allows a user to actually upload and process an archive
     of XML files.
 * Customize the importer settings in Administration » Configuration »
   XML Import


MAINTAINERS
-----------
Current maintainer:
 * Mario Finelli (super_mario) - https://www.drupal.org/user/3003877
This project has been sponsored by:
 * UNLEASHED TECHNOLOGIES
   Specializing in open-source technologies (Drupal, Magento and Symfony2),
   Unleashed Technologies offers installation, theming, customization, and
   hosting. Visit https://www.unleashed-technologies.com and
   https://www.unleashed-hosting.com/ for more information.
