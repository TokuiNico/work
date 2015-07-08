<?php
/**
 * This file returns the lists of potential regions and density of potential regions
 * 
 * Input: Database
 *      trajectory
 *          format: tid
 *      Gpspoint
 *          format: tid index lng lat
 *      grid3x
 *          format: rid density range
 *
 * Output:
 *      data/grid3x_densityThre{density}.dat 
 *          format: rid density
 *      data/grid3x_transformedTraDB_{density}.dat 
 *          format: tid numRid rid1 rid2 …
 *
 **/
 
error_reporting(E_ERROR & ~E_NOTICE & ~E_WARNING );
//error_reporting(E_ALL);
include("../PostgreDB.class.php");



//Lines: 27,84; 21,68
/**
 * parameters
 * @param   griddb              the name indicating directories of filename1 and filename2(default : grid3x)
 * @param   densityThreshold    a positive integer indicating region density threshold
 * @param   filename1           the file name to save rids and densities of region which density threshold >= densityThreshold
 * @param   filename2           the file name to save trajectory data
 **/
$griddb="grid3x";
$densityThreshold=4;
$filename1="./data/".$griddb."_densityThre".$densityThreshold.".dat";
$filename2="./data/".$griddb."_transformedTraDB_".$densityThreshold.".dat";
//--------------------//

/**
 * Step1:   Generate potential regions w.r.t. threshold
 *          Select regions that their density threshold are great than or equal to given threshold value,
 *          and save region id and its density to output file
 **/

$db=new PostgreDB();
$result=$db->query("SELECT count(tid) FROM trajectory");
//$result=$db->query("SELECT count(distinct tid) FROM traofuser"); //for hits
$numTra=pg_result($result,0);
$densityThreshold=$densityThreshold*$numTra;
$densityThreshold=4;

$file1=fopen($filename1,"w");
$result=$db->query("SELECT rid,density FROM grid3x WHERE density>=$densityThreshold ORDER BY density DESC"); //<----------- grid?x
for($i=0; $i< $db->num_rows(); $i++){
	$rid=pg_result($result, $i, 'rid');
	$density=pg_result($result, $i, 'density');
	echo $rid."	".$density."\n";
	$msg=$rid."	".$density."\n";
	fputs($file1,$msg);
}
fclose($file1);



 
/**
 * Check if it a first line. return 1 if it is a first line of file
 * or return 0 and set rid and density with this line
 *
 * @param   line        the line got from input data, is a string
 * @param   rid         the region id, is reference type, default is null
 * @param   density     the density of this region, is reference type, default is null
 * @return              1: it's the first line, or 0: otherwise
 **/
function parse_PR($line,  &$rid, &$density)
{
	$tok = strtok($line,"\n\t\r");
	if($tok=='#')return 1;
		else {
		$rid = $tok;
		$tok = strtok("\n\t\r");
		$density = $tok;
		return 0;
		}
}

/**
 * Step2:   Transform trajectories
 *          Read filename1, and gets line and rid from each line of file.
 *          Finally, save rid to listPR
 *
 * @param   line        the line got from input file
 * @param   rid         the region id got from @param line
 * @param   density     the density of region got from @param line
 * @param   ListPR      a list save region id
 **/
$file=fopen($filename1,"r");
$line=fgets($file);
$indexPR=0;
while(!feof($file)){
	$is_1stLine=parse_PR($line, $rid, $density);
	if($is_1stLine){$line=fgets($file); continue;}	
	//echo $rid."	".$density."\n";
	$indexPR++;
	$listPR[$indexPR]=$rid;
	$line=fgets($file);
}
fclose($file);


$db=new PostgreDB();

/**
 * Step3:   Select all trajectories from traofuser DB, and find containing points including lat & lng.
 *          Then, transform each point to corresponding region which density >= densityThreshold
 **/
