<?php

module_load_include('inc', 'ge_json', 'include/content');
module_load_include('inc', 'ge_json', 'bfl');

function _get_tour_json() {
  //Here we are going to get post paramters in json format
  $data = file_get_contents('php://input');
  $data_arr = drupal_json_decode($data);
  $valid = _validate_tour_data_json($data_arr);
  if ($valid !== TRUE) {
    return $valid;
  }
  $default_language = _get_country_details($data_arr['country'], 'lang');
  $country_language = isset($default_language) ? $default_language : "null";
  $country_tid = _get_country_details($data_arr['country'], 'tid');
  $tids = _get_tids_portal($data_arr['roles'], $country_tid);
  $businessgeid = $data_arr['business'];
  $functiongeid = $data_arr['function'];
  $locationgeid = $data_arr['location'];
  $bandgeid = $data_arr['band'];
  $bfl = _get_user_bfl_tids($businessgeid, $functiongeid, $locationgeid, $bandgeid);
  $data_arr['bfl'] = $bfl;
  $data_arr['tids'] = $tids;
  $data_arr['country'] = $country_tid;
  $data_arr['country_language'] = $country_language;
  $data_arr['date'] = empty($data_arr['date']) ? time() : _convert_unix_time_stamp($data_arr['date']);
  $data_arr['tour_items']=_get_tids_for_system_codes($data_arr['tour_items']);
  $preview = $data_arr['preview'];

  $valid = _validate_tour_data($data_arr);
  if ($valid !== TRUE) {
    return $valid;
  }

  if ((strcmp($preview, 'true') == 0)) {
    $flag = TRUE;
  }
  else {
    $flag = FALSE;
  }

  $data_arr['preview'] = FALSE;

  $portal_preview = '';
  if (isset($_SERVER["HTTP_PORTALPREVIEW"])) {
    $portal_preview = check_plain($_SERVER["HTTP_PORTALPREVIEW"]);
    $data_arr['preview'] = TRUE;
  }

  $results = _get_tour_nodes($data_arr, $country_language, $tids);

  $nodes = $results['nodes'];
  $params['count'] = $data_arr['count'];


  if (empty($nodes)) {
    $params['error_code'] = 'Fail';
    $params['error_message'] = 'No nodes available/Nodes are more than limit';
    $params = array_remove_null($params);
    return $params;
  }
  $portal_preview = '';
  if (isset($_SERVER["HTTP_PORTALPREVIEW"])) {
    $portal_preview = check_plain($_SERVER["HTTP_PORTALPREVIEW"]);
  }


  /* If preview is true and portal server is present,we get all latest drafts */
  if (!empty($portal_preview)) {
    $got_header = TRUE;
    $preview_nodes = _get_preview_nodes($portal_preview, $nodes);
    $nodes = array();
    $nodes = $preview_nodes;
  }

  $bfl_roles['business'] = $bfl['business'];
  $bfl_roles['function'] = $bfl['function'];
  $bfl_roles['location'] = $bfl['location'];
  $bfl_roles['band'] = $bfl['band'];
  $bfl_roles['roles'] = $tids;


  foreach ($nodes as $node) {
    $wrapper = entity_metadata_wrapper('node', $node);
    $data = _get_tour_single_node_content($bfl_roles, $node, $country_tid, $wrapper);
    $params['nodes'][] = $data;
	$params['tour_items']=$data_arr['tour_items'];
  }

  if (empty($params['nodes'])) {
    $params['error_code'] = 'Fail';
    $params['error_message'] = 'No nodes available/Nodes are more than limit';
    $params = array_remove_null($params);
    return $params;
  }
  $params = array_remove_null($params);
  return $params;
}

