<?php

/**
 * @file
 * The basic services function resides here
 *
 */
module_load_include('inc', 'ge_json', 'utilities');
module_load_include('inc', 'ge_json', 'bfl');

/**
 * This function prints the error messages
 *
 * @param type $error_code the status code.
 * @param type $error_message the mesage.
 */
function _print_errors($error_code, $error_message) {
  $params = array();
  $params['error']['error_code'] = $error_code;
  $params['error']['error_messsage'] = $error_message;
  print theme('error_json', $params);
}

/**
 * Function which will determine what node type and which function should be called
 * parameters we are going to get in json format {roles:['role1','role2','role3','role4','role5'], ohrid: 'ohrid',preview:true/false}
 */
function _get_content_json() {
//Here we are going to get post paramters
  $data = file_get_contents('php://input');
  if (!empty($data)) {
    $data_arr = drupal_json_decode($data);
    if (is_null($data_arr)) {
      _print_errors('Fail', 'Invalid JSON');
      return;
    }
    $businessgeid = $data_arr['business'];
    $functiongeid = $data_arr['function'];
    $locationgeid = $data_arr['location'];
    $bandgeid = $data_arr['band'];
    $bfl = _get_user_bfl_tids($businessgeid, $functiongeid, $locationgeid, $bandgeid);
    $roles = $data_arr['roles'];
    if (isset($data_arr['msid']) && is_numeric($data_arr['msid'])) {
      $msid = $data_arr['msid'];
    }
    else {
      $msid = NULL;
    }
    if (empty($roles)) {
      _print_errors('Fail', 'Empty Roles');
      return;
    }

    $ohrid = $data_arr['ohrid'];
    $preview = $data_arr['preview'];
    if (empty($preview)) {
      _print_errors('Fail', 'Empty Preview');
      return;
    }
    $country = $data_arr['country'];
    $country_tid = _get_country_details($country, 'tid');
    if (empty($country) || empty($country_tid)) {
      _print_errors('Fail', 'Empty/Invalid Country');
      return;
    }
    $args = func_get_args();
    $params = array();
    if (!empty($args)) {
      $nid = (!empty($args[0]) ) ? $args[0] : NULL;
    }
    if (!is_numeric($nid)) {
      _print_errors('Fail', 'Invalid node id');
      return;
    }

    if ((strcmp($preview, 'true') == 0)) {
      $flag = TRUE;
    }
    else {
      $flag = FALSE;
    }
    $preview_server = FALSE;
    $show_preview = FALSE;
    if (!is_null($nid)) {
      if (isset($args[2])) {
        $lang = check_plain($args[2]);
        $nid = _get_tanslated_node($nid, $lang);
      }
      $previous_vid = FALSE;
      $current_node = node_load($nid);
      if ($current_node) {
        $previous_vid = _get_current_revision_id($current_node);
      }
      if ($flag) {
        $show_preview = TRUE;
        if ($previous_vid == FALSE) {
          $node = $current_node;
        }
        else {
          $node = node_load($nid, $previous_vid);
        }
      }
      if (!$flag) {
        $node = $current_node;
        $show_preview = FALSE;
      }
      $portal_preview = $_SERVER["HTTP_PORTALPREVIEW"];
      if (!empty($portal_preview)) {
        $show_preview = TRUE;
        $preview_server = _get_preview_server($nid, $previous_vid);
        if (($preview_server) && ($flag === FALSE)) {
          $node = node_load($nid, $previous_vid);
        }
      }else{
		  if (empty($preview_server)&&$node->status==0) {
           _print_errors('Fail', 'Node is unpublished');
            return;
        }
    }
    }
  }



  if (isset($node->nid)) {
    $type = $node->type;
    $flag_country = FALSE;
    $lang = field_language('node', $node, 'field_country');
    $node_country_tid = $node->field_country[$lang][0]['tid'];
    /* Get the global country */
    $global_country_tid = _get_global_tid();
    if ((($node_country_tid == $country_tid)) || ($node_country_tid == $global_country_tid)) {
      $flag_country = TRUE;
    }

    if (!$flag_country) {
      _print_errors('Fail', 'Node doesnt have country-' . $country);
      return;
    }
    $return_type = 'json';
    switch ($type) {
      case 'create_links':
        _get_url_content_json($roles, $node, $country_tid, $bfl,$country);
        break;
      case 'link_to_a_document':
        _get_document_content_json($roles, $node, $country_tid, $bfl,$country);
        break;
      case 'add_page':
        _get_page_content_json($roles, $node, $country_tid, $bfl, $msid, $show_preview,$country);
        break;
      case 'news_article':
        _get_news_article_single_node_content_json($roles, $node, $country_tid, $bfl);
        break;
      case 'multisite':
        _get_multisite_json_content($roles, $node, $country_tid, $bfl, $msid, $show_preview,$country);
        break;
      default:
        $content = 'No Results for this node';
    }
  }
  else {
    $error_code = 'fail';
    $error_message = 'No node exists';
    $params['error']['error_code'] = $error_code;
    $params['error']['error_messsage'] = $error_message;
    print theme('error_json', $params);
    return;
  }
}