$result=$db->query("SELECT tid FROM trajectory ORDER BY tid ASC");
//$result=$db->query("SELECT distinct(tid) FROM traofuser ORDER BY tid ASC"); //for hits
$file2=fopen($filename2,"a");
for($i=0; $i< $db->num_rows(); $i++){
	echo $i." "."running... \n";
	$tid=pg_result($result,$i,'tid');
	$db_1=new PostgreDB();
    /**
     * query all points( lon & lat ) in each trajectory
     **/
	$result_1=$db_1->query("SELECT tid,index,lon,lat FROM gpspoint WHERE tid=$tid ORDER BY index ASC");
	echo $tid." ".$db_1->num_rows()."\n";
	$indexTra=1;
	$numRid=0;
	
	for($j=0; $j< $db_1->num_rows(); $j++){
		$lon=pg_result($result_1, $j, 'lon');
		$lat=pg_result($result_1, $j, 'lat');
		$db_2=new PostgreDB();
		//echo $j."\t".$lon."\t".$lat;
        
        /**
         * query corresponding region id which density >= densityThreshold
         **/
		$result_2=$db_2->query("SELECT rid FROM grid3x WHERE point'($lon,$lat)' @ range AND density>=$densityThreshold");//<--------- grid?x
		//echo $db_2->num_rows()."\t";
		$rid=pg_result($result_2,0);
		//echo $rid."\n";
		if($rid>0){
			if($j%100==0) echo $j." ";
			$numRid++;
			$listTra[$i][$indexTra]=$rid;
			$indexTra++;
		}
	}
	$listTra[$i][0]=$numRid;
	//echo "\n".$tid." ";

    /**
     * export trajectory id and number of rid to ./data/{$griddb}_transformedTraDB_{$densityThreshold}.dat
     **/
	fputs($file2,$tid." ".$listTra[$i][0]);
    /**
     * export trajectory tid and all containing rids to ./data/{$griddb}_transformedTraDB_{$densityThreshold}.dat
     **/
	for($j=1; $j<= $listTra[$i][0]; $j++){
		//echo $listTra[$i][$j]." ";
		fputs($file2," ".$listTra[$i][$j]);
	}
	//echo "\n\n";
	fputs($file2,"\n");
	
}
fclose($file2);


/**
 * Transform trajectories in a fix range
 **/
/*
function parse_tid($line,  &$tid)
{
	$tok = strtok($line,"\n\r");
	if($tok=='#')return 1;
		else {
		$tid = $tok;
		return 0;
		}
}
$file3=fopen("./data/tempTid.txt","r");
$line=fgets($file3);
$file4=fopen("./data/HsinchuTra.dat","a");
while(!feof($file3)){
	$i=0;
	echo $i." "."running... \n";
	$is_1stLine=parse_tid($line, $tid);
	if($is_1stLine){$line=fgets($file3); continue;}	
	
	
	$db=new PostgreDB();
	$result=$db->query("SELECT tid,index,lon,lat FROM gpspoint WHERE tid=$tid ORDER BY index ASC");
	echo $tid." ".$db->num_rows()."\n";
	$indexTra=1;
	$numRid=0;
	for($j=0; $j< $db->num_rows(); $j++){
		$lon=pg_result($result, $j, 'lon');
		$lat=pg_result($result, $j, 'lat');
		$db_2=new PostgreDB();
		$result_2=$db_2->query("SELECT rid FROM grid WHERE point'($lon,$lat)' @ range AND density>=$densityThreshold");
		$rid=pg_result($result_2,0);
		if($rid>0){
			if($j%100==0) echo $j." ";
			$numRid++;
			$listTra[$i][$indexTra]=$rid;
			$indexTra++;
		}
	}
	$listTra[$i][0]=$numRid;
	//echo "\n".$tid." ";
	fputs($file4,$tid." ".$listTra[$i][0]);
	for($j=1; $j<= $listTra[$i][0]; $j++){
		//echo $listTra[$i][$j]." ";
		fputs($file4," ".$listTra[$i][$j]);
	}
	//echo "\n\n";
	fputs($file4,"\n");
	
	$line=fgets($file3);
	$i=$i+1;
}
fclose($file3);
fclose($file4);
*/

?>
