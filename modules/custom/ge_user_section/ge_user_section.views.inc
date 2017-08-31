<?php


/**
 * @file
 * Implementation of hook_views_data_alter()
 */

function ge_user_section_views_data() {

  $data = array();
  $data['users']['current_user_access_section'] = array(
    'title' => t('Current State access field'),
    'help' => t('Filter taxonomy by access level.'),
	'field' => array(
      'handler' => 'ge_user_section_handler_field_section_info',
      'click sortable' => FALSE,
    ),
  );
  
  return $data;
}