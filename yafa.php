<?php
class yafa_database_mysql_where {
	protected $_sql;
	protected $_mysqli;
	public function __construct($mysqli, &$sql) {
		$this->_mysqli = $mysqli;
		$this->_sql = $sql;
	}
	public function build($c) {
		if(is_string($c)) {
			$this->_sql .= $c;
		}else if(!is_array($c)) {
			throw new Exception("conditions must be string or array");
		}
		$len = count($c);
		foreach($c as $key=>$val) {
			$this->append_item($key, $val);
			if(--$len != 0) {
				$this->_sql .= " AND ";
			}
		}
	}
	private function append_item($key, $val) {
		if($key[strlen($key)-1] == ']') {
			$pos = stripos($key, '[');
			$op  = substr($key, $pos + 1, strlen($key) - $pos - 2);
			$this->_sql .= "`";
			$this->_sql .= substr($key, 0, $pos);
			$this->_sql .= "`";
			if($op == "!") {
				if(is_null($val)) {
					$this->_sql .= " IS NOT ";
				}else if(is_array($val)) {
					$this->_sql .= " NOT IN ";
				}else{
					$this->_sql .= "!=";
				}
			}else if($op == ">" || $op == "<" || $op == ">=" || $op == "<=") {
				$this->_sql .= $op;
			}else if(op == "<>") {
				$this->_sql .= " BETWEEN ";
				$this->append_val($val[0]);
				$this->_sql .= " AND ";
				$this->append_val($val[1]);
				return;
			}else if(op == "><") {
				$this->_sql .= " NOT BETWEEN ";
				$this->append_val($val[0]);
				$this->_sql .= " AND ";
				$this->append_val($val[1]);
				return;
			}else if(op == "~") {
				$this->_sql .= " LIKE ";
			}else if(op == "!~") {
				$this->_sql .= " NOT LIKE ";
			}else{
				throw new Exception("operation \"".$op."\" not suppored");
			}
			$this->append_val($val);
		}else if($key == "AND") {
			$this->_sql .= "(";
			$len = count($val);
			foreach($val as $key=>$val) {
				$this->append_item($key, $val);
				if(--$len != 0) {
					$this->_sql .= " AND ";
				}
			}
			$this->_sql .= ")";
		}else if(key == "OR") {
			$this->_sql .= "(";
			$len = count($val);
			foreach($val as $key=>$val) {
				$this->append_item($key, $val);
				if(--$len != 0) {
					$this->_sql .= " OR ";
				}
			}
			$this->_sql .= ")";
		}else{ // `key`
			$this->_sql .= "`".$key."`";
			if(is_null($val)) { // IS NULL
				$this->_sql .= " IS ";
			}else if(val.is_type(IS_ARRAY)) {
				$this->_sql .= " IN ";
			}else{
				$this->_sql .= "=";
			}
			$this->append_val(val);
		}
	}

	private function append_val($val) {
		if(is_null($val)) { // IS NULL
			$this->_sql .= "NULL";
		}else if(is_array($val)) { // ('1','2','3')
			$this->_sql .= "(";
			$len = count($val);
			foreach($val as $v) {
				$this->append_val($v);
				if(--$len != 0) {
					$this->_sql .= ",";
				}
			}
			$this->_sql .= ")";
		}else if(is_bool($val)) {
			$this->_sql .= $val ? "true" : "false";
		}else if(is_long($val)) {
			$this->_sql .= $val;
		}else{ // 'val'
			$this->_sql .= "'".$this->_mysqli->escape_string($val)."'";
		}
	}
}

class yafa_database_mysql {
	protected static $_cache;
	protected static $_conf; 
	public static function init($conf) {
		if(empty($conf) || !is_array($conf)) {
			throw new Exception("mysql config of type array is required");
		}
		if(!isset($conf["master"]) && !isset($conf["slave"])) {
			throw new Exception("illegal mysql config #1");
		}
		self::$_conf = $conf;
		return true;
	}

	private static function create($conf) {
		$mysqli = new mysqli($conf["host"], $conf["user"], $conf["pass"], $conf["db"] ?? "", $conf["port"]);
		if(!empty($mysqli->connect_error)) {
			throw new Exception("failed to create mysqli");
		}
		return new yafa_database_mysql($mysqli);
	}

	public static function get_master($index = 0) {
		$key = "master_".$index;
		if(empty(self::$_cache[$key])) {
			$conf = self::$conf["master"][$index] ?? null;
			if(empty($conf) || !is_array($conf)) {
				throw new Exception("illegal mysql config #2");
			}
			self::$_cache[$key] = self::create($conf);
		}
		return self::$_cache[$key];
		
	}
	public static function get_slave() {
		$key = "slave_".$index;
		if(empty(self::$_cache[$key])) {
			$conf = self::$conf["master"][$index] ?? null;
			if(empty($conf) || !is_array($conf)) {
				throw new Exception("illegal mysql config #2");
			}
			self::$_cache[$key] = self::create($conf);
		}
		return self::$_cache[$key];
	}
	private $_mysqli;
	public function __construct($mysqli) {
		$this->_mysqli = $mysqli;
	}
	public function format($format, ...$argv) {
		$i = 0;
		$format = str_replace('?', function() use($argv, $i) {
			if(is_long($argv[$i])) {
				return (string)$argv[$i];
			}else if(is_bool($argv[$i])) {
				return $argv[$i] ? "true" : "false";
			}else{
				return $this->_mysqli->escape_string($argv[$i]);
			}
		}, $format);
		return $format;
	}
	public function format_query($format, ...$argv) {
		return $this->_mysqli->query($this->format($format, ...$argv));
	}

