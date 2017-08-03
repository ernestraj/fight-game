<?php

$input = array();

//Reading file
$file = fopen("input.txt", "r") or die("Unable to open file!");
while(! feof($file)) {
  $input[] = fgets($file);
}
fclose($file);

//Sanitizing input
$dimensions = $input[0];
$ships = $input[1];
$p1_input = 2+(int)$ships;
$p2_input = 2+(int)$ships+1;
$player1 = $input[$p1_input];
$player2 = $input[$p2_input];

//Getting ships
for($i = 2; $i < 2+(int)$ships; $i++) {
  $ship[] = $input[$i];
}

//Size of battle field
$dim = explode(" ", $dimensions);

//Extracting ship features
foreach($ship as $item) {
  $ship_features[] = explode(" ", $item);
}

//Storing each user missiles
$player1_missiles = explode(" ", $player1);
$player2_missiles = explode(" ", $player2);

//Plotting battle area
$traverse_array = array();
$battle_area = array();
for($i = 'A'; $i <= $dim[1]; $i++) {
  for($j = 1; $j <= $dim[0]; $j++) {
    $battle_area[trim($i) . trim($j)] = $i . $j;
	$traverse_array[$i.$j] = $i . '-' . $j;
  }
}

//Get player ship features
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

//Drawing each player's battle field
$player2_battle_area = get_player_battle_area($battle_area, $player2_ship, $traverse_array, $dim);
$player1_battle_area = get_player_battle_area($battle_area, $player1_ship, $traverse_array, $dim);

//battle begins
battle1($player1_battle_area, $player1_missiles, $player2_battle_area, $player2_missiles);

//Battle function for player1
function battle1($battle_area1, $missiles1, $battle_area2, $missiles2) {
    if(!empty($missiles1)) {
      foreach($missiles1 as $key=>$item) {
        if((isset($battle_area2[$item]) && $battle_area2[$item] == "Q")) {
          $battle_area2[$item] = 'P';
          unset($missiles1[$key]);
          print 'Player-1 fires a missile with target ' . trim($item) . ' which got hit' . '<br/>';
        }
        elseif((isset($battle_area2[$item]) && $battle_area2[$item] == 'P')) {
          $battle_area2[$item] = 'X';
          unset($missiles1[$key]);
          print 'Player-1 fires a missile with target ' . trim($item) . ' which got hit' . '<br/>';
		  declare_results($battle_area1, $battle_area2);
        }
        else {
          print 'Player-1 fires a missile with target ' . trim($item) . ' which got miss' . '<br/>';
          unset($missiles1[$key]);
          battle2($battle_area1, $missiles1, $battle_area2, $missiles2);
        }
      }
    }
    else {
      print 'Player-1 has no more missiles left to launch' . '<br/>';
      if(empty($missiles1) && empty($missiles2)) {
        print "Battle resulted in a draw";
		exit;
      }
      battle2($battle_area1, $missiles1, $battle_area2, $missiles2);
    }
}

//Battle function for player 2
function battle2($battle_area1, $missiles1, $battle_area2, $missiles2) {
    if(!empty($missiles2)) {
      foreach($missiles2 as $key=>$item) {
        if((isset($battle_area1[$item]) && $battle_area1[$item] == "Q")) {
          $battle_area1[$item] = 'P';
          unset($missiles2[$key]);
          print 'Player-2 fires a missile with target ' . trim($item) . ' which got hit' . '<br/>';
        }
        elseif((isset($battle_area1[$item]) && $battle_area1[$item] == 'P')) {
          $battle_area1[$item] = 'X';
          unset($missiles2[$key]);
          print 'Player-2 fires a missile with target ' . trim($item) . ' which got hit' . '<br/>';
		  declare_results($battle_area1, $battle_area2);
        }
        else {
          print 'Player-2 fires a missile with target ' . trim($item) . ' which got miss' . '<br/>';
          unset($missiles2[$key]);
          battle1($battle_area1, $missiles1, $battle_area2, $missiles2);
        }
      }
    }
    else {
      print 'Player-2 has no more missiles left to launch' . '<br/>';
      if(empty($missiles1) && empty($missiles2)) {
        print "Battle resulted in a draw";
        exit;		
      }
      battle1($battle_area1, $missiles1, $battle_area2, $missiles2);
    }
}

//Checking if any player destroyed other player's all ships
function declare_results($battle_area1, $battle_area2) {
  if(!in_array("P", $battle_area1) && !in_array("Q", $battle_area1)) {
    print "Player2 won the battle";
	exit;
  }
  elseif(!in_array("P", $battle_area2) && !in_array("Q", $battle_area2)) {
    print "Player1 won the battle";
	exit;
  }
}

//Plotting each player's battle area
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

//get position for each ship to be drawn in battle area
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

?>