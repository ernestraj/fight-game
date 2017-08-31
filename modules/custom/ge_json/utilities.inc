<?php

/**
 * Get section nodes for coressponding menu_tid(categories),country,business/function/location/roles,node language
 */
module_load_include('inc', 'ge_json', 'include/content');
module_load_include('inc', 'ge_json', 'include/newsarticle');

function _get_section_nodes($section_id, $country_tid, $bfl_roles, $lang, $type = 'section', $show_preview) {
  $business = $bfl_roles['business'];
  $function = $bfl_roles['function'];
  $location = $bfl_roles['location'];
  $band = $bfl_roles['band'];
  $roles = $bfl_roles['roles'];
  $global_tid = _get_global_tid();
    if (!empty($global_tid)) {
    $countries = array($global_tid, $country_tid);
  }
  if (empty($section_id) || empty($bfl_roles['roles'])) {
    return NULL;
  }
  $get_drafts = FALSE;
  $portal_preview = '';
  if (isset($_SERVER["HTTP_PORTALPREVIEW"])) {
    $portal_preview = check_plain($_SERVER["HTTP_PORTALPREVIEW"]);
    $get_drafts = TRUE;
  }

  $query = db_select('{node}', 'node');
  $query->fields('node', array('title', 'nid', 'language', 'created'));
  $query->leftJoin('{field_data_field_country}', 'country', 'node.nid = country.entity_id  AND (country.entity_type = \'node\' AND country.deleted = \'0\' AND country.bundle=\'section\')');
  $query->leftJoin('{field_data_field_side_menu}', 'menu', 'node.nid = menu.entity_id AND (menu.entity_type = \'node\' AND menu.deleted = \'0\')');
  $query->leftJoin('field_data_field_weight', 'weight_weights', 'node.nid = weight_weights.entity_id AND weight_weights.entity_type=\'node\'');
  $query->leftJoin('{field_data_field_section}', 'section', 'country.entity_id = section.entity_id');
  $query->leftJoin('{field_data_field_business}', 'field_data_field_business', 'section.field_section_value = field_data_field_business.entity_id');
  $query->leftJoin('{field_data_field_function}', 'field_data_field_function', 'section.field_section_value = field_data_field_function.entity_id');
  $query->leftJoin('{field_data_field_location}', 'field_data_field_location', 'section.field_section_value = field_data_field_location.entity_id');
  $query->leftJoin('{field_data_field_band}', 'field_data_field_band', 'section.field_section_value = field_data_field_band.entity_id');
  $query->leftJoin('field_data_field_self_service_roles', 'sr', 'sr.entity_id = section.field_section_value AND sr.entity_type = \'field_collection_item\'');
  if (!empty($lang)) {
    $query->condition('node.language', $lang);
  }
  $query->condition('country.field_country_tid', $countries, 'IN');
  $query->condition('menu.field_side_menu_tid', $section_id, '=');
  if (!empty($business)) {
    $or_business = db_or();
    $or_business->condition('field_data_field_business.field_business_tid', $business, 'IN');
    $or_business->isNull('field_data_field_business.field_business_tid');
    $query->condition($or_business);
  }
  if (!empty($function)) {
    $or_function = db_or();
    $or_function->condition('field_data_field_function.field_function_tid', $function, 'IN');
    $or_function->isNull('field_data_field_function.field_function_tid');
    $query->condition($or_function);
  }
  if (!empty($location)) {
    $or_location = db_or();
    $or_location->condition('field_data_field_location.field_location_tid', $location, 'IN');
    $or_location->isNull('field_data_field_location.field_location_tid');
    $query->condition($or_location);
  }

  if (!empty($band)) {
    $or_band = db_or();
    $or_band->condition('field_data_field_band.field_band_tid', $band, 'IN');
    $or_band->isNull('field_data_field_band.field_band_tid');
    $query->condition($or_band);
  }
  if (!empty($roles)) {
    $or_2 = db_or();
    $or_2->condition('sr.field_self_service_roles_tid', $roles, 'IN');
    $or_2->isNull('sr.field_self_service_roles_tid');
    $query->condition($or_2);
  }
  else {
    $query->isNull('sr.field_self_service_roles_tid');
  }
  if ($get_drafts === FALSE) {
    $query->condition('node.status', 1);
  }
  $query->condition('node.type', array('section'), 'IN');
  $query->orderBy('weight_weights.field_weight_value', 'ASC');

  $result = $query->execute();

  $nids = array();
  if (!empty($result)) {
    foreach ($result as $item) {
      $nids[] = $item->nid;
    }
  }
  $nids = array_unique($nids);
  if (empty($nids)) {
    return NULL;
  }
  return node_load_multiple($nids);
}

/* GET sidebar section values for individual nodes */

function _get_sidebar_values($sidebarnodes, $bfl_roles) {

  $params = array();

  $portal_preview = '';
  if (isset($_SERVER["HTTP_PORTALPREVIEW"])) {
    $portal_preview = check_plain($_SERVER["HTTP_PORTALPREVIEW"]);
  }

  /* If preview is true and portal server is present,we get all latest drafts */
  if (!empty($portal_preview)) {
    $preview_nodes = _get_preview_nodes($portal_preview, $sidebarnodes);
    $nodes = array();
    $sidebarnodes = $preview_nodes;
  }


  if (empty($sidebarnodes)) {
    return NULL;
  }
  /* Get all sections in the fieldcollection */
  foreach ($sidebarnodes as $node) {
    $wrapper = entity_metadata_wrapper('node', $node);
    $section_location =isset($wrapper->field_sidebar_location)?$wrapper->field_sidebar_location->value():NULL;
    $field_collection = field_get_items('node', $node, 'field_section');
    if ($field_collection) {
      foreach ($field_collection as $collection) {
        $section = field_collection_field_get_entity($collection);
        $values = _get_section_content_json($bfl_roles, $section);
        $values['sidebarid'] = $node->nid;
        $values['section_location']=$section_location->field_system_code['und'][0]['value'];
      }
    }
    $params['section'][] = $values;
  }

  return $params;
}

/**
 * Helper function to dynamically get the tid from the term_name
 *
 * @param $term_name Term name
 * @param $vocabulary_name Name of the vocabulary to search the term in
 *
 * @return Term id of the found term or else FALSE
 */
function _get_term_from_name($term_name, $vocabulary_name) {
  if ($vocabulary = taxonomy_vocabulary_machine_name_load($vocabulary_name)) {
    $query = new EntityFieldQuery;
    $result = $query
        ->entityCondition('entity_type', 'taxonomy_term')
        ->propertyCondition('name', $term_name)
        ->propertyCondition('vid', $vocabulary->vid)
        ->execute();
    if ($result) {
      $tid = array_shift($result['taxonomy_term'])->tid;
      return $tid;
    }
  }

  return FALSE;
}

/**
 * To retrieve msid information for add page
 * @nid node id of add page
 * @return multisite node id
 */
function _get_multi_site_id($nid) {
  $result = FALSE;
  if (is_numeric($nid)) {
    $query = db_select('field_data_field_link_type_new', 'l');
    $query->fields('l', array('entity_id'));
    $query->innerJoin('field_data_field_existing_url', 'u', 'l.field_link_type_new_value = u.entity_id');
    $query->condition('u.field_existing_url_target_id', $nid);
    $query->condition('l.bundle', 'multisite', '=');
    $result = $query->distinct()->execute()->fetchAll();
  }
  return $result;
}