function _get_tour_nodes($data_arr, $lang, $tids) {
  $countries = array();
  $global_tid = _get_global_tid();
  $count = isset($data_arr['count']) ? $data_arr['count'] : NULL;
  $offset = isset($data_arr['offset']) ? $data_arr['offset'] : 0;
  $country_tid = $data_arr['country'];
  if (!empty($global_tid)) {
    $countries = array($global_tid, $country_tid);
  }
  $business = $data_arr['bfl']['business'];
  $function = $data_arr['bfl']['function'];
  $location = $data_arr['bfl']['location'];
  $band = $data_arr['bfl']['band'];
  $query = db_select('field_data_field_country', 'c');
  $query->fields('c', array('entity_id', 'field_country_tid'));
  $query->innerJoin('node', 'n', "c.entity_id = n.nid AND (n.tnid = n.nid OR n.tnid = 0) AND n.type='tour'");
  $query->leftJoin('field_data_field_self_service_roles', 'sr', 'sr.entity_id = c.entity_id AND sr.entity_type = \'node\'');
  $query->leftJoin('{weight_settings}', 'weight_settings', 'n.type = weight_settings.type');
  $query->leftJoin('field_data_field_weight', 'weight_weights', 'n.nid = weight_weights.entity_id AND weight_weights.entity_type=\'node\'');
  $query->leftJoin('field_data_field_live_date', 'ld', 'ld.entity_id = c.entity_id');
  $query->leftJoin('field_data_field_news_end_date', 'ed', 'ed.entity_id = c.entity_id');
  if (!empty($business) || !(is_null($business))) {
    $query->leftJoin('{field_data_field_business}', 'field_data_field_business', 'n.nid = field_data_field_business.entity_id AND (field_data_field_business.entity_type = \'node\' AND field_data_field_business.deleted = \'0\')');
    $or_business = db_or();
    $or_business->condition('field_data_field_business.field_business_tid', $business, 'IN');
    $or_business->isNull('field_data_field_business.field_business_tid');
    $query->condition($or_business);
  }
  if (!empty($function) || !(is_null($function))) {
    $query->leftJoin('{field_data_field_function}', 'field_data_field_function', 'n.nid = field_data_field_function.entity_id AND (field_data_field_function.entity_type = \'node\' AND field_data_field_function.deleted = \'0\')');
    $or_function = db_or();
    $or_function->condition('field_data_field_function.field_function_tid', $function, 'IN');
    $or_function->isNull('field_data_field_function.field_function_tid');
    $query->condition($or_function);
  }

  if (!empty($location) || !(is_null($location))) {
    $query->leftJoin('{field_data_field_location}', 'field_data_field_location', 'n.nid = field_data_field_location.entity_id AND (field_data_field_location.entity_type = \'node\' AND field_data_field_location.deleted = \'0\')');
    $or_location = db_or();
    $or_location->condition('field_data_field_location.field_location_tid', $location, 'IN');
    $or_location->isNull('field_data_field_location.field_location_tid');
    $query->condition($or_location);
  }

  if (!empty($band) || !(is_null($band))) {
    $query->leftJoin('{field_data_field_band}', 'field_data_field_band', 'n.nid = field_data_field_band.entity_id AND (field_data_field_band.entity_type = \'node\' AND field_data_field_band.deleted = \'0\')');
    $or_band = db_or();
    $or_band->condition('field_data_field_band.field_band_tid', $band, 'IN');
    $or_band->isNull('field_data_field_band.field_band_tid');
    $query->condition($or_band);
  }
  

  
  if (!empty($data_arr['tour_items']) || !(is_null($data_arr['tour_items']))) {
    $query->leftJoin('{field_data_field_tour_items}', 'ti', 'n.nid = ti.entity_id AND (ti.entity_type = \'node\')');
    $query->condition('ti.field_tour_items_tid', $data_arr['tour_items'], 'IN');
     }

  $start_date = strtotime("midnight", $data_arr['date']);
  $end_date = strtotime("tomorrow", $data_arr['date']) - 1;

   
  if (!empty($start_date) && !empty($end_date)) {
  $or = db_or();
  $or->condition(db_and()->isNull('ed.field_news_end_date_value')->condition('ld.field_live_date_value', $end_date, '<='));
  $or->condition(db_and()->isNotNull('ed.field_news_end_date_value')->condition('ed.field_news_end_date_value', $start_date, '>=')->condition('ld.field_live_date_value', $end_date, '<='));
  $query->condition($or);
}
  if (!empty($tids)) {
    $or_2 = db_or();
    $or_2->condition('sr.field_self_service_roles_tid', $tids, 'IN');
    $or_2->isNull('sr.field_self_service_roles_tid');
    $query->condition($or_2);
  }
  else {
    $query->isNull('sr.field_self_service_roles_tid');
  }
  $query->condition('c.field_country_tid', $countries, 'IN');
  if ($data_arr['preview'] == FALSE) {
    $query->condition('n.status', 1, '=');
  }
  $query->orderBy('weight_weights.field_weight_value', 'ASC');
  $query->orderBy('ld.field_live_date_value', 'DESC');
  $query->orderBy('n.changed', 'DESC');
  if (!empty($count)) {
    $total = $query->execute()->rowCount();
    $query->range($offset, $count);
  }
  $result = $query->execute()->fetchAll();
  $nids = array();

  foreach ($result as $node) {
    $nids[] = $node->entity_id;
  }
  $nodes = array();
  $passed_language = $data_arr['language'];
  if (!empty($nids)) {
    foreach ($nids as $nid) {
      $new_nids[] = _get_tanslated_node($nid, $passed_language);
    }
  }
  if (!empty($new_nids)) {
    $nodes = node_load_multiple($new_nids);
  }
  $results['nodes'] = $nodes;
  $results['total'] = $total;
  return $results;
}

