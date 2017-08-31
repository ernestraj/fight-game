<?php
require_once('DatumboxAPI.php');

$api_key='21ca304b358ae2eaca9dc24ba3a98816'; //To get your API visit datumbox.com, register for an account and go to your API Key panel: http://www.datumbox.com/apikeys/view/

$DatumboxAPI = new DatumboxAPI($api_key);

$tweet="I love the new Datumbox API! :)";
$DocumentClassification['TwitterSentimentAnalysis']=$DatumboxAPI->SentimentAnalysis($tweet);

//Example of using Information Retrieval API Functions
$InformationRetrieval=array();

$url='http://localhost/nlp/google/google.html';
$html=file_get_contents($url);

$InformationRetrieval['TextExtraction']=$DatumboxAPI->TextExtraction($html);
$InformationRetrieval['KeywordExtraction']=$DatumboxAPI->KeywordExtraction($InformationRetrieval['TextExtraction'],5);

unset($DatumboxAPI);

echo '<h1>Information Retrieval</h1>';
echo '<pre>';
print_r($InformationRetrieval);
echo '</pre>';

