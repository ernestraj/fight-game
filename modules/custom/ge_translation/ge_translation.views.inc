<?php


/**
 * @file
 * Implementation of hook_views_data()
 */

function ge_translation_views_data() {
  $data = array();
 
 $data['workbench_moderation_node_history']['translation_actions'] = array(
    'title' => t('Translation Actions'),
    'help' => t('Content translation state of the node revisision.'),
    'real field' => 'state',
    'field' => array(
      'title' => t('Translation Links-custom'),
      'handler' => 'ge_translation_handler_field_links',
      'click sortable' => FALSE,
      'additional fields' => array('nid', 'vid', 'current'),
    ),
  
  );
  
  return $data;
}

