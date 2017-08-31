<?php

module_load_include('inc', 'ge_json', 'include/content');
module_load_include('inc', 'ge_json', 'bfl');

/**
 * This function is for getting top five news_article 
 */
function _get_policies_json() {
  //Here we are going to get post paramters in json format
  $data = file_get_contents('php://input');
  $data_attr = drupal_json_decode($data);
  if (empty($data_attr['countries'])) {
    _print_errors('Fail', 'No Country Information');
    return;
  }
  if (empty($data_attr['categoryid'])) {
    _get_policies_categories($data_attr);
  }
  else {
    _get_policy_nodes($data_attr);
  }
}

function _get_policy_nodes($data_attr) {
  $targeting = _get_targeting_values($data_attr);
  $nids = _get_policies_nodes($targeting['bfl_roles'], $targeting['countries'], $data_attr['categoryid'][0]);
  $data = _get_policies_content($nids, $targeting['bfl_roles'], $preview);
  $content = fix_keys($data);
  $nodes['content']['ppnodes'] = $content;
  print theme('generic_json', $nodes);
}

function _get_policies_json_tree() {
  $data = file_get_contents('php://input');
  $data_attr = drupal_json_decode($data);
  if (empty($data_attr['countries'])) {
    _print_errors('Fail', 'No Country Information');
    return;
  }

  if(empty($data_attr['languages'])) {
    _print_errors('Fail', 'No Language Information provided. Please contact Site administrator for the issue.');
    return;
  }

  if ($vocabulary = taxonomy_vocabulary_machine_name_load('policy_and_procedure_categories')) {
    if ($terms = taxonomy_get_tree($vocabulary->vid)) {
      foreach ($terms as $term) {
        $options[$term->tid] = array('id' => $term->tid, 'parent_id' => end($term->parents), 'name' => $term->name);
      }
    }
  }
  $key_content = convert_json_tree($options);
  $content = fix_keys($key_content);
  $tree['content']['term'] = $content;
  print theme('generic_json', $tree);
}

function _get_policies_categories($data_attr) {
  $targeting = _get_targeting_values($data_attr);
  if(!empty($data_attr['preview']))
    $preview = $data_attr['preview'];
  else
    $preview = "FALSE";

  if (empty($data_attr['countries'])) {
    _print_errors('Fail', 'No Country Information');
    return;
  }

  if(empty($data_attr['languages'])) {
    _print_errors('Fail', 'No Language Information provided. Please contact Site administrator for the issue.');
    return;
  }

  if ($vocabulary = taxonomy_vocabulary_machine_name_load('policy_and_procedure_categories')) {
    if ($terms = i18n_taxonomy_get_tree($vocabulary->vid,'en')) {
      foreach ($terms as $term) {
        $data = array();
        $nids = _get_policies_nodes($targeting['bfl_roles'], $targeting['countries'], $term->tid, $preview, $data_attr['languages']);
        if(!empty($nids))
          $data = _get_policies_content($nids, $targeting['bfl_roles'], $preview);

        $term_name = $term->name;
        if($data_attr['portal-language'] != 'en') {
          $term_content = i18n_taxonomy_term_get_translation($term, $data_attr['portal-language']);
          if(!empty($term_content))
            $term_name = $term_content->name;
        }
        $options[$term->tid] = array('id' => $term->tid, 'parent_id' => end($term->parents), 'name' => $term_name, 'ppnodes' => $data);
      }
    }
  }
  if ($vocabulary_trans = taxonomy_vocabulary_machine_name_load('terms_translations')) {
    if ($terms = i18n_taxonomy_get_tree($vocabulary_trans->vid,'en')) {
      foreach($terms as $term) {
        $term_name = $term->name;
        if($data_attr['portal-language'] != 'en') {
          $term_content = i18n_taxonomy_term_get_translation($term, $data_attr['portal-language']);
          if(empty($term_content))
            $term_content = taxonomy_term_load($term->tid);
          $term_name = $term_content->name;
          $key = $term_content->field_term_translation_key['und'][0]['value'];
        }
        else {
          $term = taxonomy_term_load($term->tid);
          $key = $term->field_term_translation_key['und'][0]['value'];
        }		
        $string_trans[$term->tid] = array($key => $term_name);
      }
    }
  }
  $key_content = convert_json_tree($options);
  $content = fix_keys($key_content);
  $string_key_content = convert_json_tree($string_trans);
  $string_content = fix_keys($string_key_content);
  $data['content']['heading'] = $string_content;
  $data['content']['term'] = $content;
  print theme('generic_json', $data);
}

