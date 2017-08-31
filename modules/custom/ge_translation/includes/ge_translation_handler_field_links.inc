<?php

/**
 * @file
 * Provides moderation links for Views.
 */

class ge_translation_handler_field_links extends views_handler_field {
  function render($values) {
    if ($values->{$this->aliases['current']}) {
      $node = node_load($values->{$this->aliases['nid']}, $values->{$this->aliases['vid']});
      return theme('item_list', array('items' => ge_get_translation_links($node, array('query' => array('destination' => $_GET['q'])))));
    }
    return '';
  }
}
