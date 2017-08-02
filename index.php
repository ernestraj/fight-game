<?php

$input = array();

$file = fopen("input.txt", "r") or die("Unable to open file!");
while(! feof($file)) {
  $input[] = fgets($file);
}
fclose($file);

$dimensions = $input[0];
$ships = $input[1];
$p1_input = 2+(int)$ships;
$p2_input = 2+(int)$ships+1;
$player1 = $input[$p1_input];
$player2 = $input[$p2_input];

for($i = 2; $i < 2+(int)$ships; $i++) {
  $ship[] = $input[$i];
}

$dim = explode(" ", $dimensions);

foreach($ship as $item) {
  $ship_features[] = explode(" ", $item);
}

$player1_hits = explode(" ", $player1);
$player2_hits = explode(" ", $player2);

print '<pre>';
//Plotting battle area
$traverse_array = array();
for($i = 'A'; $i <= $dim[1]; $i++) {
  for($j = 1; $j <= $dim[0]; $j++) {
    $battle_area[trim($i) . trim($j)] = $i . $j;
	$traverse_array[$i.$j] = $i . '-' . $j;
  }
}

$battle_area;

//get player 1 and player 2 ship positions
foreach($ship_features as $item) {
  $player1_ship[] = array(
    'type' => $item[0],
    'width' => $item[1],
    'height' => $item[2],
    'position_start' => $item[3],
  );
  $player2_ship[] = array(
    'type' => $item[0],
    'width' => $item[1],
    'height' => $item[2],
    'position_start' => $item[4],
  );
}
$i = 0;

$player2_battle_area = get_player_battle_area($battle_area, $player2_ship, $traverse_array, $dim);
$player1_battle_area = get_player_battle_area($battle_area, $player1_ship, $traverse_array, $dim);

print_r($player1_battle_area);
print_r($player2_battle_area);

function get_player_battle_area($area = array(), $player_ship, $traverse_array, $dim) {
  //collect ship positions for player
  foreach($player_ship as $item) {
    $plot = get_ship_position(trim($item['position_start']), $item['width'], $item['height'], $traverse_array, $dim);
	foreach($plot as $item1) {
      $area[trim($item1)] = $item['type'];
    }
  }

  return $area;
}

function get_ship_position($origin, $width, $height, $traverse_array, $dim) {
  $limit = explode('-', $traverse_array[$origin]);
  $max_width = $limit[1] + $width;
  $l = 0;
  for($k = $limit[0]; $k <= $dim[1]; $k++) {
    $l++;
	if($l == $height) {
	  $max_height = $k;
	  break;
	}
  }

  for($i = $limit[1]; $i < $max_width; $i++) {
    for($j = $limit[0]; $j <= $max_height; $j++) {
      $plot[$j . $i] = $j . $i;
    }
  }
  
  return $plot;
}

function get_current_area($area, $key) {
  while (key($area) != $key) next($area);
  return $area;
}

?>