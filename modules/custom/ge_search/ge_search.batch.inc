<?php

function search_batch_process($nid, &$context) { 
  //Perform the actual revision deletion.
  $indexed = ge_search_node_type($nid);

  $context['results'][$nid] = $nid;
}

/**
 * Batch finish function to be called
 */
function ge_search_build_batch_finished($success, $results, $operations) {
  if ($success) {
    // Here we could do something meaningful with the results.
    // We just display the number of data we processed...
    drupal_set_message(t('@count nodes  processed.', array('@count' => count($results))));
  } 
  else {
    // An error occurred.
    // $operations contains the operations that remained unprocessed.
    $error_operation = reset($operations);
    drupal_set_message(t('An error occurred while processing @operation with arguments : @args', array('@operation' => $error_operation[0], '@args' => print_r($error_operation[0], TRUE))));
  }
}

/**
 * Delete batch process
 */
function search_delete_batch_process($nid, &$context) {
  $indexed = ge_search_delete_records($nid);

  $context['results'][$nid] = $nid;
}

/**
 * delete batch process finish
 */
function ge_search_delete_batch_finished($success, $results, $operations) {
  if ($success) {
    // Here we could do something meaningful with the results.
    // We just display the number of data we processed...
    drupal_set_message(t('@count nodes  records deleted.', array('@count' => count($results))));
  } 
  else {
    // An error occurred.
    // $operations contains the operations that remained unprocessed.
    $error_operation = reset($operations);
    drupal_set_message(t('An error occurred while processing @operation with arguments : @args', array('@operation' => $error_operation[0], '@args' => print_r($error_operation[0], TRUE))));
  }
}