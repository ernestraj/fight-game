<?php

module_load_include('inc', 'ge_json', 'include/content');
module_load_include('inc', 'ge_json', 'bfl');
/**
 * @file
 * The basic news article services function resides here
 *
 */

/**
 * This function returns the error messages'
 * @param type $error_code the status code.
 * @param type $error_message the mesage.
 */
function _return_errors($error_code, $error_message) {
  $params = array();
  $params['error']['error_code'] = $error_code;
  $params['error']['error_messsage'] = $error_message;
  return $params;
}

/* This function is for getting top five news_article */

function _get_newsarticle_json() {
  //Here we are going to get post paramters in json format
  $data = file_get_contents('php://input');
  $data_arr = drupal_json_decode($data);
  $valid = _validate_data_json($data_arr);
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
  $news_type_arr = explode('|', $data_arr['news_type']);
  $news_type_code = $news_type_arr[0];
  $round_tag_vid = _get_vid('news_roundup_tags');
  $news_type_vid = _get_vid('news_type');
  $passed_news_tag = $data_arr['news_tag'];
  if ($data_arr['news_tag'] == 'all') {
    $data_arr['news_tag'] = _get_taxonomy_tree($round_tag_vid, 'tid');
  }
  elseif (!empty($data_arr['news_tag'])) {
    $news_tag_tid = _get_news_tid($data_arr['news_tag'], $round_tag_vid);
    $news_category = 'news_roundup';
    $data_arr['news_tag'] = array($news_tag_tid);
  }
  if ($news_type_code == 'all') {
    $news_category = 'news_roundup';
    $data_arr['news_type'] = _get_news_roundup_types('news_roundup', 'tid');
  }
  elseif ($news_type_code != 'all') {

    if (!empty($news_type_arr)) {
      foreach ($news_type_arr as $news_type_item) {

        $news_type_tid[] = _get_news_type_tid($news_type_item);
      }
      $data_arr['news_type'] = $news_type_tid;
    }
    $news_category = _get_news_category($news_type_tid[0]);
  }

  $passed_date = $data_arr['date'];
  $data_arr['date'] = empty($data_arr['date']) ? time() : _convert_unix_time_stamp($data_arr['date']);
  $passed_month = $data_arr['month'];
  $data_arr['month'] = _get_passed_month($passed_date, $data_arr['month'], $data_arr['day']);
  $data_arr['offset'] = empty($data_arr['offset']) ? 0 : $data_arr['offset'];
  $preview = $data_arr['preview'];
  $valid = _validate_data($data_arr);


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

  $results = _get_news_article_nodes($data_arr, $country_language, $tids);

  $nodes = $results['nodes'];
  $params['total'] = $results['total'];
  $params['offset'] = $data_arr['offset'];
  $params['count'] = $data_arr['count'];
  $params['page'] = round(($data_arr['offset'] / $data_arr['count']) + 1);
  $params['month'] = $passed_month;

  if (($news_category == 'news_roundup') || ($news_type_code == 'all') || ($passed_news_tag == 'all')) {
    $tags['news_tag'] = _get_taxonomy_tree($round_tag_vid, 'portal');
    $params['news_tags'] = $tags;
    $params['news_types'] = _get_news_roundup_types('news_roundup', 'portal');
    $params['passed_type'] = $news_type_code;
    $params['passed_date'] = empty($passed_date) ? date('d-m-Y') : $passed_date;
    $params['passed_month'] = date('d-m-Y', ($data_arr['month']));
    $params['passed_tag'] = $passed_news_tag;
  }
  $params['passed_type'] = $news_type_code;
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
    $data = _get_news_article_single_node_content($bfl_roles, $node, $country_tid, $wrapper);
    $params['nodes'][] = $data;
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

function _get_news_type_tid($system_code) {
  $new_tid = '';
  if (!empty($system_code)) {
    $new_tid = db_select('field_data_field_system_code', 'sc')
        ->fields('sc', array('entity_id'))
        ->condition('field_system_code_value', $system_code, '=')
        ->execute()
        ->fetchField();
  }
  if (!empty($new_tid)) {
    return $new_tid;
  }

  return '';
}

function _get_news_article_nodes($data_arr, $lang, $tids) {
  $countries = array();
  $global_tid = _get_global_tid();
  $count = isset($data_arr['count']) ? $data_arr['count'] : NULL;
  $offset = $data_arr['offset'];
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
  $query->innerJoin('field_data_field_news_type', 'nt', 'nt.entity_id = c.entity_id');
  $query->innerJoin('node', 'n', "c.entity_id = n.nid AND (n.tnid = n.nid OR n.tnid = 0)");
  if (!empty($data_arr['news_tag'])) {
    $query->innerJoin('field_data_field_news_tags', 'ntag', 'c.entity_id=ntag.entity_id');
    $query->condition('ntag.field_news_tags_tid', $data_arr['news_tag'], IN);
  }
  $query->leftJoin('field_data_field_self_service_roles', 'sr', 'sr.entity_id = c.entity_id AND sr.entity_type = \'node\'');
  $query->leftJoin('field_data_field_weight', 'weight_weights', 'c.entity_id = weight_weights.entity_id AND weight_weights.entity_type=\'node\'');
  $query->leftJoin('field_data_field_live_date', 'ld', 'ld.entity_id = c.entity_id');
  $query->leftJoin('field_data_field_news_end_date', 'ed', 'ed.entity_id = c.entity_id');
  $query->condition('nt.field_news_type_tid', $data_arr['news_type'], 'IN');

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

  if ($data_arr['month'] > $data_arr['date']) {
    $start_date = $data_arr['date'];
    $end_date = $data_arr['month'];
  }
  elseif ($data_arr['month'] < $data_arr['date']) {
    $start_date = $data_arr['month'];
    $end_date = $data_arr['date'];
  }
  elseif ($data_arr['month'] == $data_arr['date']) {
    $start_date = strtotime("midnight", $data_arr['date']);
    $end_date = strtotime("tomorrow", $data_arr['date']) - 1;
  }
  $or = db_or();
  $or->condition(db_and()->isNull('ed.field_news_end_date_value')->condition('ld.field_live_date_value', $end_date, '<='));
  $or->condition(db_and()->isNotNull('ed.field_news_end_date_value')->condition('ed.field_news_end_date_value', $start_date, '>=')->condition('ld.field_live_date_value', $end_date, '<='));
  $query->condition($or);

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
  $query->condition('c.bundle', 'news_article', '=');
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

/* For get draft nodes */

function _get_preview_nodes($portal_preview, $check_nodes) {
  $nodes = array();
  foreach ($check_nodes as $current_node) {
    $previous_vid = _get_current_revision_id($current_node);
    $nid = $current_node->nid;
    if (!empty($portal_preview) && ($current_node->status == 1) && $previous_vid == FALSE) {
      $nodes[] = $current_node;
    }

    if (($current_node->status == 0)) {
      $preview_server = _get_preview_server($nid, $current_node->vid);
      if ($preview_server) {
        $nodes[] = $current_node;
      }
    }
    if ($previous_vid != FALSE) {
      $preview_server = _get_preview_server($nid, $previous_vid);
      if ($preview_server) {
        $nodes[] = node_load($nid, $previous_vid);
      }
      elseif (!$preview_server && ($current_node->status == 1)) {
        $nodes[] = $current_node;
      }
    }
  }
  return $nodes;
}

/**
 * Validates that all the $data_arr elements are set and have a valid argument.
 * @param array $data an array containing the elements for a valid call.
 * @param array $args an array with the parent funciton arguments.
 * @return mixed returns an array with the node id (nid) and the country id
 *  (country_id) or FALSE if at least one validation failed.
 */
function _validate_data($data_arr) {
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

  if (empty($data_arr['news_type'])) {
    $params = _print_errors('Fail', 'Invalid news type');
    return;
  }


  if (empty($data_arr['country_language'])) {
    $params = _print_errors('Fail', 'Empty or Invalid Language');
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
function _validate_data_json($data_arr) {
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

  if (empty($data_arr['news_type'])) {
    $params = _print_errors('Fail', 'Invalid news type');
    return;
  }


  return TRUE;
}

function _get_news_tid($name, $vid) {
  $taxonomy = taxonomy_get_term_by_name_and_vocabulary($name, $vid);
  if ($taxonomy) {
    $tid = array_keys($taxonomy);
    return $tid[0];
  }
  else {
    return FALSE;
  }
}

function _get_taxonomy_tree($vid, $key = 'tid') {
  $names = array();
  $terms = taxonomy_get_tree($vid);
  if ($terms) {
    foreach ($terms as $term) {
      $names[] = $term->name;
      $tids[] = $term->tid;
    }
    if ($key == 'portal') {
      return $names;
    }
    else {
      return $tids;
    }
  }
}

function _get_news_roundup_types($type = 'news_roundup', $key = 'tid') {
  $category = _get_vid('news_category');
  $news_type = _get_vid('news_type');
  $tid = _get_news_tid('news_roundup', $category);
  $taxonomyQuery = new EntityFieldQuery();
  $entities = $taxonomyQuery->entityCondition('entity_type', 'taxonomy_term')
      ->propertyCondition('vid', $news_type)
      ->fieldCondition('field_category', 'tid', $tid)
      ->execute();
  $terms = taxonomy_term_load_multiple(array_keys($entities['taxonomy_term']));
  foreach ($terms as $term) {
    $arr['name'] = $term->name;
    $arr['system_code'] = $term->field_system_code['und'][0]['value'];
    $list['news_type'][] = $arr;
    $list_tid[] = $term->tid;
  }
  if ($key == 'portal') {
    return $list;
  }
  else {
    return $list_tid;
  }
}

function _get_news_category($news_type_tid) {
  $news_category = 'none';
  $news_type_tax = taxonomy_term_load($news_type_tid);
  if (isset($news_type_tax->field_category)) {
    $news_category = taxonomy_term_load($news_type_tax->field_category['und'][0]['tid']);
    $news_category = $news_category->name;
  }
  return $news_category;
}

function _convert_unix_time_stamp($str) {
  $date = strtotime($str);
  if ($date === false) {
    $date = time();
  }
  return $date;
}

function _get_passed_month($passed_date, $month, $day) {
  $month = intval($month);

  if (empty($passed_date) && empty($month) && empty($day)) {
    $currentDate = time();
  }
  elseif (empty($passed_date) && !empty($month)) {
    $currentDate = new DateTime();
    $date = incrementDate($currentDate, $month);
  }
  elseif (!empty($passed_date) && !empty($month)) {
    $currentDate = new DateTime($passed_date);
    $day = $currentDate->format('d');
    $date = incrementDate($currentDate, $month);
    $date = date('Y-m-' . $day, $date);
    $date = strtotime($date);
  }
  elseif (!empty($passed_date) && ($month == 0)) {
    $currentDate = new DateTime($passed_date);
    $date = incrementDate($currentDate, $month);
  }

  if (!empty($day) && !empty($month)) {
    $date = incrementday($date, $day);
  }
  elseif (!empty($day) && ($month == 0)) {
    $current_date = time();
    $date = incrementday($current_date, $day);
  }
  elseif (!empty($day) && empty($month)) {
    $current_date = time();
    $date = incrementday($current_date, $day);
  }

  if (empty($date) || ($date === FALSE)) {
    $date = time();
  }

  return $date;
}

function incrementDate($startDate, $monthIncrement = 0) {
  $startingTimeStamp = $startDate->getTimestamp();
  // Get the month value of the given date:
  $monthString = date('Y-m', $startingTimeStamp);
  // Create a date string corresponding to the 1st of the give month,
  // making it safe for monthly calculations:
  $safeDateString = "first day of $monthString";
  // Increment date by given month increments:
  $incrementedDateString = "$safeDateString $monthIncrement month";
  $newTimeStamp = strtotime($incrementedDateString);
  $newDate = $newTimeStamp;
  return $newDate;
}

function incrementday($startDate, $day = 0) {
  $new_date = strtotime($day . " day", $startDate);
  return $new_date;
}
