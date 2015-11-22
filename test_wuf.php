<?php
echo "test start ", microtime(), "\n";
$b = [
	"master"=>[
		["host"=>"192.168.56.101", "port"=>3306, "user"=> "root", "pass"=>"", "db"=>"test"]
	],
];

var_dump(testFunction($b));

$a = new testClass();

var_dump($a->test($b), testClass::$save);

wuf_database_mysql::init($b);
// $i = new mysqli("192.168.56.101", "wuhao", "", "test", 3306);
$m = wuf_database_mysql::get_master();

echo "format:", $m->format("SELECT * FROM `user` LIMIT ?", 2), "\n";
// echo "escape:", $i->escape_string("xx'xx"), "\n";

var_dump( $m->format_query("SELECT * FROM `user`"), $m->query("select * from `aaauser`") );

echo "error: [", isset($m->error),"] ", $m->error, "\n";

echo "done.\n";

$m->update("table", ["a"=>"b", "c"=>"d"]);
$m->update("table", ["a"=>"b", "c"=>"d"], ["a"=>"b", "c[!]"=>"d", "e[<>]"=>[100, 200], "OR"=>["f[><]"=>[300, 400], "g[~]"=>"pre_%"]]);
$m->insert("table", ["a"=>"b", "c"=>"d", "e"=>"f"]);

echo "error: [", isset($m->error),"] ", $m->error, "\n";

exit(0);