function _get_multisite_json($roles, $node, $country_tid, $request_bfl, $msid, $show_preview,$country) {
  $flag_page = FALSE;
  if (!empty($roles)) {
    try {
      $wrapper = entity_metadata_wrapper('node', $node);
    }
    catch (EntityMetadataWrapperException $e) {
      watchdog_exception('services', $e);
      _print_errors('Fail', 'Error in wrapper,please logs in CMS,some module/file is missing');
      return;
    }
    $bfl_roles['roles'] = _get_tids_portal($roles, $country_tid);
    $bfl_roles['business'] = $request_bfl['business'];
    $bfl_roles['function'] = $request_bfl['function'];
    $bfl_roles['location'] = $request_bfl['location'];
    $bfl_roles['band'] = $request_bfl['band'];
    $flag_page = _targeting_value_in_object($bfl_roles, $wrapper);
    $menu = $wrapper->field_menu->value();
    if ($flag_page) {
      $type = $node->type;
      $params['country']=$country;
      $params['id'] = $node->nid;
      $params['type'] = $type;
      $params['title'] = $node->title;
      $params['page_css_class_names'] = isset($wrapper->field_css_class_names) ? $wrapper->field_css_class_names->value() : NULL;
      $multisite_type = isset($wrapper->field_multisite_type) ? $wrapper->field_multisite_type->value() : NULL;
      $multisite_type_code = $multisite_type->field_system_code[LANGUAGE_NONE][0]['safe_value'];
      $params['multisite_type'] = $multisite_type_code;
      $field_collection = field_get_items('node', $node, 'field_link_type_new');
      $params['linkfiles'][] = _get_link_files_content_json($bfl_roles, $field_collection);
    }
  }

  $type = 'section';
  if ($flag_page) {
    $sidebarnodes = _get_section_nodes($menu->tid, $country_tid, $bfl_roles, $node->language, $type, $show_preview);
    if (!empty($sidebarnodes)) {
      $params['sidebar'] = _get_sidebar_values($sidebarnodes, $bfl_roles);
    }

    return $params;
  }
}

function _get_multisite_json_content($request_roles, $node, $country_tid, $request_bfl, $msid, $show_preview,$country) {
  $multisite['multisite'] = _get_multisite_json($request_roles, $node, $country_tid, $request_bfl, $msid, $show_preview,$country);
  if ((is_null($msid) && isset($multisite['multisite']['linkfiles'][0]['section_links_files'][0]['nid']))) {
    $first_nid = $multisite['multisite']['linkfiles'][0]['section_links_files'][0]['nid'];
    $lang = $node->language;
    $first_nid = _get_tanslated_node($first_nid, $lang);
    $first_node = node_load($first_nid);
    if ($show_preview) {
      $previous_vid = _get_current_revision_id($first_node);
      if (isset($previous_vid)) {
        $first_node = node_load($first_nid, $previous_vid);
      }
    }
    $type = $first_node->type;
    if ($type == "add_page") {
      $node_content = _get_page_content($request_roles, $first_node, $country_tid, $request_bfl, $msid, $show_preview,$country);
      if (!empty($node_content)) {
        $params['id'] = $first_node->nid;
        $params['type'] = $type;
        $params['node'] = $node_content['node'];
        $params['groups'] = $node_content['groups'];
        $params['sidebar']=$node_content['sidebar'];
      }
    }
  }
  $params['multisite'] = $multisite['multisite'];
  if (empty($params['multisite'])) {
    $error_code = 'fail';
    $error_message = 'No targeting roles and business and function and location';
    $params['error']['error_code'] = $error_code;
    $params['error']['error_messsage'] = $error_message;
    print theme('error_json', $params);
    return;
  }
  else {
    $params['content'] = $params;
    print theme('generic_json', $params);
  }
}

function _get_document_content_json($request_roles, $node, $country_tid, $request_bfl,$country) {
  if (!empty($request_roles)) {
    $type = $node->type;
    $tids = _get_tids_portal($request_roles, $country_tid);
    try {
      $wrapper = entity_metadata_wrapper('node', $node);
    }
    catch (EntityMetadataWrapperException $e) {
      watchdog_exception('services', $e);
      return;
    }
    $image = $wrapper->field_image_for_file->value();
    $bfl_roles['roles'] = _get_tids_portal($request_roles, $country_tid);
    $bfl_roles['business'] = $request_bfl['business'];
    $bfl_roles['function'] = $request_bfl['function'];
    $bfl_roles['location'] = $request_bfl['location'];
    $bfl_roles['band'] = $request_bfl['band'];
    $flag_page = _targeting_value_in_object($bfl_roles, $wrapper);
    $params['country']=$country;
    $params['id'] = $node->nid;
    $params['type'] = $type;
    $params['top_menu'] = $wrapper->field_top_menu->value();
    $params['node_title'] = $node->title;
    if (!empty($image)) {
      $params['image'] = $image['uri'];
    }
//if flag is true get files
    if ($flag_page) {
      $role_rids = array();
      $field_collection = field_get_items('node', $node, 'field_files', $node->language);
      if ($field_collection) {
        foreach ($field_collection as $collection) {
          $fc = field_collection_field_get_entity($collection);
          $wrapper = entity_metadata_wrapper('field_collection_item', $fc);
          $ids[] = $wrapper->item_id->value();
// all values from a field collection
          $field_admin_title = $wrapper->field_admin_title->value();
          $field_role = $wrapper->field_self_service_roles->value(); // an array of self service data
          $field_document = $wrapper->field_document->value();
          $result = _targeting_value_in_object($bfl_roles, $wrapper);

          if ($result) {
            $params['admin_title'] = $field_admin_title;
            $params['document'] = $field_document['uri'];
            break;
          }
          $role_rids = array();
        }
      }
    }
  }
  if ($flag_page) {
    $params['node'] = $params;
    print theme('doc_json', $params);
  }
  else {
    $error_code = 'fail';
    $error_message = 'No targeting roles/business/function/location';
    $params['error']['error_code'] = $error_code;
    $params['error']['error_messsage'] = $error_message;
    print theme('error_json', $params);
    return;
  }
}

