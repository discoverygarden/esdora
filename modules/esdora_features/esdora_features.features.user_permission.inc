<?php

/**
 * Implementation of hook_user_default_permissions().
 */
function esdora_features_user_default_permissions() {
  $permissions = array();

  // Exported permission: Admin Form Associations
  $permissions['Admin Form Associations'] = array(
    'name' => 'Admin Form Associations',
    'roles' => array(),
  );

  // Exported permission: Create XML Forms
  $permissions['Create XML Forms'] = array(
    'name' => 'Create XML Forms',
    'roles' => array(),
  );

  // Exported permission: Delete XML Forms
  $permissions['Delete XML Forms'] = array(
    'name' => 'Delete XML Forms',
    'roles' => array(),
  );

  // Exported permission: Edit XML Forms
  $permissions['Edit XML Forms'] = array(
    'name' => 'Edit XML Forms',
    'roles' => array(),
  );

  // Exported permission: List XML Forms
  $permissions['List XML Forms'] = array(
    'name' => 'List XML Forms',
    'roles' => array(),
  );

  // Exported permission: access administration menu
  $permissions['access administration menu'] = array(
    'name' => 'access administration menu',
    'roles' => array(),
  );

  // Exported permission: access administration pages
  $permissions['access administration pages'] = array(
    'name' => 'access administration pages',
    'roles' => array(),
  );

  // Exported permission: access content
  $permissions['access content'] = array(
    'name' => 'access content',
    'roles' => array(
      '0' => 'anonymous user',
      '1' => 'authenticated user',
    ),
  );

  // Exported permission: access devel information
  $permissions['access devel information'] = array(
    'name' => 'access devel information',
    'roles' => array(),
  );

  // Exported permission: access site reports
  $permissions['access site reports'] = array(
    'name' => 'access site reports',
    'roles' => array(),
  );

  // Exported permission: access user profiles
  $permissions['access user profiles'] = array(
    'name' => 'access user profiles',
    'roles' => array(),
  );

  // Exported permission: add fedora datastreams
  $permissions['add fedora datastreams'] = array(
    'name' => 'add fedora datastreams',
    'roles' => array(),
  );

  // Exported permission: administer actions
  $permissions['administer actions'] = array(
    'name' => 'administer actions',
    'roles' => array(),
  );

  // Exported permission: administer blocks
  $permissions['administer blocks'] = array(
    'name' => 'administer blocks',
    'roles' => array(),
  );

  // Exported permission: administer content types
  $permissions['administer content types'] = array(
    'name' => 'administer content types',
    'roles' => array(),
  );

  // Exported permission: administer fancybox
  $permissions['administer fancybox'] = array(
    'name' => 'administer fancybox',
    'roles' => array(),
  );

  // Exported permission: administer features
  $permissions['administer features'] = array(
    'name' => 'administer features',
    'roles' => array(),
  );

  // Exported permission: administer files
  $permissions['administer files'] = array(
    'name' => 'administer files',
    'roles' => array(),
  );

  // Exported permission: administer filters
  $permissions['administer filters'] = array(
    'name' => 'administer filters',
    'roles' => array(),
  );

  // Exported permission: administer imageapi
  $permissions['administer imageapi'] = array(
    'name' => 'administer imageapi',
    'roles' => array(),
  );

  // Exported permission: administer islandora autocomplete
  $permissions['administer islandora autocomplete'] = array(
    'name' => 'administer islandora autocomplete',
    'roles' => array(),
  );

  // Exported permission: administer menu
  $permissions['administer menu'] = array(
    'name' => 'administer menu',
    'roles' => array(),
  );

  // Exported permission: administer nodes
  $permissions['administer nodes'] = array(
    'name' => 'administer nodes',
    'roles' => array(),
  );

  // Exported permission: administer permissions
  $permissions['administer permissions'] = array(
    'name' => 'administer permissions',
    'roles' => array(),
  );

  // Exported permission: administer site configuration
  $permissions['administer site configuration'] = array(
    'name' => 'administer site configuration',
    'roles' => array(),
  );

  // Exported permission: administer users
  $permissions['administer users'] = array(
    'name' => 'administer users',
    'roles' => array(),
  );

  // Exported permission: change own username
  $permissions['change own username'] = array(
    'name' => 'change own username',
    'roles' => array(),
  );

  // Exported permission: create batch process
  $permissions['create batch process'] = array(
    'name' => 'create batch process',
    'roles' => array(),
  );

  // Exported permission: create metadata_node
  $permissions['create metadata_node'] = array(
    'name' => 'create metadata_node',
    'roles' => array(
      '0' => 'anonymous user',
      '1' => 'authenticated user',
    ),
  );

  // Exported permission: create page content
  $permissions['create page content'] = array(
    'name' => 'create page content',
    'roles' => array(),
  );

  // Exported permission: create story content
  $permissions['create story content'] = array(
    'name' => 'create story content',
    'roles' => array(),
  );

  // Exported permission: delete any page content
  $permissions['delete any page content'] = array(
    'name' => 'delete any page content',
    'roles' => array(),
  );

  // Exported permission: delete any story content
  $permissions['delete any story content'] = array(
    'name' => 'delete any story content',
    'roles' => array(),
  );

  // Exported permission: delete entire collections
  $permissions['delete entire collections'] = array(
    'name' => 'delete entire collections',
    'roles' => array(),
  );

  // Exported permission: delete own page content
  $permissions['delete own page content'] = array(
    'name' => 'delete own page content',
    'roles' => array(),
  );

  // Exported permission: delete own story content
  $permissions['delete own story content'] = array(
    'name' => 'delete own story content',
    'roles' => array(),
  );

  // Exported permission: delete revisions
  $permissions['delete revisions'] = array(
    'name' => 'delete revisions',
    'roles' => array(),
  );

  // Exported permission: display drupal links
  $permissions['display drupal links'] = array(
    'name' => 'display drupal links',
    'roles' => array(),
  );

  // Exported permission: display source code
  $permissions['display source code'] = array(
    'name' => 'display source code',
    'roles' => array(),
  );

  // Exported permission: edit all metadata_node
  $permissions['edit all metadata_node'] = array(
    'name' => 'edit all metadata_node',
    'roles' => array(
      '0' => 'anonymous user',
      '1' => 'authenticated user',
    ),
  );

  // Exported permission: edit any page content
  $permissions['edit any page content'] = array(
    'name' => 'edit any page content',
    'roles' => array(),
  );

  // Exported permission: edit any story content
  $permissions['edit any story content'] = array(
    'name' => 'edit any story content',
    'roles' => array(),
  );

  // Exported permission: edit fedora meta data
  $permissions['edit fedora meta data'] = array(
    'name' => 'edit fedora meta data',
    'roles' => array(),
  );

  // Exported permission: edit own metadata_node
  $permissions['edit own metadata_node'] = array(
    'name' => 'edit own metadata_node',
    'roles' => array(
      '0' => 'anonymous user',
      '1' => 'authenticated user',
    ),
  );

  // Exported permission: edit own page content
  $permissions['edit own page content'] = array(
    'name' => 'edit own page content',
    'roles' => array(),
  );

  // Exported permission: edit own story content
  $permissions['edit own story content'] = array(
    'name' => 'edit own story content',
    'roles' => array(),
  );

  // Exported permission: edit tags datastream
  $permissions['edit tags datastream'] = array(
    'name' => 'edit tags datastream',
    'roles' => array(),
  );

  // Exported permission: execute php code
  $permissions['execute php code'] = array(
    'name' => 'execute php code',
    'roles' => array(),
  );

  // Exported permission: ingest new fedora objects
  $permissions['ingest new fedora objects'] = array(
    'name' => 'ingest new fedora objects',
    'roles' => array(),
  );

  // Exported permission: islandora_workflow_Administrator
  $permissions['islandora_workflow_Administrator'] = array(
    'name' => 'islandora_workflow_Administrator',
    'roles' => array(),
  );

  // Exported permission: islandora_workflow_Editor
  $permissions['islandora_workflow_Editor'] = array(
    'name' => 'islandora_workflow_Editor',
    'roles' => array(),
  );

  // Exported permission: islandora_workflow_Manager
  $permissions['islandora_workflow_Manager'] = array(
    'name' => 'islandora_workflow_Manager',
    'roles' => array(),
  );

  // Exported permission: islandora_workflow_Submitter
  $permissions['islandora_workflow_Submitter'] = array(
    'name' => 'islandora_workflow_Submitter',
    'roles' => array(),
  );

  // Exported permission: manage collections
  $permissions['manage collections'] = array(
    'name' => 'manage collections',
    'roles' => array(),
  );

  // Exported permission: manage features
  $permissions['manage features'] = array(
    'name' => 'manage features',
    'roles' => array(),
  );

  // Exported permission: purge objects and datastreams
  $permissions['purge objects and datastreams'] = array(
    'name' => 'purge objects and datastreams',
    'roles' => array(),
  );

  // Exported permission: revert revisions
  $permissions['revert revisions'] = array(
    'name' => 'revert revisions',
    'roles' => array(),
  );

  // Exported permission: select different theme
  $permissions['select different theme'] = array(
    'name' => 'select different theme',
    'roles' => array(),
  );

  // Exported permission: switch users
  $permissions['switch users'] = array(
    'name' => 'switch users',
    'roles' => array(),
  );

  // Exported permission: use PHP for block visibility
  $permissions['use PHP for block visibility'] = array(
    'name' => 'use PHP for block visibility',
    'roles' => array(),
  );

  // Exported permission: view detailed list of content
  $permissions['view detailed list of content'] = array(
    'name' => 'view detailed list of content',
    'roles' => array(),
  );

  // Exported permission: view fedora collection
  $permissions['view fedora collection'] = array(
    'name' => 'view fedora collection',
    'roles' => array(),
  );

  // Exported permission: view metadata_node
  $permissions['view metadata_node'] = array(
    'name' => 'view metadata_node',
    'roles' => array(
      '0' => 'anonymous user',
      '1' => 'authenticated user',
    ),
  );

  // Exported permission: view revisions
  $permissions['view revisions'] = array(
    'name' => 'view revisions',
    'roles' => array(),
  );

  return $permissions;
}