function _get_targeting_values($data_attr) {
  if (!empty($data_attr['business']))
    $bfl_roles['business'] = _get_policies_term_ids($data_attr['business'], 'businesses');

  if (!empty($data_attr['location']))
    $bfl_roles['location'] = _get_policies_term_ids($data_attr['location'], 'locations');

  if (!empty($data_attr['function']))
    $bfl_roles['function'] = _get_policies_term_ids($data_attr['function'], 'functions');

  if (!empty($data_attr['band']))
    $bfl_roles['band'] = _get_policies_term_by_name($data_attr['band'], 'band');

  if (!empty($data_attr['roles']))
    $bfl_roles['roles'] = _get_policies_term_by_name($data_attr['roles']);

  if (!empty($data_attr['countries']))
    $countries = _get_policies_term_by_name($data_attr['countries']);

  $gbl = _get_policies_term_by_name(array('GBL'));

  $countries = array_merge($countries, $gbl);

  return array(
    'bfl_roles' => $bfl_roles,
    'countries' => $countries
  );
}

function fix_keys($array) {
  $numberCheck = false;
  foreach ($array as $k => $val) {
    if (is_array($val)) {
      $array[$k] = fix_keys($val); //recurse
    }
    if (is_numeric($k)) {
      $numberCheck = true;
    }
  }
  if ($numberCheck === true) {
    return array_values($array);
  }
  else {
    return $array;
  }
}

function convert_json_tree($data) {
  $itemsByReference = array();

  // Build array of item references:
  foreach ($data as $key => &$item) {
    $itemsByReference[$item['id']] = &$item;
    //$itemsByReference[$item['id']]['children'] = '';
  }

  // Set items as children of the relevant parent item.
  foreach ($data as $key => &$item)
    if ($item['parent_id'] && isset($itemsByReference[$item['parent_id']]))
      $itemsByReference [$item['parent_id']]['children'][$key] = &$item;

  // Remove items that were added to parents elsewhere:
  foreach ($data as $key => &$item) {
    if ($item['parent_id'] && isset($itemsByReference[$item['parent_id']]))
      unset($data[$key]);
  }

  return $data;
}

function _get_policies_content($nids, $bfl_roles, $preview) {
  $cache_key = '';
  foreach($nids as $item)
    $cache_key .= $item;

  $nodes = node_load_multiple($nids);
  if(strcmp($preview, "TRUE") == 0)
    $nodes = _get_current_policy_nodes($nodes);

  foreach ($nodes as $key => $item) {
    switch ($item->type) {
      case 'link_to_a_document':
        $field_collection = field_get_items('node', $item, 'field_files', $item->language);
        if ($field_collection) {
          foreach ($field_collection as $collection) {
            $file = field_collection_field_get_entity($collection);
            $access = _check_user_access($file->field_business, $file->field_function, $file->field_location, $file->field_band, $file->field_self_service_roles, $bfl_roles);
            if ($access) {
              $data[$key] = _get_node_information($item, $key);
              $data[$key]['filepath'] = $file->field_document['und'][0]['uri'];
              $data[$key]['url'] = '';
              $data[$key]['roles'] = _get_roles($item->field_self_service_roles);
              break;
            }
          }
        }
        break;
      case 'add_page':
        $data[$key] = _get_node_information($item, $key);
        $data[$key]['filepath'] = '';
        $data[$key]['url'] = '';
        $data[$key]['roles'] = _get_roles($item->field_self_service_roles);
        break;

      case 'create_links':
        $field_collection = field_get_items('node', $item, 'field_urls', $item->language);
        if ($field_collection) {
          foreach ($field_collection as $collection) {
            $url = field_collection_field_get_entity($collection);
            $access = _check_user_access($url->field_business, $url->field_function, $url->field_location, $url->field_band, $url->field_self_service_roles, $bfl_roles);
            if ($access) {
              $data[$key] = _get_node_information($item, $key);
              $data[$key]['filepath'] = '';
              $data[$key]['url'] = $url->field_url['und'][0]['url'];
              $data[$key]['roles'] = _get_roles($item->field_self_service_roles);
              break;
            }
          }
        }
        break;
    }
  }
  return $data;
}