	public function insert($table, $data) {
		$sql = "INSERT INTO `".$table."`(";
		$len = count($data);
		foreach($data as $key=>$val) {
			$sql .= "`".$key."`";
			if(--$len != 0) {
				$sql .= ",";
			}
		}
		$sql .= ") VALUES(";
		$len = count($data);
		foreach($data as $key=>$val) {
			if(is_array($val)) {
				$val = json_encode($val, JSON_UNESCAPED_UNICODE);
			}
			$sql .= "'".$this->_mysqli->escape_string($val)."'";
			if(--$len != 0) {
				$sql .= ",";
			}
		}
		$sql .= ")";
		return $this->_mysqli->query($sql);
	}
	public function remove($table, $cond) {
		$sql = "DELETE FROM `".$tabel."`";
		// TODO where
	}
	public function update($table, $data, $cond = []) {
		$sql = "UPDATE `".$table."` SET ";
		$len = count($data);
		foreach($data as $key=>$val) {
			if(is_array($val)) {
				json_encode($val, JSON_UNESCAPED_UNICODE);
			}
			$sql .= "`".$key."`='".$this->_mysqli->escape_string($val)."'";
			if(--$len != 0) {
				$sql .= ",";
			}
		}
		// TODO where
		return $this->_mysqli->query($sql);
	}
	public function select($table, $column, $cond = [], $group = [], $order = [], $limit = []) {
		$sql = "SELECT ";
		if(is_array($column)) {
			$sql .= "`".implode("`,`", $columns)."`";
		}else if(is_string($column){
			$sql .= $column;
		}
		$sql .= " FROM `".$table."`";

		// TODO WHERE
		if(!empty($group)) {
			$sql .= " GROUP BY ";
			if(is_array($group)) {
				$sql .= "`".implode("`,`", $group)."`";
			}else if(is_string($group)) {
				$sql .= $group;
			}
		}
		$this->build_order($sql, $order);
		if(!empty($limit)) {
			$sql .= " LIMIT ";
			if(is_array($limit)) {
				$len = count($limit);
				foreach($limit as $l) {
					$sql .= intval($l);
					if(--$len != 0) {
						$sql .= ",";
					}
				}
			}else if(is_long($limit) || is_string($limit)) {
				$sql .= $limit;
			}
		}
		return $this->_mysqli->query($sql);
	}

	public function one($table, $cond = [], $order = []) {
		$sql = "SELECT * FROM `".$table."`";
		if(!empty($cond)) {
			// TODO build where
		}
		$this->build_order($sql, $order);
		$sql .= " LIMIT 1";
		$rs = $this->_mysqli->query($sql);
		if(is_object($rs)) {
			return $rs->fetch_assoc();
		}
		return null;
	}

	private function build_order(&$sql, $order) {
		if(empty($order)) return;
		$sql .= " ORDER BY ";
		if(is_array($order)) {
			$len = count($order);
			foreach($order as $key=>$val) {
				$sql .= "`".$key."`";
				if(empty($val)) {
					$sql .= " DESC";
				}else if(is_string($val)) {
					$sql .= " ".$val;
				}else{
					$sql .= " ASC";
				}
				if(--$len != 0) {
					$sql .= ",";
				}
			}
		}else if(is_string($order)) {
			$sql .= $order;
		}
	}

	public function __call($name, $argv) {
		return call_user_func_array([$this->_mysqli, $name], $argv);
	}
	public function __isset($name) {
		return isset($this->_mysqli->$name);
	}
	public function __get($name) {
		return $this->_mysqli->$name;
	}
}


class yafa_database_redis {
	protected static $_conf;
	protected static $_cache;

	public static function init($conf) {
		if(!is_array($config)) {
			throw new Exception("redis config of type array is required");
		}
		if(!isset($config["master"]) && !isset($config["slave"])) {
			throw new Exception("illegal redis config #1");
		}
		self::$_conf = $conf;
	}

	private static function create($conf) {
		$r = new Redis();
		if(isset($conf["timeout"])) {
			$r->connect($conf["host"], $conf["port"], $conf["timeout"]);
		}else{
			$r->connect($conf["host"], $conf["port"], 2.0);
		}
		if(isset($conf["prefix"])) {
			$r->setoption(Redis::OPT_PREFIX, $conf["prefix"]);
		}
		if(isset($conf["auth"])) {
			$r->auth($conf["auth"]);
		}
		return $r;
 	}

	public static function get_master($index = 0) {
		$key = "master_".$index;
		if(empty(self::$_cache[$key])) {
			$conf = self::$conf["master"][$index] ?? null;
			if(empty($conf) || !is_array($conf)) {
				throw new Exception("illegal redis config #2");
			}
			self::$_cache[$key] = self::create($conf);
		}
		return self::$_cache[$key];
	}
	public static function get_slave($index = 0) {
		$key = "slave_".$index;
		if(empty(self::$_cache[$key])) {
			$conf = self::$conf["slave"][$index] ?? null;
			if(empty($conf) || !is_array($conf)) {
				throw new Exception("illegal redis config #2");
			}
			self::$_cache[$key] = self::create($conf);
		}
		return self::$_cache[$key];
	}
}
