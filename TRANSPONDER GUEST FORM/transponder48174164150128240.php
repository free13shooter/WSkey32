<?php
//error_reporting(E_ERROR);
// PHP_VERSION_ID доступна в версиях PHP 5.2.7 и выше. Если версия ниже, можно её сэмулировать
if (!defined('PHP_VERSION_ID')) {
    $version = explode('.', PHP_VERSION);
    define('PHP_VERSION_ID', ($version[0] * 10000 + $version[1] * 100 + $version[2]));
	// ver < 5.2.7 if (PHP_VERSION_ID < 50207)
}
//key=AAA&cmd=BBB&arg=CCC&ext=EEE[&dev=DDD for device]
define ('INTERNAL_TRASPONDER_DEV_NUM','48174164150128240');
if (!defined('TRANSPONDER_VERSION')){define('TRANSPONDER_VERSION','1');}
if (!defined('AD')) {define('AD','access denied');}
if (!defined('PHP_EOL')) {define('PHP_EOL','\r\n');} //Доступно с PHP 5.0.2

$dev= isset($_GET['dev'])?$_GET['dev']:'';$key= isset($_GET['key'])?$_GET['key']:'';
$access= isset($_GET['access'])?$_GET['access']:'';$cmd= isset($_GET['cmd'])?$_GET['cmd']:'';
$arg= isset($_GET['arg'])?$_GET['arg']:'';$ext= isset($_GET['ext'])?$_GET['ext']:'';
$logfile='log'.$dev.'.png';$ukey= isset($_GET['ukey'])?$_GET['ukey']:'';
//echo '$logfile='.$logfile.'<br>';	
$unixtime=time();//int:количество секунд, прошедших с начала эпохи Unix (1 января 1970 00:00:00 GMT) до текущего времени.
$tzone='UTC';$maxlogs=32;$logline=1;		//временная зона,максимальное количество строк запросов(логировние),начальная строка для логирования

if(file_exists($logfile)) 
{
	$lines=file($logfile); 		//Считываем весь файл в массив
	if(count($lines)>=$logline)list($tzone,$maxlogs,$logline)=explode(';',$lines[0]);else unlink ($logfile);
	//echo '$tzone='.$tzone.'<br>'.'$maxlogs='.$maxlogs.'<br>'.'$logline='.$logline.'<br>';	
}