function _get_current_policy_nodes($nodes) {
  foreach($nodes as $key=>$node) {
    $current_rid = _get_current_revision_id($node);
    if($current_rid) {
      $nodes[$key] = node_load($node->nid, $current_rid);
    }
  }
  return $nodes;
}

function _get_node_information($item, $key) {
  $country = taxonomy_term_load($item->field_country['und'][0]['tid']);
  $data['nid'] = $item->nid;
  $data['type'] = $item->type;
  $data['title'] = $item->title;
  $data['categoryid'] = $item->field_policy_category['und'][0]['tid'];
  $data['country'] = $country->name;

  return $data;
}

/**
 *
 */
function _check_user_access($business, $function, $location, $band, $roles, $user_roles) {
  if ((empty($business[LANGUAGE_NONE]) || _targeting_comparison($business[LANGUAGE_NONE], $user_roles['business']) || in_array(_get_targeting_all_tid('business', $business[LANGUAGE_NONE]))) && (empty($function[LANGUAGE_NONE]) || _targeting_comparison($function[LANGUAGE_NONE], $user_roles['function']) || in_array(_get_targeting_all_tid('function'), $function[LANGUAGE_NONE])) && (empty($location[LANGUAGE_NONE]) || _targeting_comparison($location[LANGUAGE_NONE], $user_roles['location']) || in_array(_get_targeting_all_tid('location', $location[LANGUAGE_NONE]))) && (empty($band[LANGUAGE_NONE]) || _targeting_comparison($band[LANGUAGE_NONE], $user_roles['band']) || in_array(_get_targeting_all_tid('band'), $band[LANGUAGE_NONE])) && (empty($roles[LANGUAGE_NONE]) || _targeting_comparison($roles[LANGUAGE_NONE], $user_roles['roles'])))
    return TRUE;
  else
    return FALSE;
}

function _targeting_comparison($targeting, $user_targeting) {
  foreach ($targeting as $item) {
    if (in_array($item['tid'], $user_targeting))
      return TRUE;
  }
  return FALSE;
}

function _get_roles($roles) {
  if (empty($roles))
    return '';
  else {
    $role_names = _get_role_names($roles[LANGUAGE_NONE]);
    return $role_names;
  }
}