function _get_url_content_json($request_roles, $node, $country_tid, $request_bfl,$country) {
  $flag_page = FALSE;

  if (!empty($request_roles)) {
    $type = $node->type;
    $params['country']=$country;
    $params['id'] = $node->nid;
    $params['type'] = $type;
    $tids = _get_tids_portal($request_roles, $country_tid);
    $wrapper = entity_metadata_wrapper('node', $node);
    $urls = $wrapper->field_urls->value();
    $image = $wrapper->field_image->value();
    $window_type = $wrapper->field_window_types->value();
    $window_type_code = $window_type->field_system_code[LANGUAGE_NONE][0]['safe_value'];
    $params['window_type'] = $window_type->name;
    $params['window_type_code'] = $window_type_code;
    $top_menu = $wrapper->field_top_menu->value();
    if (!empty($top_menu)) {
      $params['top_menu'] = $top_menu;
    }
    $params['node_title'] = $node->title;
    if (!empty($image)) {
      $params['image'] = $image['uri'];
    }
    $bfl_roles['roles'] = array_values(_get_tids_portal($request_roles, $country_tid));
    $bfl_roles['business'] = $request_bfl['business'];
    $bfl_roles['function'] = $request_bfl['function'];
    $bfl_roles['location'] = $request_bfl['location'];
    $bfl_roles['band'] = $request_bfl['band'];
    $flag_page = _targeting_value_in_object($bfl_roles, $wrapper);
    if ($flag_page) {
      $role_tids = array();
      $urls = field_get_items('node', $node, 'field_urls', $node->language);
      foreach ($urls as $url) {
        $fc = field_collection_field_get_entity($url);
        $wrapper = entity_metadata_wrapper('field_collection_item', $fc);
        // all values from a field collection
        $field_admin_title = $wrapper->field_admin_title->value();
        $field_roles = $wrapper->field_self_service_roles->value(); // an array of image data
        $field_url = $wrapper->field_url->value();
        $url_type = $wrapper->field_url_type->value();
        $result = _targeting_value_in_object($bfl_roles, $wrapper);
        if ($result) {
          $params['admin_title'] = $field_admin_title;
          $params['url'] = $field_url['url'];
          $params['url_type'] = $url_type->name;
          break;
        }
        $role_tids = array();
      }
    }
  }
  if ($flag_page) {
    $params['node'] = $params;
    print theme('url_json', $params);
  }
  else {
    $error_code = 'fail';
    $error_message = 'No targeting roles';
    $params['error']['error_code'] = $error_code;
    $params['error']['error_messsage'] = $error_message;
    print theme('error_json', $params);
    return;
  }
}

/* To find any matches for queried role/b/f/l */

function _find_any_matches_json($tids, $bfl_role_tids) {
  if (!empty($tids)) {
    foreach ($tids as $k => $v) {
      if (isset($bfl_role_tids[$v])) {
        return TRUE;
      }
    }
  }
  return FALSE;
}

/* function to get page content in json format */

function _get_page_content_json($request_roles, $node, $country_tid, $request_bfl, $msid, $show_preview,$country) {
  $params = _get_page_content($request_roles, $node, $country_tid, $request_bfl, $msid, $show_preview,$country);
  if (empty($params)) {
    $error_code = 'fail';
    $error_message = 'No targeting roles and business and function and location';
    $params['error']['error_code'] = $error_code;
    $params['error']['error_messsage'] = $error_message;
    print theme('error_json', $params);
    return;
  }
  else {
    print theme('generic_json', $params);
  }
}

/* function to get page content in array format */

