<?php
echo "test start ", microtime(), "\n";
$b = [
	"master"=>[
		["host"=>"182.92.228.217", "port"=>3306, "user"=> "wuhao", "pass"=>"CrD5ivfNUBiFuLzf", "db"=>"fangxin8_core"]
	],
];

var_dump(testFunction($b));

$a = new testClass();

var_dump($a->test($b), testClass::$save);

wuf_database_mysql::init([
	"master"=>[
		["host"=>"182.92.228.217", "port"=>3306, "user"=> "wuhao", "pass"=>"CrD5ivfNUBiFuLzf", "db"=>"fangxin8_core"]
	],
]);
// $i = new mysqli("182.92.228.217", "wuhao", "CrD5ivfNUBiFuLzf", "fangxin8_core", 3306);
$m = wuf_database_mysql::get_master();

echo "format:", $m->format("SELECT * FROM `user` LIMIT ?", 2), "\n";
// echo "escape:", $i->escape_string("xx'xx"), "\n";

var_dump( $m->format_query("SELECT * FROM `user`"), $m->query("select * from `aaauser`") );

echo "error:", $m->error,"\n";
echo "done.\n";

exit(0);