<?php
 drupal_add_http_header('Content-Type', 'application/json');
if (!empty($variables)) {
$var=$variables['error'];
$json=drupal_json_encode($var);
print preg_replace('/,\s*"[^"]+":null|"[^"]+":null,?/', '', $json);
}