function _get_page_content($request_roles, $node, $country_tid, $request_bfl, $msid, $show_preview,$country) {
  $flag_page = FALSE;
  $tids = array();
  $params = array();
  $roles = array_values(_get_tids_portal($request_roles, $country_tid));
  if (!empty($roles)) {
    $content_type = $node->type;
    try {
      $wrapper = entity_metadata_wrapper('node', $node);
    }
    catch (EntityMetadataWrapperException $e) {
      watchdog_exception('services', $e);
      _print_errors('Fail', 'Error in wrapper,please logs in CMS,some module/file is missing');
    }

    $content_type = $node->type;
    $bfl_roles['roles'] = $roles;
    $bfl_roles['business'] = $request_bfl['business'];
    $bfl_roles['function'] = $request_bfl['function'];
    $bfl_roles['location'] = $request_bfl['location'];
    $bfl_roles['band'] = $request_bfl['band'];
    $flag_page = _targeting_value_in_object($bfl_roles, $wrapper);

    //targeting at page level
    //  $flag_page =  _targeting_value_in_object($tids,$wrapper);

    if ($flag_page) {
      $type = $node->type;
      $params['country']=$country;
      $params['id'] = $node->nid;
      $params['type'] = $type;
      $params['node']['title'] = $node->title;
      $image_banner = $wrapper->field_image_banner->value();
      $image_banner_style = isset($wrapper->field_image_banner_types) ? $wrapper->field_image_banner_types->value() : NULL;
      $image_banner_size = isset($wrapper->field_image_banner_size) ? $wrapper->field_image_banner_size->value() : NULL;
      $banner_video_code = isset($wrapper->field_banner_video_code) ? $wrapper->field_banner_video_code->value() : NULL;
      $params['node']['banner_video_code'] = $banner_video_code;
      $params['node']['image_banner'] = $image_banner['uri'];
      $params['node']['image_banner_style'] = isset($image_banner_style->field_system_code['und'][0]['value']) ? $image_banner_style->field_system_code['und'][0]['value'] : $image_banner_style->name;
      $params['node']['image_banner_size'] = isset($image_banner_size->field_system_code['und'][0]['value']) ? $image_banner_size->field_system_code['und'][0]['value'] : $image_banner_size->name;
      $image_alignment = $wrapper->field_body_image_alignment->value();
      $menu = $wrapper->field_menu->value();

      if (!empty($image_alignment)) {
        $params['node']['image_alignment'] = isset($image_alignment->field_system_code['und'][0]['value']) ? $image_alignment->field_system_code['und'][0]['value'] : $image_alignment->name;
      }

      $image_type = $wrapper->field_body_image_type->value();
      if (!empty($image_type)) {
        $params['node']['image_type'] = isset($image_type->field_system_code['und'][0]['value']) ? $image_type->field_system_code['und'][0]['value'] : $image_type->name;
      }
      $image = $wrapper->field_body_image->value();
      if (!empty($image)) {
        $params['node']['image'] = $image['uri'];
      }
      $group_style = isset($wrapper->field_group_style) ? $wrapper->field_group_style->value() : NULL;
      if (!empty($group_style)) {
        $params['node']['group_style'] = isset($group_style->field_system_code['und'][0]['value']) ? $group_style->field_system_code['und'][0]['value'] : $group_style->name;

        if (strcasecmp($group_style->name, 'carousel') == 0) {
          $params['node']['carousel_interval'] = isset($wrapper->field_carousel_interval) ? $wrapper->field_carousel_interval->value() : NULL;
          $carousel_settings = isset($wrapper->field_carousel_settings) ? $wrapper->field_carousel_settings->value() : NULL;
          $params['node']['carousel_settings'] = isset($carousel_settings->name) ? $carousel_settings->name : NULL;
        }
      }

      $params['node']['body'] = $node->body['und'][0]['value'];
      $params['node']['summary'] = $node->body['und'][0]['summary'];
      $values = $wrapper->field_asset->raw();
      $asset = array();
      foreach ($values as $value) {
        $asset[] = $value['uri'];
      }
      $groups = array();
      $params['node']['asset'] = $asset;
      /* Added on 10/30 new changes for page design and classes */
      $page_design = isset($wrapper->field_design_settings) ? $wrapper->field_design_settings->value() : NULL;
      if (!empty($page_design)) {
        $params['node']['page_design'] = isset($page_design->field_system_code['und'][0]['value']) ? $page_design->field_system_code['und'][0]['value'] : NULL;
      }
      $params['node']['page_css_class_names'] = isset($wrapper->field_css_class_names) ? $wrapper->field_css_class_names->value() : NULL;
      /* end of code 10/30 */
      /* get the groups from the content type */
      $field_collection = field_get_items('node', $node, 'field_group', $node->language);
      if ($field_collection) {
        foreach ($field_collection as $collection) {
          $group = field_collection_field_get_entity($collection);
          $wrapper = entity_metadata_wrapper('field_collection_item', $group);
          $result = _targeting_value_in_object($bfl_roles, $wrapper);
          /* if any matches found */
          if ($result) {
            $groups = _get_group_content_json($wrapper, $group, $bfl_roles, $params);
            $params['groups'][] = $groups;
          }
          $group_role_tids = array();
        }
      }
    }
  }

  if ($flag_page) {
    $sidebarnodes = _get_section_nodes($menu->tid, $country_tid, $bfl_roles, $node->language, $type = 'section', $show_preview);
    if (!empty($sidebarnodes)) {
      $params['sidebar'] = _get_sidebar_values($sidebarnodes, $bfl_roles);
    }
    if ((is_null($msid))) {
      $result = _get_multi_site_id($node->nid);
      $msid = $result[0]->entity_id;
    }

    if (isset($msid)) {
      $lang = $node->language;
      $msid = _get_tanslated_node($msid, $lang);
      $multisite_node = node_load($msid);
      if ($show_preview) {
        $previous_vid = _get_current_revision_id($multisite_node);
        if (isset($previous_vid)) {
          $multisite_node = node_load($msid, $previous_vid);
        }
      }

      if (!empty($multisite_node) && ($multisite_node->type == "multisite")) {
        $multisite_content = _get_multisite_json($request_roles, $multisite_node, $country_tid, $request_bfl, $msid, $show_preview);
        if ($multisite_content) {
          $params['multisite'] = $multisite_content;
        }
      }
    }

    $params['content'] = $params;
    return $params;
  }
}

