<?php
echo "test start ", microtime(), "\n";
// var_dump(testFunction());

// $a = new testClass();
// var_dump($a->test());

wue_database_mysql::init([
	"master"=>[
		["host"=>"172.16.0.51", "port"=>3306, "user"=> "panda", "pass"=>"panda.tv", "db"=>"text"]
	],
]);

wue_database_mysql::get_master();