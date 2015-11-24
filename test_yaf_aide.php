<?php
echo "test start ", microtime(), "\n";
$b = [
	"master"=>[
		// ["host"=>"192.168.56.101", "port"=>3306, "user"=> "root", "pass"=>"", "db"=>"test"]
		["host"=>"182.92.228.217", "port"=>3306, "user"=> "wuhao", "pass"=>"CrD5ivfNUBiFuLzf", "db"=>"test"]
	],
];

// var_dump(testFunction($b));

// $a = new testClass();

// var_dump($a->test($b), testClass::$save);

yafa_database_mysql::init($b);
// $i = new mysqli("192.168.56.101", "wuhao", "", "test", 3306);
$m = yafa_database_mysql::get_master();

echo "format: ", $m->format("SELECT * FROM `user` LIMIT ?", 2), "\n";
// echo "escape:", $i->escape_string("xx'xx"), "\n";

var_dump( $m->format_query("SELECT * FROM `user`"), $m->query("select * from `aaauser`") );

echo "error: [", isset($m->error),"] ", $m->error, "\n";

echo "done.\n";

echo $m->update("table", ["a"=>"b", "c"=>["c1"=>"xxx", "c2"=>"yyy"]]), "\n";
echo $m->update("table", ["a"=>"b", "c"=>"d"], ["a"=>"b", "c[!]"=>"d", "e[<>]"=>[100, 200], "OR"=>["f[><]"=>[300, 400], "g[~]"=>"pre_%"]]), "\n";
echo $m->insert("table", ["a"=>"b", "c"=>"d", "e"=>"f"]), "\n";

echo "select1: ", $m->select("table", ["a","b"], ["b"=>"bbbb"], null, "`a`", [10, 50]), "\n";
echo "select2: ", $m->select("table", "*", ["b[!]"=>"cccc"], ["a"], ["a"=>true, "b"=>"DESC"]), "\n";
echo "remove: ", $m->remove("table", ["a"=>"aaaa"]), "\n";
echo "one: ", $m->one("table", ["a"=>"aaaa"]), "\n";
echo "error: [", isset($m->error),"] ", $m->error, "\n";

exit(0);