/* Function to get group fieldcollection item */

function _get_group_content_json($wrapper, $group, $bfl_roles, $params) {
  // $wrapper = entity_metadata_wrapper('field_collection_item', $group);
  $field_title = $wrapper->field_title->value();
  $field_admin_title = $wrapper->field_admin_title->value();
  $params['group']['id'] = $wrapper->item_id->value();
  $params['group']['admin_title'] = $field_admin_title;
  $params['group']['title'] = $field_title;
  $values = $wrapper->field_asset->raw();
  $asset = array();
  foreach ($values as $value) {
    $asset[] = $value['uri'];
  }
  $params['group']['asset'] = $asset;
  $body = $wrapper->field_body->value();
  $params['group']['body'] = ($body['value']);
  $params['group']['summary'] = $body['summary'];
  /* Added on 10/30 new changes for page design and classes */
  $group_design = isset($wrapper->field_design_settings) ? $wrapper->field_design_settings->value() : NULL;
  if (!empty($group_design)) {
    $params['group']['group_design'] = isset($group_design->field_system_code['und'][0]['value']) ? $group_design->field_system_code['und'][0]['value'] : NULL;
  }
  $params['group']['group_css_class_names'] = isset($wrapper->field_css_class_names) ? $wrapper->field_css_class_names->value() : NULL;
  /* end of code 10/30 */
  $image_alignment = $wrapper->field_body_image_alignment->value();
  if (!empty($image_alignment)) {
    $params['group']['image_alignment'] = isset($image_alignment->field_system_code['und'][0]['value']) ? $image_alignment->field_system_code['und'][0]['value'] : $image_alignment->name;
  }

  $image_type = $wrapper->field_body_image_type->value();
  if (!empty($image_type)) {
    $params['group']['image_type'] = isset($image_type->field_system_code['und'][0]['value']) ? $image_type->field_system_code['und'][0]['value'] : $image_type->name;
  }
  $image = $wrapper->field_body_image->value();
  if (!empty($image)) {
    $params['group']['image'] = $image['uri'];
  }
  $section_style = isset($wrapper->field_section_type) ? $wrapper->field_section_type->value() : NULL;
  if (!empty($section_style)) {
    $params['group']['section_style'] = isset($section_style->field_system_code['und'][0]['value']) ? $section_style->field_system_code['und'][0]['value'] : $section_style->name;
    if (strcasecmp($section_style->name, 'carousel') == 0) {
      $params['group']['carousel_interval'] = isset($wrapper->field_carousel_interval) ? $wrapper->field_carousel_interval->value() : NULL;
      $carousel_settings = isset($wrapper->field_carousel_settings) ? $wrapper->field_carousel_settings->value() : NULL;
      $params['group']['carousel_settings'] = isset($carousel_settings->name) ? $carousel_settings->name : NULL;
    }
  }
  /* Get all sections in the fieldcollection */
  $field_collection = field_get_items('field_collection_item', $group, 'field_section');
  if ($field_collection) {
    foreach ($field_collection as $collection) {
      $section = field_collection_field_get_entity($collection);
      $wrapper = entity_metadata_wrapper('field_collection_item', $section);
      $field_role = $wrapper->field_self_service_roles->value(); // an array of image data
      $result = _targeting_value_in_object($bfl_roles, $wrapper);
      if ($result) {
        $params['group']['section'][] = _get_section_content_json($bfl_roles, $section);
      }
      $section_role_tids = array();
    }
  }
  return $params['group'];
}

/* get the section values from fieldcollection item */

