<?php

/**
 * @file
 * Provides moderation links for Views.
 */

class ge_user_section_handler_field_section_info extends views_handler_field{
   // No query for this field.
  function query() {}

  /**
   * Render the link.
   */
  function render($values) {
    $output = get_user_sections($values);
    return $output;
  }

}