// установка временной зоны по умолчанию. Доступно с PHP 5.1
if(PHP_VERSION_ID >= 50100){ if(date_default_timezone_set($tzone)==false && file_exists($logfile)){unlink($logfile);$tzone='UTC';} }
//$reqtime=date('Y-m-d H:i:s T');// выведет примерно следующее:2021-02-12 20:42:12 +03
$reqtime=date('Y-m-d H:i:s T');// выведет : Mon, 15 Aug 2005 15:12:46 UTC
//echo "<br>".date('Y-m-d H:i:s T')."<br>" ;
//-----------------------------------
function wrap_output($_str)
{
	global $dev;
	return '<transponder'.$dev.">".$_str.'</transponder'.$dev.">";
}
//-----------------------------------
switch ($access){
	case 'get_server':print wrap_output(isset($_SERVER[$arg]) ? $_SERVER[$arg] : 'error');break;
	case 'get_my_ip':print wrap_output(isset($_SERVER['HTTP_X_FORWARDED_FOR']) ? $_SERVER['HTTP_X_FORWARDED_FOR'] : $_SERVER['REMOTE_ADDR']);break;
	case 'get_transponder':print wrap_output(TRANSPONDER_VERSION);break;
	case 'get_unixtime':print wrap_output($unixtime);break;
  case 'transfer':print wrap_output(save_request_to_file($logfile,$key,$cmd,$arg,$ext));check_if_file_correct($logfile);break;
  case 'receive':print wrap_output(get_request_from_file($logfile));check_if_file_correct($logfile);break;
	case 'delete_line':print wrap_output(delete_line_in_file($_file,$arg/*line*/));break;
	case 'set_response':print wrap_output(set_response_in_file($logfile,$ukey/*user key*/,$arg/*$unixtime*/,$ext/*$response*/));break;
	case 'get_response':print wrap_output(get_response_from_file($logfile,$key,$arg/*$unixtime*/));break;
	case 'set_tzone':
    if($key!=$dev){print(wrap_output(AD));break;}
	  $tzone=$arg;if(PHP_VERSION_ID >= 50100)date_default_timezone_set($tzone);
	  print wrap_output(replace_line_in_file($logfile,0,$tzone.';'.$maxlogs.';'.$logline));
	  break;
	case 'set_maxlogs':
    if($key!=$dev){print(wrap_output(AD));break;}
	  $maxlogs=$arg;print wrap_output(replace_line_in_file($logfile,0,$tzone.';'.$maxlogs.';'.$logline));
	  break;
	case 'set_logline':
    if($key!=$dev){print(wrap_output(AD));break;}
	  $logline=$arg;print wrap_output(replace_line_in_file($logfile,0,$tzone.';'.$maxlogs.';'.$logline));
	  break;
	case 'get_filesize':
    if($key!=$dev){print(wrap_output(AD));break;}
	  if (file_exists($arg)) print wrap_output(filesize($arg));else print wrap_output('no file');
	  break;
	case 'delete_file':
		if($key!=$dev)print(wrap_output(AD)); else { unlink ($arg);print wrap_output('success');}
		break;
  default : break;//break;//print(wrap_output(AD)); break;
	}
	
	if (isset($_GET['guestkey']))
	{
		echo $_POST['username'];
		$r=false;
		while ($r=false)
		{
			sleep (get_response_from_file($logfile,$key,$arg/*$unixtime*/)
		}
	}
	
	else return accessForm();
//-----------------------------------
function replace_line_in_file($_file,$index,$line) 
{
	if(!file_exists($_file))if(create_file($_file)!='success')return 'error';
		
	$lines = file($_file); // Считываем весь файл в массив
	if($index>=count($lines))return 'error';
	$fp = fopen($_file, 'wt');// Открываем файл в режиме записи
	if(!$fp)return 'error';
		
	while(!flock($fp, LOCK_EX));
	$lines[$index]=$line;
	for($i=0;$i<count($lines);$i++)
	{
		$sl=trim($lines[$i]);
		fwrite($fp,$sl.PHP_EOL);
	}
	flock($fp, LOCK_UN);fclose($fp);return 'success';
}
//-----------------------------------
function delete_line_in_file($_file,$line) 
{
	if(!file_exists($_file))return 'no file';
	$lines = file($_file); // Считываем весь файл в массив
	$fp = fopen($_file, 'wt');// Открываем файл в режиме записи
	if(!$fp)return 'error';	
	while(!flock($fp, LOCK_EX));
	
	for($i=0;$i<count($lines);$i++)
	{
		if($lines[$i]==$line)
		{
			unset($lines[$i]);$lines = array_values($lines);// Переиндексация
			for($i=0;$i<count($lines);$i++)
			{
				$sl=trim($lines[$i]);
				//echo $sl.'<br>';
				fwrite($fp,$sl.PHP_EOL);
			}
			flock($fp, LOCK_UN);fclose($fp);return 'success';
		}
	}
	flock($fp, LOCK_UN);fclose($fp);return 'not found';
}
//-----------------------------------
function check_if_file_correct($_file) 
{
	global $logline;
	if(!file_exists($_file))return "no file";
	$lines = file($_file); // Считываем весь файл в массив

	$del=false;
	for($i=(int)$logline;$i<count($lines);$i++)
	{
		if(strripos($lines[$i],'key=')===false || strripos($lines[$i],'unixtime=')===false)//не найдено одно из значений
		{
			//echo '<br>'.'delete line='.$lines[$i].'<br>';
			unset($lines[$i]);$lines = array_values($lines);// Переиндексация
			$i--;
			$del=true;
		}
	}
	
	if($del==false)return 'success';
	
	$fp = fopen($_file, 'wt');// Открываем файл в режиме записи
	if(!$fp)return 'error';
	while(!flock($fp, LOCK_EX));
	for($i=0;$i<count($lines) && $del==true;$i++)
	{
		$sl=trim($lines[$i]);
		//echo $sl.'<br>';
		fwrite($fp,$sl.PHP_EOL);
	}

	flock($fp, LOCK_UN);fclose($fp);return 'success';
}
//-----------------------------------
function put_line_to_file($_file,$line) 
{
	global $tzone,$maxlogs,$logline;
	if(!file_exists($_file))if('success'!=create_file($_file))return 'error';
	$lines = file($_file);
	$len=count($lines);if($len==0)return 'error';
	//echo '<br>put_line_to_file: $len='.$len.'<br>';
	$fp = fopen($_file, 'wt');if(!$fp)return 'error';//запись с уничтожением
	
	while(!flock($fp, LOCK_EX));
	$lines[]=$line;
	
	for($i=(int)$logline;$i<count($lines) && count($lines)>$maxlogs;$i++)
	{
		//echo 'delline=( '.$lines[$i].' )<br>';
		unset($lines[$i]);$lines = array_values($lines);// Переиндексация
	}
		
	for($i=0;$i<count($lines);$i++)
	{
		$sl=trim($lines[$i]);
		//echo $sl.'<br>';
		fwrite($fp,$sl.PHP_EOL);
	}
		
	flock($fp, LOCK_UN);fclose($fp);return 'success';
}
//-----------------------------------
function create_file($_file) 
{
	global $tzone,$maxlogs,$logline;
		
	if(file_exists($_file))return 'success';
	$fp = fopen($_file, 'at');if(!$fp)return 'error';
		
	while(!flock($fp, LOCK_EX));
	//echo "create_file:<br>";
	for($i = 0; $i < $logline; $i++)
	{
		$sline='';//PHP_EOL;
		if($i==0)$sline=$tzone.';'.$maxlogs.';'.$logline;
		fwrite($fp, $sline);
	}
	flock($fp, LOCK_UN);fclose($fp);return 'success';
}
//-----------------------------------
function save_request_to_file($_file,$_key,$_cmd,$_arg,$_ext) 
{
  global $unixtime,$reqtime;
	if($_key=='')return 'error';
	$str='key='.$_key.';cmd='.$_cmd;
	if($_arg!='')$str=$str.';arg='.$_arg;
	if($_ext!='')$str=$str.';ext='.$_ext;
	$str=$str.';unixtime='.$unixtime.';reqtime='.$reqtime;
	return put_line_to_file($_file,$str) ;
}
//-----------------------------------
function get_request_from_file($_file,$_set_processed=false)//получить запрос [и отметить как обработанный]
{
  global $logline,$reqtime,$key,$dev;
	if($key!=$dev)return AD;
	if(!file_exists($_file))return 'empty';//нет файла
	$lines = file($_file);
	for($i=(int)$logline;$i<count($lines);$i++)
	{
		$pos = strripos($lines[$i],'response=');
		if($pos===false)//еще не обработан
		{
			$str_out=trim($lines[$i]);//выходная строка
			if($_set_processed)if('success'!=replace_line_in_file($_file,$i,$str_out.';response=processed;time='.$reqtime))return 'error' ;
			//echo '<br>$str_out='.trim($lines[count($lines)-1]).'<br>';
			return $str_out ;
		}
	}
	return 'empty';
}
//-----------------------------------
function check_line_in_file($_file,$_line_index=null)
{
	global $logline;
	if($_line_index=null)$_line_index=(int)$logline;
	if(!file_exists($_file))return 'no file';//нет файла
	$lines = file($_file);
	if(count($lines)<(int)$logline || (int)$_line_index>=count($lines) || (int)$_line_index<(int)$logline)return 'error';
	return 'success';//все в норме
}
//-----------------------------------
function set_response_in_file($_file,$_ukey,$_unixtime,$_response)
{
	global $logline,$reqtime,$key,$dev;
	if($key!=$dev)return AD;
	if(!file_exists($_file))return 'error';//нет файла
	$lines = file($_file);
	for($i=(int)$logline;$i<count($lines);$i++)
	{
		if(strripos($lines[$i],'key='.$_ukey)!==false && strripos($lines[$i],'unixtime='.$_unixtime)!==false && strripos($lines[$i],'response=')===false)
		{
			if('success'!=replace_line_in_file($_file,$i,trim($lines[$i]).';response='.$_response.';time='.$reqtime))return 'error' ;else return 'success'; 
		}			
	}
	return 'not found';
}
//-----------------------------------
function get_response_from_file($_file,$_key,$_unixtime)
{
	global $logline;
	if(!file_exists($_file))return 'not found';//нет файла
	$lines = file($_file);
	for($i=(int)$logline;$i<count($lines);$i++)
	{
		if(strripos($lines[$i],'key='.$_key)!==false && strripos($lines[$i],'unixtime='.$_unixtime)!==false)
		{
			$b=strripos($lines[$i],'response=');
			if($b===false)return 'processed';//обрабатыватся
			else 
			{
				$e=strripos($lines[$i],';',$b);if($e===false)return 'not found';//равноценно ошибке
				$b+=strlen('response=');
				return substr($lines[$i],$b,$e-$b);// : string
			}
		}			
	}
	return 'not found';//возможно,строка была затерта другими запросами,т.к. лог смещается вверх
}
//-----------------------------------
function dev_name()
{
	$n1=basename(__FILE__); 
	$n2=substr ($n1,0,strlen($n1)-strlen('.php'));
	return substr ($n2,strlen('transponder'));
}
//-----------------------------------
function accessForm()
{
	?>
<html><body bgcolor="#000000"> 
 
 <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
  <style type="text/css">
	.stxt {
    height: 17%; /* Высота блока */
    width: 100%; /* Ширина блока */
    font-size: 300%; /* Размер текста */
		text-align: center;
   }
   .sinput {
    height: 10%;
    width: 20%;
    background: #010000;/* Цвет фона input */
    border: 5% solid #0f0f; /* Параметры рамки */
    text-align: center;
    font-size: 300%; /* Размер текста */
		color: rgb(200, 109, 0);
    line-height: 50%; /* Выравниваем по  центру в IE */
   }
	 .sbutton {
    height: 10%; /* Высота блока */
    width: 20%; /* Ширина блока */
    background: #000066;/* Цвет фона input */
    border: 2% solid #0000; /* Параметры рамки */
    text-align: center;
    font-size: 300%; /* Размер текста */
		color: rgb(255,0,0);
    line-height: 50%; /* Выравниваем по  центру в IE */
   }
  </style>
	
<form align="center" action= <? echo basename(__FILE__).'?access=transmit&key='.TRANSPONDER_VERSION; ?> method= "post"> 
<p class="stxt" style="color: rgb(255,0,0)"  ><br><? echo 'tx: '. dev_name() .' v:'.TRANSPONDER_VERSION; ?></p>
<p class="stxt" style="color: rgb(155,50, 200) ">INPUT ACCESS KEY:</p>
<p> <input class="sinput" type= "text" name= "guestkey" maxlength="7" autofocus> </p> 
<input class="sbutton" type= "submit" value= "SEND"> 
</body></html> 
<?php
;
}
//-----------------------------------<input type= "text" name= "name" border="5" size="50" maxlength="7" autofocus> </p> 
//ERRORS : найти ,Notice:,Parse error:,500 Internal Server Error,
?>