function _get_section_content_json($bfl_roles, $section) {

  $wrapper = entity_metadata_wrapper('field_collection_item', $section);
  $params['id'] = $wrapper->item_id->value();
  $params['field_title'] = $wrapper->field_title->value();
  $field_section_layout = $wrapper->field_section_layout->value();
  if (!empty($field_section_layout)) {
    $params['field_section_layout'] = isset($field_section_layout->field_system_code['und'][0]['value']) ? $field_section_layout->field_system_code['und'][0]['value'] : $field_section_layout->name;
  }
  $image_alignment = $wrapper->field_body_image_alignment->value();
  if (!empty($image_alignment)) {
    $params['image_alignment'] = isset($image_alignment->field_system_code['und'][0]['value']) ? $image_alignment->field_system_code['und'][0]['value'] : $image_alignment->name;
  }
  $image_type = $wrapper->field_body_image_type->value();
  if (!empty($image_type)) {
    $params['image_type'] = isset($image_type->field_system_code['und'][0]['value']) ? $image_type->field_system_code['und'][0]['value'] : $image_type->name;
  }
  $image = $wrapper->field_body_image->value();
  if (!empty($image)) {
    $params['image'] = $image['uri'];
  }
  $body = $wrapper->field_body->value();
  $params['body'] = ($body['value']);
  $params['summary'] = ($body['summary']);
  $values = $wrapper->field_asset->raw();
  $asset = array();
  foreach ($values as $value) {
    $asset[] = $value['uri'];
  }
  $params['section_asset'] = $asset;
  /* Added on 10/30 new changes for page design and classes */
  $section_design = isset($wrapper->field_design_settings) ? $wrapper->field_design_settings->value() : NULL;
  if (!empty($section_design)) {
    $params['section_design'] = isset($section_design->field_system_code['und'][0]['value']) ? $section_design->field_system_code['und'][0]['value'] : NULL;
  }
  $params['section_css_class_names'] = isset($wrapper->field_css_class_names) ? $wrapper->field_css_class_names->value() : NULL;

  $section_type = isset($wrapper->field_section_type_list) ? $wrapper->field_section_type_list->value() : NULL;
  $params['section_type'] = isset($section_type->field_system_code['und'][0]['value']) ? $section_type->field_system_code['und'][0]['value'] : NULL;
  $params['section_ids'] = isset($wrapper->field_section_ids) ? $wrapper->field_section_ids->value() : NULL;
  /* end of code 10/30 */
  $link_style = isset($wrapper->field_link_style) ? $wrapper->field_link_style->value() : NULL;

  if (!empty($link_style)) {
    $params['link_style'] = isset($link_style->field_system_code['und'][0]['value']) ? $link_style->field_system_code['und'][0]['value'] : $link_style->name;
    if (strcasecmp($link_style->name, 'carousel') == 0) {
      $params['carousel_interval'] = isset($wrapper->field_carousel_interval) ? $wrapper->field_carousel_interval->value() : NULL;
      $carousel_settings = isset($wrapper->field_carousel_settings) ? $wrapper->field_carousel_settings->value() : NULL;
      $params['carousel_settings'] = isset($carousel_settings->name) ? $carousel_settings->name : NULL;
    }
  }
  $fieldcollection = field_get_items('field_collection_item', $section, 'field_link_type_new');
  $params_link_files = _get_link_files_content_json($bfl_roles, $fieldcollection);
  $params['section_links_files'] = $params_link_files['section_links_files'];
  return $params;
}

