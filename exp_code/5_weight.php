<?php
	error_reporting(E_ERROR & ~E_NOTICE & ~E_WARNING );
	//error_reporting(E_ALL);
	include("../PostgreDB.class.php");
 
	class Node
	{	    
	    public $tid;
	    public $num; //numRegion
	    function __construct($tid,$score)
	    {
		$this->tid = $tid;
		$this->num = $num;
	    }
	}
	/*
		Parameters: 19,23,28,34,43,49
	*/
	$griddb="grid3x";
	$filename="./link/".$griddb."/weight.in";
	$file=fopen($filename,"w");
	$db = new PostgreDB();
	$results = $db->query("SELECT distinct(rid_f) FROM grid3x_edge ORDER BY rid_f");
	for($i=0; $i < $db->num_rows(); $i++){
		$rid_f=pg_result($results,$i,0);
		echo $rid_f."\n";
		$db_1 = new PostgreDB();
		$results_1 = $db_1->query("SELECT distinct(tid) FROM grid3x_edge WHERE rid_f='$rid_f' ");
		$totalTra=$db_1->num_rows();
		$db_2 = new PostgreDB();
		$w = array();
		for($j=0; $j < $totalTra; $j++){
			$tid=pg_result($results_1,$j,0);
			$results_2=$db_2->query("SELECT count(distinct(rid_b)) FROM grid3x_edge WHERE rid_f='$rid_f' AND tid='$tid'");			
			settype($weight_2, "float");			
			$weight_2=pg_result($results_2,0,0);
			$node= new Node;
			$node->tid=$tid;
			$node->num=1/$weight_2;
			array_push($w,$node);						
		}

		$results_1 = $db_1->query("SELECT distinct(rid_b) FROM grid3x_edge WHERE rid_f='$rid_f' ");
		
		for($j=0; $j < $db_1->num_rows(); $j++){
			$weight=0.0;
			$rid_b=pg_result($results_1,$j,0);
			$db_2 = new PostgreDB();
			$results_2=$db_2->query("SELECT distinct(tid) FROM grid3x_edge WHERE rid_f='$rid_f' AND rid_b='$rid_b' ");
			for($k=0; $k < $db_2->num_rows(); $k++){
				$tid=pg_result($results_2,$k,0);
				for($t=0; $t < $totalTra; $t++){
					if($w[$t]->tid==$tid){
						$weight_1=$w[$t]->num;
						break;
					}	
				}
				$weight=$weight+$weight_1;
			}			
			$weight=$weight/$totalTra;
			$msg=$rid_f."\t".$rid_b."\t".$weight."\n";
			fputs($file,$msg);
		}
		//echo "............\n";
	}		
	fclose($file);
?>
