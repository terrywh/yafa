<?php
echo "test start ", microtime(), "\n";
$mysql_config = [
	"master"=>[
		// ["host"=>"192.168.56.1", "port"=>3306, "user"=> "root", "pass"=>"", "db"=>"mysql"]
		["host"=>"172.16.0.251", "port"=>3306, "user"=> "wuhao", "pass"=>"mysqlwuhao!", "db"=>"test"]
	],
];
$redis_config = [
	"master"=>[
		["host"=>"172.16.0.251", "port"=>14000, "auth"=> "panda.tv@2015", "prefix"=>"test_"],
	],
];
$ssdb_config = [
	"master"=>[
		["host"=>"172.16.0.251", "port"=>8888, "prefix"=>"test_"],
	],
];


// var_dump("testFunction:", testFunction($mysql_config));

// $a = new testClass();
// echo "test -----------------\n";
// var_dump($a->test($mysql_config));
// var_dump($a->prop1, $a->prop2,
// 	testClass::$save);

// yafa_database_mysql::init($mysql_config);
// $m = yafa_database_mysql::get_master();
// echo "format: ", $m->format("SELECT * FROM `user` LIMIT ?", 2), "\n";
// echo "escape:", $i->escape_string("xx'xx"), "\n";
// $table = "admin_project_info";
// $start = 0; $max_page_list = 10;
// $params = array(
// 	'field' => '*',
// 	'cond' => array('isdel' => 0, 'fpid' => 0,),
// 	'group' => null,
// 	'order' => "`pid` DESC",
// 	'limit' => array($start, $max_page_list),
// );
// debug_zval_dump($m);
// var_dump( $m->select($table, $params["field"], $params["cond"], $params["group"], $params["order"], $params["limit"]) );
// var_dump( $m->query("select * from `aaauser`") );

// echo "error: [", isset($m->error),"] ", var_dump($m->error);
// echo "done.\n";

// echo $m->update("table", ["a"=>"b", "c"=>["c1"=>"xxx", "c2"=>"yyy"]]), "\n";
// echo $m->update("table", ["a"=>"b", "c"=>"d"], ["a"=>"b", "c[!]"=>"d", "e[<>]"=>[100, 200], "OR"=>["f[><]"=>[300, 400], "g[~]"=>"pre_%"]]), "\n";
// echo $m->insert("table", ["a"=>"b", "c"=>"d", "e"=>"f"]), "\n";
// echo $m->insert("table", [["a"=>"b"], ["a"=>"c"], ["a"=>"c"]]), "\n";

// echo "select1: ", $m->select("table", ["a","b"], ["b"=>"bbbb"], null, "`a`", [10, 50]), "\n";
// echo "select2: ", $m->select("table", "*", ["b[!]"=>"cccc"], ["a"], ["a"=>true, "b"=>"DESC"]), "\n";
// echo "remove: ", $m->remove("table", ["a"=>"aaaa"]), "\n";
// echo "one: ", $m->one("table", ["a"=>"aaaa"]), "\n";
// echo "error: [", isset($m->error),"] ", $m->error, "\n";

// yafa_database_redis::init($redis_config);
// $db = yafa_database_redis::get_master();
// var_dump($db);
// var_dump($db->get("abc"));
// var_dump($db->hgetall("hash"));
// var_dump($db->smembers("set"));

// var_dump( $db->multi()->get("abc")->hgetall("hash")->exec() );

// $c = 0;
// var_dump($db->scan($c));


yafa_database_ssdb::init($ssdb_config);
$db = yafa_database_ssdb::get_master();
var_dump($db);
var_dump($db->exists("abc"));
var_dump($db->get("abc"));
var_dump($db->hgetall("hash"));
var_dump($db->zscan("zset", "", "", "", 10));

exit(0);