function _get_tour_single_node_content($bfl_roles, $node, $country_tid, $wrapper) {

  $data['id'] = $node->nid;
  $data['title'] = $node->title;
  $data['image'] = $node->field_image[LANGUAGE_NONE][0]['uri'];
  $data['top_menu'] = $node->field_top_menu[LANGUAGE_NONE][0]['value'];
  $data['effective_date'] = _get_date($node->field_live_date[LANGUAGE_NONE][0]['value']);
  $data['end_date'] = _get_date($node->field_news_end_date[LANGUAGE_NONE][0]['value']);
  $data['body'] = $node->body['und'][0]['value'];
  $values = $wrapper->field_asset->raw();
  $asset = array();
  foreach ($values as $value) {
    $asset[] = $value['uri'];
  }
  $data['force_view'] = filter_var($node->field_force_view[LANGUAGE_NONE][0]['value'], FILTER_VALIDATE_BOOLEAN);
  $data['site_check'] = filter_var($node->field_site_check[LANGUAGE_NONE][0]['value'], FILTER_VALIDATE_BOOLEAN);
  $data['device_check'] = filter_var($node->field_device_check[LANGUAGE_NONE][0]['value'], FILTER_VALIDATE_BOOLEAN);
  $data['location_of_tour'] = $node->field_location_of_tour[LANGUAGE_NONE][0]['value'];
  $tour_items = $wrapper->field_tour_items->value();
  $data['tour_items'] = isset($tour_items->field_system_code[LANGUAGE_NONE][0]['value']) ? $tour_items->field_system_code[LANGUAGE_NONE][0]['value'] : $tour_items->name;
  $data['node_asset'] = $asset;
  return $data;
}

function _validate_tour_data_json($data_arr) {
  if (empty($data_arr)) {
    $params = _print_errors('Fail', 'Empty Data');
    return;
  }

  if (empty($data_arr['country'])) {
    $params = _print_errors('Fail', 'Empty or Invalid country');
    return;
  }

  if (empty($data_arr['roles'])) {
    $params = _print_errors('Fail', 'Empty Roles/Non-Existent roles in CMS/Invalid miss match for country and roles');
    return;
  }

  if (empty($data_arr['preview'])) {
    $params = _print_errors('Fail', 'Empty Preview');
    return;
  }
  if (empty($data_arr['tour_items'])) {
    $params = _print_errors('Fail', 'Empty tour items');
    return;
  }

  return TRUE;
}

/**
 * Validates that all the $data_arr elements are set and have a valid argument.
 * @param array $data an array containing the elements for a valid call.
 * @param array $args an array with the parent funciton arguments.
 * @return mixed returns an array with the node id (nid) and the country id
 *  (country_id) or FALSE if at least one validation failed.
 */
function _validate_tour_data($data_arr) {
  if (empty($data_arr)) {
    $params = _print_errors('Fail', 'Empty Data');
    return;
  }

  if (empty($data_arr['country'])) {
    $params = _print_errors('Fail', 'Empty or Invalid country');
    return;
  }

  if (empty($data_arr['tids'])) {
    $params = _print_errors('Fail', 'Empty Roles/Non-Existent roles in CMS/Invalid miss match for country and roles');
    return;
  }

  if (empty($data_arr['preview'])) {
    $params = _print_errors('Fail', 'Empty Preview');
    return;
  }

  if (empty($data_arr['country_language'])) {
    $params = _print_errors('Fail', 'Empty or Invalid Language');
    return;
  }

  return TRUE;
}

/* get tids for system code*/
function _get_tids_for_system_codes($system_codes) {
  $new_tids = '';
  if (!empty($system_codes)) {
    $new_tids = db_select('field_data_field_system_code', 'sc')
        ->fields('sc', array('entity_id'))
        ->condition('field_system_code_value', $system_codes, 'IN')
        ->execute()
        ->fetchAll();
  }
  if (!empty($new_tids)) {
	  foreach($new_tids as $value){
		 $tids[]= $value->entity_id;
	  }
	   return $tids;
  }

  

  return '';
}