function _get_link_files_content_json($bfl_roles, $field_collection) {
  if ($field_collection) {

    $params_link_files = array();
    foreach ($field_collection as $collection) {
      $fc = field_collection_field_get_entity($collection);
      $wrapper = entity_metadata_wrapper('field_collection_item', $fc);
      $section_link_type = $wrapper->field_link_type->value();
      if (!empty($section_link_type)) {
        $section_link_type = isset($section_link_type->field_system_code['und'][0]['value']) ? $section_link_type->field_system_code['und'][0]['value'] : $section_link_type->name;;
      }
      $title = $wrapper->field_title;
      $window_type = $wrapper->field_window_types->value();
      $field_role = $wrapper->field_self_service_roles->value();
      $image = '';
      $toenter = array();
      $result = FALSE;

      /* targeting at link level level */
      $result = _targeting_value_in_object($bfl_roles, $wrapper);
      if ($result) {
        /* Added on 10/30 new changes for page design and classes */
        $icon_location = isset($wrapper->field_icon_location_values) ? $wrapper->field_icon_location_values->value() : NULL;
        $icon_name=isset($wrapper->field_icon_name) ? $wrapper->field_icon_name->value() : NULL;
        $toenter['icon_location'] = isset($icon_location->field_system_code['und'][0]['value']) ? $icon_location->field_system_code['und'][0]['value'] : NULL;
        $toenter['icon_name'] = isset($icon_name->field_system_code['und'][0]['value']) ? $icon_name->field_system_code['und'][0]['value'] : NULL;
        /* end of code 10/30 */
        $toenter['id'] = $wrapper->item_id->value();
        if (strcmp($section_link_type, 'existing url') == 0) {
          $section_existing_url = $wrapper->field_existing_url->value();
          if (!empty($section_existing_url)) {
            foreach ($section_existing_url as $url) {
              $toenter['type'] = 'existing_url';
              $toenter['nid'] = $url->nid;
              $toenter['title'] = $url->title;
              $window_type = $wrapper->field_window_types->value();
              $toenter['window_type'] = $window_type->name;
              $toenter['window_type_code'] = $window_type->field_system_code[LANGUAGE_NONE][0]['safe_value'];
              $image = isset($wrapper->field_image) ? $wrapper->field_image->value() : NULL;
              if (!empty($image)) {
                $toenter['image'] = $image['uri'];
              }
              $params_link_files['section_links_files'][] = $toenter;
            }
          }
        }
        if (strcmp($section_link_type, 'link') == 0) {
          $section_link = $wrapper->field_link->value();
          $toenter['type'] = 'link';
          $toenter['url'] = $section_link['url'];
          $toenter['title'] = $section_link['title'];
          $window_type = $wrapper->field_window_types->value();
          $toenter['window_type'] = $window_type->name;
          $toenter['window_type_code'] = $window_type->field_system_code[LANGUAGE_NONE][0]['safe_value'];
          $image = isset($wrapper->field_image) ? $wrapper->field_image->value() : NULL;
          if (!empty($image)) {
            $toenter['image'] = $image['uri'];
          }
          $params_link_files['section_links_files'][] = $toenter;
        }
        if (strcmp($section_link_type, 'files') == 0) {
          $section_file = $wrapper->field_file->value();
          if (!empty($section_file)) {
            foreach ($section_file as $file) {
              $toenter['type'] = 'files';
              $toenter['file'] = $file['uri'];
              $toenter['title'] = $file['description'];
              $window_type = $wrapper->field_window_types->value();
              $toenter['window_type'] = $window_type->name;
              $toenter['window_type_code'] = $window_type->field_system_code[LANGUAGE_NONE][0]['safe_value'];
              $image = isset($wrapper->field_image) ? $wrapper->field_image->value() : NULL;
              if (!empty($image)) {
                $toenter['image'] = $image['uri'];
              }
              $params_link_files['section_links_files'][] = $toenter;
            }
          }
        }
        if (strcmp($section_link_type, 'text') == 0) {
          $section_link = $wrapper->field_link->value();
          $toenter['type'] = 'text';
          $toenter['url'] = $section_link['url'];
          $description = $wrapper->field_description->value();
          $toenter['description'] = $description['value'];
          $toenter['title'] = $wrapper->field_title->value();
          $window_type = $wrapper->field_window_types->value();
          $toenter['window_type'] = $window_type->name;
          $toenter['window_type_code'] = $window_type->field_system_code[LANGUAGE_NONE][0]['safe_value'];
          $image = isset($wrapper->field_image) ? $wrapper->field_image->value() : NULL;
          if (!empty($image)) {
            $toenter['image'] = $image['uri'];
          }
          $params_link_files['section_links_files'][] = $toenter;
        }
          if ((strcmp($section_link_type, 'startgroup') == 0)||(strcmp($section_link_type, 'endgroup') == 0)) {
          $section_link = $wrapper->field_link->value();
          $toenter['type'] = $section_link_type;
          $toenter['title'] = $wrapper->field_title->value();
          $window_type = $wrapper->field_window_types->value();
          $toenter['window_type'] = $window_type->name;
          $toenter['window_type_code'] = $window_type->field_system_code[LANGUAGE_NONE][0]['safe_value'];
          $params_link_files['section_links_files'][] = $toenter;
        }
      }
    }
  }
  return $params_link_files;
}

function taxonomy_get_term_by_name_and_vocabulary($name, $vid) {
  return taxonomy_term_load_multiple(array(), array('name' => trim($name), 'vid' => $vid));
}

/* Function to get tids of terms */

function _get_tids_portal($terms_tax, $country_tid) {
  $tids = array();
  $tid_voc = taxonomy_vocabulary_machine_name_load('self_service_roles');
  $taxonomnyquery = new EntityFieldQuery();
  if (!empty($terms_tax)) {
    $terms = $taxonomnyquery->entityCondition('entity_type', 'taxonomy_term')
        ->propertyCondition('vid', $tid_voc->vid)
        ->propertyCondition('name', (array) $terms_tax, 'IN')
        ->propertyOrderBy('tid', 'DESC')
        ->execute();
  }
  if (isset($terms['taxonomy_term'])) {
    $tids = array_keys($terms['taxonomy_term']);
  }
  return $tids;
}

/* Function to get roles from field basically services roles */

function _get_field_roles_json($field_roles) {
  foreach ($field_roles as $role) {
    $role_tids[] = $role->tid;
  }
  return $role_tids;
}

/* get translated node from source node id */

function _get_tanslated_node($nid, $lang) {
  $translations = translation_node_get_translations($nid);
  if (is_array($translations)) {
    foreach ($translations as $tnode) {
      if (strcmp($tnode->language, $lang) == 0) {
        $nid = $tnode->nid;
      }
    }
  }
  return $nid;
}

function _get_global_tid() {
  $global_country_tid = variable_get('global_country_tid');
  if (!isset($global_country_tid)) {
    $taxs = taxonomy_get_term_by_name('GBL', 'country');
    if (!empty($taxs)) {
      foreach ($taxs as $tax) {
        $global_country_tid = $tax->tid;
      }
    }
  }
  return $global_country_tid;
}

function _get_preview_server($nid, $previous_vid) {
  $preview_server = FALSE;
  $portal_preview = check_plain($_SERVER["HTTP_PORTALPREVIEW"]);
  $preview_node = node_load($nid, $previous_vid);
  if (!empty($preview_node) && !empty($portal_preview)) {
    if (isset($preview_node->field_projectname)) {
      $project_node = node_load($preview_node->field_projectname[LANGUAGE_NONE][0]['target_id']);
	  $preview_on = $project_node->field_preview_on[LANGUAGE_NONE];
	 foreach ($preview_on as $item) { 
     $preview_on_tids[] = $item['tid'];
	 }
     $taxonomies = taxonomy_term_load_multiple($preview_on_tids);
	 foreach ($taxonomies as $tax) {
        if ((strcmp($tax->name, $portal_preview) == 0)) {
          $preview_server = TRUE;
        }
      }
    }
  }
  return $preview_server;
}

