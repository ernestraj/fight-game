<?php

module_load_include('inc', 'ge_json', 'includes/content');

function _get_tid_from_geid($geid, $vid) {
  if (is_array($geid) && !empty($geid)) {
    $query = new EntityFieldQuery();
    $entity = $query->entityCondition('entity_type', 'taxonomy_term')
        ->propertyCondition('vid', $vid)
        ->fieldCondition('field_geid', 'value', $geid, 'IN')
        ->execute();
    return $entity;
  }
  return FALSE;
}

/* get a users bfl tids as well as parent terms */

function _get_user_bfl_tids($businessgeid, $functiongeid, $locationgeid, $bandgeid) {
  $tids = array();
  $terms = array();
  $geids = array('business' => $businessgeid, 'function' => $functiongeid, 'location' => $locationgeid);
  foreach ($geids as $key => $geid) {
    $vid[$key] = _get_vid_from_key($key);
    $entity = _get_tid_from_geid($geid, $vid[$key]);
    if (is_array($entity)) {
      $terms = array_keys($entity['taxonomy_term']);
    }
    $all = _get_term_tid_all($key);
    if (isset($all[0])) {
      $terms[] = $all[0];
    }
    $tids[$key] = $terms;
    unset($terms);
  }
  $band_term = taxonomy_get_term_by_name($bandgeid, 'ge_bfl_band');
  if (!empty($band_term)) {
    $bands = array_keys($band_term);
    $tids['band'] = array($bands[0]);
  }
  $all = _get_term_tid_all('band');
  if (isset($all[0])) {
    $tids['band'][] = $all[0];
  }
  return $tids;
}

/* get all bfl in entityobject */

function _get_bfl_roles_terms_from_object($wrapper) {
  $bfl_roles['roles'] = isset($wrapper->field_self_service_roles) ? _get_field_bfl_json($wrapper->field_self_service_roles->value()) : array();
  $bfl_roles['business'] = isset($wrapper->field_business) ? _get_field_bfl_json($wrapper->field_business->value()) : array();
  $bfl_roles['function'] = isset($wrapper->field_function) ? _get_field_bfl_json($wrapper->field_function->value()) : array();
  $bfl_roles['location'] = isset($wrapper->field_location) ? _get_field_bfl_json($wrapper->field_location->value()) : array();
  $bfl_roles['band'] = isset($wrapper->field_band) ? _get_field_bfl_json($wrapper->field_band->value()) : array();
  return $bfl_roles;
}

function _get_field_bfl_json($field_bfl) {
  $tids = array();
  foreach ($field_bfl as $term) {
    $tids[$term->tid] = $term->tid;
  }
  return $tids;
}

/* Get targeting values in objects like node/field collection-groups/section/link_files)
 * $tids are user roles/business/function/location- tids['roles'],tids['business'],tids['function'],tids['location'] 
 */
function _targeting_value_in_object($user_tids, $wrapper) {
  if (empty($user_tids) || empty($user_tids['roles'])) {
    return FALSE;
  }

  $targeting = _get_bfl_roles_terms_from_object($wrapper);
  foreach ($user_tids as $key => $value) {
    if (!empty($targeting[$key]) && !empty($value)) {
      $flag_in_bfl_roles[$key] = _find_any_matches_json($value, $targeting[$key]);
    }
    else {
      $flag_in_bfl_roles[$key] = TRUE;
    }
    if ($flag_in_bfl_roles[$key] == FALSE) {
      return FALSE;
    }
  }
  return TRUE;
}

function _get_bfl_all_term_tid($variable_name, $vocabulary_name) {
  $variable_tid = variable_get($variable_name);
  if (empty($variable_tid)) {
    $variable_tid = _get_term_from_name('All', $vocabulary_name);
  }
  return $variable_tid;
}

function _get_term_tid_all($key) {
  if ($key == 'business') {
    $all_tid[] = _get_bfl_all_term_tid('business_all', 'ge_bfl_businesses');
  }
  elseif ($key == 'function') {
    $all_tid[] = _get_bfl_all_term_tid('function_all', 'ge_bfl_functions');
  }
  elseif ($key == 'location') {
    $all_tid[] = _get_bfl_all_term_tid('location_all', 'ge_bfl_locations');
  }
  elseif ($key == 'band') {
    $all_tid[] = _get_bfl_all_term_tid('band_all', 'ge_bfl_band');
  }
  return $all_tid;
}

function _get_vid_from_key($key) {
  $vid = null;
  if ($key == 'business') {
    $vid = _get_vid('ge_bfl_businesses');
  }
  elseif ($key == 'function') {
    $vid = _get_vid('ge_bfl_functions');
  }
  elseif ($key == 'location') {
    $vid = _get_vid('ge_bfl_locations');
  }
  elseif ($key == 'band') {
    $vid = _get_vid('ge_bfl_band');
  }
  return $vid;
}

    
function _get_node_details($values,$type){
  foreach($values as $v){     
     $check_depth=_check_depth($v,$type);
     if($check_depth==TRUE){
       $tax[$v->name]=$v->name;
     }
    }
    if(isset($tax['All'])||empty($values)){
      $all_list['All']='All';
      return $all_list;
    }
    ksort($tax);
  return $tax;
}
    
  
function _check_depth($v,$type) {
  if($type=='business'&&($v->hdepth==1||$v->hdepth==0))
     return TRUE;
  if($type=='location'&&($v->hdepth==2||$v->hdepth==0))
    return TRUE;
  
  return FALSE;
}