function _get_policies_nodes($bfl_roles, $countries, $category_id, $preview, $language) {
  $query = db_select('field_data_field_country', 'c');
  $query->fields('c', array('entity_id', 'field_country_tid'));
  $query->innerJoin('field_data_field_policy_category', 'pc', 'pc.entity_id = c.entity_id AND pc. field_policy_category_tid= ' . $category_id);
  $query->innerJoin('node', 'n', "c.entity_id = n.nid");
  $query->condition('n.language', $language, "IN");
  $query->leftJoin('field_data_field_self_service_roles', 'sr', 'sr.entity_id = c.entity_id AND sr.entity_type = \'node\'');
  if (!empty($bfl_roles['business']) || !(is_null($bfl_roles['business']))) {
    $query->leftJoin('{field_data_field_business}', 'field_data_field_business', 'n.nid = field_data_field_business.entity_id AND (field_data_field_business.entity_type = \'node\' AND field_data_field_business.deleted = \'0\')');
    $or_business = db_or();
    $or_business->condition('field_data_field_business.field_business_tid', $bfl_roles['business'], 'IN');
    $or_business->isNull('field_data_field_business.field_business_tid');
    $query->condition($or_business);
  }
  if (!empty($bfl_roles['function']) || !(is_null($bfl_roles['function']))) {
    $query->leftJoin('{field_data_field_function}', 'field_data_field_function', 'n.nid = field_data_field_function.entity_id AND (field_data_field_function.entity_type = \'node\' AND field_data_field_function.deleted = \'0\')');
    $or_function = db_or();
    $or_function->condition('field_data_field_function.field_function_tid', $bfl_roles['function'], 'IN');
    $or_function->isNull('field_data_field_function.field_function_tid');
    $query->condition($or_function);
  }

  if (!empty($bfl_roles['location']) || !(is_null($bfl_roles['location']))) {
    $query->leftJoin('{field_data_field_location}', 'field_data_field_location', 'n.nid = field_data_field_location.entity_id AND (field_data_field_location.entity_type = \'node\' AND field_data_field_location.deleted = \'0\')');
    $or_location = db_or();
    $or_location->condition('field_data_field_location.field_location_tid', $bfl_roles['location'], 'IN');
    $or_location->isNull('field_data_field_location.field_location_tid');
    $query->condition($or_location);
  }

  if (!empty($bfl_roles['band']) || !(is_null($bfl_roles['band']))) {
    $query->leftJoin('{field_data_field_band}', 'field_data_field_band', 'n.nid = field_data_field_band.entity_id AND (field_data_field_band.entity_type = \'node\' AND field_data_field_band.deleted = \'0\')');
    $or_band = db_or();
    $or_band->condition('field_data_field_band.field_band_tid', $bfl_roles['band'], 'IN');
    $or_band->isNull('field_data_field_band.field_band_tid');
    $query->condition($or_band);
  }

  if (!empty($bfl_roles['roles'])) {
    $or_2 = db_or();
    $or_2->condition('sr.field_self_service_roles_tid', $bfl_roles['roles'], 'IN');
    $or_2->isNull('sr.field_self_service_roles_tid');
    $query->condition($or_2);
  }
  else {
    $query->isNull('sr.field_self_service_roles_tid');
  }
  $query->condition('c.field_country_tid', $countries, 'IN');
  if (strcmp($preview,"FALSE") == 0) {
    $query->condition('n.status', 1, '=');
  }
  $query->orderBy('n.title', 'ASC');
  $result = $query->execute()->fetchAll();
  $nids = array();
  foreach ($result as $node) {
    $nids[] = utf8_encode($node->entity_id);
  }

  if (!empty($nids))
    return $nids;
  else
    return '';
}

function _get_policies_term_ids($geids, $type) {
  $tids = array();
  $taxonomnyQuery = new EntityFieldQuery();
  $terms = $taxonomnyQuery->entityCondition('entity_type', 'taxonomy_term')
      ->fieldCondition('field_geid', 'value', $geids, 'IN')
      ->propertyOrderBy('tid', 'DESC')
      ->execute();

  $all_tid = _get_targeting_all_tid($type);
  $tids[] = $all_tid;
  if (isset($terms['taxonomy_term'])) {
    $tids = array_keys($terms['taxonomy_term']);
    $tids[] = $all_tid;
    return $tids;
  }
  else
    return $tids;
}

function _get_policies_term_by_name($names, $type = '') {
  $tids = array();
  $taxonomnyQuery = new EntityFieldQuery();
  $terms = $taxonomnyQuery->entityCondition('entity_type', 'taxonomy_term')
      ->propertyCondition('name', $names, 'IN')
      ->propertyOrderBy('tid', 'DESC')
      ->execute();

  if (!empty($type)) {
    $all_tid = _get_targeting_all_tid($type);
    $tids[] = $all_tid;
  }

  if (isset($terms['taxonomy_term'])) {
    $tids = array_keys($terms['taxonomy_term']);
    if (!empty($all_tid))
      $tids[] = $all_tid;

    return $tids;
  }
  else
    return $tids;
}

function _get_targeting_all_tid($type) {
  $all = taxonomy_get_term_by_name('All', 'ge_bfl_' . $type);
  return key($all);
}

function _get_role_names($roles) {
  $terms = taxonomy_term_load_multiple($roles);
  foreach ($terms as $item) {
    $names[] = $item->name;
  }
  return $names;
}
