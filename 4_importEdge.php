<?php
/**
 * Select region id , rid_b, and tid, and insert to database grid3x_edge
 *
 * Input
 *      link/grid3x/ridlist.in  Rid list
 *          format: Rid density
 *      link/grid3x_13/{rid}    in link and out link of region
 *          format: density
 *                  # of outlinks
 *                  {out link size
 *                  out link region, links, tid1, tid2...}
 *                  ...
 *                  # of inlinks
 *                  {in link size
 *                  in link region, links, tid1, tid2...}
 *                  ...
 * Output
 *      database grid3x_edge
 *          format: rid_f rid_b tid
 **/
	error_reporting(E_ERROR & ~E_NOTICE & ~E_WARNING );
	//error_reporting(E_ALL);
	include("../PostgreDB.class.php");

    /**
     * get rid and density from line
     *
     * @param   line    the line contain rid and density
     * @param   rid     the rid got from line, as return argument
     * @param   density the density of region forme line, as return argument
     **/
	function parse_PR($line,  &$rid, &$density)
	{
		$tok = strtok($line," \n\t\r");
		$rid = $tok;
		$tok = strtok("\n\t\r");
		$density = $tok;
		return 0;	
	}
	
	/*
		Parameters: 18,54,55
	*/
    /**
     * Step1:   Get rid and density from each line from ridlist.in
     *          then, add rids to list listPR
     **/
	$griddb="grid3x";
	$filename1="./link/".$griddb."/ridlist.in";
	//echo $filename1."\n";
	$file1=fopen($filename1,"r");
	$line=fgets($file1);
	$indexPR=0;
	while(!feof($file1)){
		parse_PR($line, $rid, $density);
		$indexPR++;
		$listPR[$indexPR]=$rid;
		$line=fgets($file1);
	}
	fclose($file1);

    /**
     * Step2:   Get rid, rid_b, and tid from each rid file, which rid in listPR,
     *          then insert them to database grid3x_edge
     **/
	for($i=3905; $i <= $indexPR; $i++){
		$rid=$listPR[$i];
		echo $i."/".$indexPR." => ".$rid."\n";
		$filename2="./link/".$griddb."/".$rid;
		$file2=fopen($filename2,"r");
		$line=fgets($file2);
		$line=fgets($file2);
		$line=fgets($file2);    //get edge of this rid
		$numRegion = strtok($line," \n\t\r");   //
		for($j=0; $j < $numRegion; $j++){
			$line = fgets($file2);
			$tok = strtok($line," \n\t\r");
			$rid_b = $tok;              //
			$tok = strtok(" \n\t\r");
			$numTID = $tok;             //
			$db_check = new PostgreDB();
			$db_insert = new PostgreDB();
			for($k=0; $k < $numTID; $k++){
				$tok = strtok(" \n\t\r");
				$tid = $tok;            //
				/*					
				$db_check->query("SELECT rid_f,rid_b,tid FROM grid3x_edge WHERE rid_f='$rid' AND rid_b='$rid_b' AND tid='$tid' ");
				if($db_check->num_rows()==0)	$db_insert->query("INSERT INTO grid3x_edge (rid_f,rid_b,tid) VALUES
							  (   '{$rid}',
								  '{$rid_b}',
								  '{$tid}'		  
							  )   ");
				*/
				$db_insert->query("INSERT INTO grid3x_edge (rid_f,rid_b,tid) VALUES
							  (   '{$rid}',
								  '{$rid_b}',
								  '{$tid}'
							  )   ");
			}
		}
		fclose($file2);
	}

?>