function _get_current_revision_id($current_node) {
  $previous_vid = FALSE;
  if (isset($current_node->workbench_moderation['current']->vid)) {
    if ($current_node->vid != $current_node->workbench_moderation['current']->vid) {
      $previous_vid = $current_node->workbench_moderation['current']->vid;
    }
  }
  return $previous_vid;
}

/* Get single node-services */

function _get_news_article_single_node_content_json($request_roles, $node, $country_tid, $bfl) {
  $tids['business'] = $bfl['business'];
  $tids['function'] = $bfl['function'];
  $tids['location'] = $bfl['location'];
  $tids['band'] = $bfl['band'];
  $tids['roles'] = _get_tids_portal($request_roles, $country_tid);
  $wrapper = entity_metadata_wrapper('node', $node);
  //$page_level_targeting = $node->field_self_service_roles[LANGUAGE_NONE];
  $page_level_roles = $wrapper->field_self_service_roles->value();

  $flag_page = _targeting_value_in_object($tids, $wrapper);


  if ($flag_page == TRUE) {
    $data['node'] = _get_news_article_single_node_content($tids, $node, $country_tid,$wrapper);
    if (!empty($data)) {
      print theme('article_json', $data);
    }
  }

  if (empty($data)) {
    _print_errors('Empty data', 'Data is empty');
    return;
  }
  if ($flag_page == FALSE) {
    _print_errors('No targeting roles', 'No Targeting roles at page level');
    return;
  }
}

/* Content for news article single node */

function _get_news_article_single_node_content($tids, $node, $country_tid,$wrapper) {
  $data = array();
  if (empty($node)) {
    return $data;
  }

  $node_businesses= isset($wrapper->field_business) ? _get_node_details($wrapper->field_business->value(),'business') : array();
  $node_locations= isset($wrapper->field_location) ? _get_node_details($wrapper->field_location->value(),'location') : array();
 
  $data['id'] = $node->nid;
  $data['node_title'] = $node->title;
  $data['image'] = $node->field_image[LANGUAGE_NONE][0]['uri'];
  $data['top_menu'] = $node->field_top_menu[LANGUAGE_NONE][0]['value'];
  $data['effective_date'] = _get_date($node->field_live_date[LANGUAGE_NONE][0]['value']);
  $data['published_date'] = _get_date($node->field_published_date[LANGUAGE_NONE][0]['value']);
  $data['end_date'] = _get_date($node->field_news_end_date[LANGUAGE_NONE][0]['value']);
  $data['description'] = $node->field_description[LANGUAGE_NONE][0]['value'];
  $data['node_locations']['location_name'] =array_keys($node_locations);
  $data['node_businesses']['business_name'] =array_keys($node_businesses);
  $data['node_locations_count'] =count($node_locations);
  $data['node_businesses_count']=count($node_businesses);
  $field_collection = field_get_items('node', $node, 'field_link_type_new');
  $params_links_files = _get_link_files_content_json($tids, $field_collection);
  $data['links_files'] = $params_links_files['section_links_files'];
  return $data;
}

function array_remove_null($array) {
  foreach ($array as $key => $value) {
    if (is_null($value)) {
      unset($array[$key]);
    }
    if (is_array($value)) {
      $array[$key] = array_remove_null($value);
    }
  }
  return $array;
}

function _get_country_details($country, $check) {
  $value = '';
  $country_data = cache_get('country_data' . $country . '_' . $check);
  $value = $country_data->data;
  if (empty($value)) {
    $voc = taxonomy_vocabulary_machine_name_load('country');
    $taxs = taxonomy_term_load_multiple(array(), array('name' => $country, 'vid' => $voc->vid));
    if (!empty($taxs)) {
      foreach ($taxs as $key => $tax) {
        if (strcmp($check, 'tid') == 0) {
          $value = $tax->tid;
          cache_set('country_data' . $country . '_' . $check, $value, 'cache', CACHE_PERMANENT);
        }
        elseif (strcmp($check, 'lang') == 0) {
          $value = $tax->field_default[LANGUAGE_NONE][0]['tid'];
          if ($value) {
            $lang = taxonomy_term_load($value);
            if ($lang) {
              $value = $lang->name;
              cache_set('country_data' . $country . '_' . $check, $value, 'cache', CACHE_PERMANENT);
            }
          }
        }
      }
    }
  }

  return $value;
}

function _get_date($unixtime) {
  $string = NULL;
  if (!empty($unixtime)) {
    $string = date('Y-m-d', $unixtime);
  }
  return $string;
}

function _get_vid($name) {
  $voc = taxonomy_vocabulary_machine_name_load($name);
  if (!empty($voc)) {
    return $voc->vid;
  }
  return '';
}
