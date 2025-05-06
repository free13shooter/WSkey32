<?php 
if (!defined('PHP_VERSION_ID')) {
    $version = explode('.', PHP_VERSION);
    define('PHP_VERSION_ID', ($version[0] * 10000 + $version[1] * 100 + $version[2]));
}
if (!defined('TRANSPONDER_VERSION')){define('TRANSPONDER_VERSION','1');}
if (!defined('AD')) {define('AD','access denied');}
if (!defined('PHP_EOL')) {define('PHP_EOL','
');}
$dev= isset($_GET['dev'])?$_GET['dev']:'';$key= isset($_GET['key'])?$_GET['key']:'';
$access= isset($_GET['access'])?$_GET['access']:(isset($_POST['access'])?$_POST['access']:'');
$cmd= isset($_GET['cmd'])?$_GET['cmd']:(isset($_POST['cmd'])?$_POST['cmd']:'');
$arg= isset($_GET['arg'])?$_GET['arg']:(isset($_POST['arg'])?$_POST['arg']:'');
$ext= isset($_GET['ext'])?$_GET['ext']:(isset($_POST['ext'])?$_POST['ext']:'');
$logfile='log'.dev_name().'.png';
$ukey= isset($_GET['ukey'])?$_GET['ukey']:'';
$unixtime=time();//int:количество секунд, прошедших с начала эпохи Unix (1 января 1970 00:00:00 GMT) до текущего времени.
$tzone='UTC';$maxlogs=32;$logline=1;
$adminkey='inferno';$lang='en';
if(isset($_POST['guestkey']) && !isset($_GET['key']))
{
	$key=$_POST['guestkey'];
	if($logfile=='log.png')$logfile='log'.dev_name().'.png';
	if($access=='access')$access='';if($cmd=='cmd')$cmd='';if($arg=='arg')$arg='';if($ext=='ext')$ext='';
}
if(file_exists($logfile))
{
	$lines=file($logfile);
	if(count($lines)>=$logline)list($tzone,$maxlogs,$logline,$adminkey,$lang)=explode(';',$lines[0]);else unlink ($logfile);
	$adminkey=trim($adminkey);$lang=trim($lang);
}
if(PHP_VERSION_ID >= 50100){ if(date_default_timezone_set($tzone)==false && file_exists($logfile)){unlink($logfile);$tzone='UTC';} }
$reqtime=date('Y-m-d H:i:s T');
function wrap_output($_str)
{
	global $dev;
	return '<transponder'.$dev.">".$_str.'</transponder'.$dev.">";
}
switch ($access){
case 'get_server':print wrap_output(isset($_SERVER[$arg]) ? $_SERVER[$arg] : 'error');break;
case 'get_my_ip':print wrap_output(isset($_SERVER['HTTP_X_FORWARDED_FOR']) ? $_SERVER['HTTP_X_FORWARDED_FOR'] : $_SERVER['REMOTE_ADDR']);break;
case 'get_transponder':print wrap_output(TRANSPONDER_VERSION);break;
case 'get_unixtime':print wrap_output('unixtime='.$unixtime);break;
case 'get_logscreen':if($key!=$dev && $key!=$adminkey){print(wrap_output(AD));break;}else out_scrlog($logfile);return;
case 'transfer':print wrap_output(save_request_to_file($logfile,$key,$cmd,$arg,$ext));check_if_file_correct($logfile);break;
case 'receive':print wrap_output(get_request_from_file($logfile));check_if_file_correct($logfile);break;
case 'delete_line':print wrap_output(delete_line_in_file($_file,$arg));break;
case 'delete_request':print wrap_output(delete_request_in_file($_file,$ukey,$arg));break;case 'set_response':print wrap_output(set_response_in_file($logfile,$ukey,$arg,$ext));break;
case 'get_response':print wrap_output(get_response_from_file($logfile,$key,$arg));break;
case 'set_tzone': $tzone=$arg;break;
case 'set_maxlogs':$maxlogs=$arg;break;
case 'set_logline':$logline=$arg;break;
case 'set_adminkey':$adminkey=$arg;break;
case 'set_lang':$lang=$arg;break;
case 'get_filesize':if($key!=$dev && $key!=$adminkey){print(wrap_output(AD));break;}
		if (file_exists($arg)) print wrap_output(filesize($arg));else print wrap_output('no file');break;
case 'delete_file':if($key!=$dev && $key!=$adminkey)print(wrap_output(AD)); else { unlink ($arg);print wrap_output('success');}	break;
default : break;//break;//print(wrap_output(AD)); break;
}
if($access=='set_tzone' || $access=='set_maxlogs' || $access=='set_logline' || $access=='set_adminkey' || $access=='set_lang')
{
	if($key!=$dev && $key!=$adminkey){print(wrap_output(AD));return;}
	if($access=='set_tzone' && PHP_VERSION_ID >= 50100)date_default_timezone_set($tzone);
	$resp=replace_line_in_file($logfile,0,$tzone.';'.$maxlogs.';'.$logline.';'.$adminkey.';'.$lang);
	if(isset($_POST['guestkey']) && $_POST['guestkey']==$adminkey)return accessForm($resp,false,false,true);
	if($key==$dev)print(wrap_output($resp));
}
if($key==dev_name() && $dev==dev_name())return;
function out_scrlog($_file)
{
  global $arg,$ext;
  ?>
  <html><body bgcolor='#000000'>
  <?php
  if(!file_exists($_file))echo '<p style="font-size: 100%; color:rgb(255,0,0)">empty file</p>';
  $lines = file($_file);
  $color='#00af00';if($arg!='')$color=$arg;
  for($i=0;$i<count($lines);$i++)
  {
    $repl=' ';$str='';
    if($ext=='' || $ext=='ext')$str=str_replace(';',$repl,$lines[$i]);else $str=str_replace(';',$ext,$lines[$i]);
    ?> <div style='font-size: 100%; color:<?php echo $color ?>' > <?php echo ($i+1).': '.$str ?> </div> <?php //html + php
  }
  ?>
  </body></html>
  <?php
}
function transl($_word)
{
	global $lang;
	$lang = strtolower($lang);
	$tr=str_replace('_',' ',$_word);
	switch($lang)
	{
		case 'russian':case 'ru':
			switch($tr)
			{
				case 'error':return '☠';
				case 'success':return 'ツ';
				case 'processed':return 'в обработке';
				case 'input command':return 'введите команду';
				case 'transfer error.retry.':return '¯\(°_o)/¯';
				case 'response not found.retry.':return 'ответ не найден.повторите.';
				case 'no reply received.retry.':return 'ответ не получен.повторите.';
				case 'error: key not found':return '¯\_(ツ)_/¯';
				case 'ok':case 'OK':return 'ツ';
				case 'enter access key':return 'введите ваш код доступа';
				case 'ENTER ACCESS KEY':return 'ВВЕДИТЕ ВАШ КОД ДОСТУПА';
				case 'Estonian server - no way faster.':return 'Эстонский сервер.Быстрее-никак.';
				case 'Hello! Do you believe in God?':return '¯\(°_o)/¯';
				case "We've rated your speed.":return 'Мы оценили вашу скорость.';
				case 'Slow down - bend the button :)':return '(◣_◢)';
				case 'do you like speed?':return 'любите скорость?';
			}
			return $tr;
			break;
		case 'en':case 'eng':case 'english':default:return $tr;
	}
}
if (isset($_POST['guestkey']))
{
	global $unixtime,$logfile,$adminkey,$lang;
	$gkey=trim($_POST['guestkey']);
	if($gkey=='')return accessForm('',true,true);
	$resp='processed';
	$t=10;if($cmd=='upload_transponder'||$cmd=='loadfromhost'||$cmd=='LOADFROMHOST')$t*=2;
 if($access=='')
 {
	  if($gkey==trim($adminkey))
	  {
		  if($cmd=='' && $arg=='' && $ext=='')return accessForm(transl('input command'),true,true,true);
	  }
	  else $cmd='open';
	  $r1=save_request_to_file($logfile,$gkey,$cmd,$arg,$ext);
	  if('success'!=$r1)return accessForm($r1=='error:repeat'?transl(speed_joke()):transl($resp),
				true,$r1=='error:repeat'?false:true,$gkey==$adminkey?true:false);
	  check_if_file_correct($logfile);
 }
	while($resp=='processed' && $t>0)
	{
		sleep (1);$t--;
		$resp=get_response_from_file($logfile,$gkey,$unixtime);
		if($resp=='not found')return  accessForm(transl('no reply received.retry.'),false,false,$gkey==$adminkey?true:false);
	}
	return accessForm($resp=='error:repeat'?transl(speed_joke()):transl($resp),true,true,$gkey==$adminkey?true:false);
}
else return accessForm('',true,true);
function speed_joke()
{
	$i=mt_rand(0,4);
	switch($i)
	{
		case 0:return 'Estonian server - no way faster.';
		case 1:return 'Hello! Do you believe in God?';
		case 2:return "We've rated your speed.";
		case 3:return 'Slow down - bend the button :)';
		default:break;
	}
	return 'do you like speed?';
}
function replace_line_in_file($_file,$index,$line)
{
	if(!file_exists($_file))if(create_file($_file)!='success')return 'error';
	$lines = file($_file);
	if($index>=count($lines))return 'error';
	$fp = fopen($_file, 'wb');
	if(!$fp || !flock($fp, LOCK_EX))return 'error';
	$lines[$index]=$line;
	for($i=0;$i<count($lines);$i++)
	{
		$sl=trim($lines[$i]);
		fwrite($fp,$sl.PHP_EOL);
	}
	flock($fp, LOCK_UN);fclose($fp);return 'success';
}
function delete_line_in_file($_file,$line)
{
	if(!file_exists($_file))return 'no file';
	$lines = file($_file);
	$fp = fopen($_file, 'wb');
	if(!$fp || !flock($fp, LOCK_EX))return 'error';
	for($i=0;$i<count($lines);$i++)
	{
		if($lines[$i]==$line)
		{
			unset($lines[$i]);$lines = array_values($lines);
			for($i=0;$i<count($lines);$i++)
			{
				$sl=trim($lines[$i]);
				fwrite($fp,$sl.PHP_EOL);
			}
			flock($fp, LOCK_UN);fclose($fp);return 'success';
		}
	}
	flock($fp, LOCK_UN);fclose($fp);return 'not found';
}
function check_if_file_correct($_file)
{
	global $logline;
	if(!file_exists($_file))return 'no file';
	$lines = file($_file);
	$del=false;
	for($i=(int)$logline;$i<count($lines);$i++)
	{
		if(strripos($lines[$i],'key=')===false || strripos($lines[$i],'unixtime=')===false)
		{
			unset($lines[$i]);$lines = array_values($lines);
			$i--;$del=true;
		}
	}
	if($del==false)return 'success';
	$fp = fopen($_file, 'wb');
	if(!$fp || !flock($fp, LOCK_EX))return 'error';
	for($i=0;$i<count($lines) && $del==true;$i++)
	{
		$sl=trim($lines[$i]);
		fwrite($fp,$sl.PHP_EOL);
	}
	flock($fp, LOCK_UN);fclose($fp);return 'success';
}
function get_var($_line,$_var)
{
	$b=strripos($_line,$_var.'=');
	if($b===false)return '';
	else
	{
		$e=strripos($_line,';',$b);if($e===false)return '';
		$b+=strlen($_var.'=');
		return substr($_line,$b,$e-$b);
	}
}
function put_line_to_file($_file,$line)
{
	global $tzone,$maxlogs,$logline;
	if(!file_exists($_file))if('success'!=create_file($_file))return 'error';
	$lines = file($_file);
	$len=count($lines);if($len==0)return 'error';
	$last=(count($lines)>=$logline?$lines[count($lines)-1]:'');
	if($last!='')
	{
		$l_unixtime=get_var($last,'unixtime');$l_key=get_var($last,'key');
		if($l_unixtime!='' && $l_key!='' && ((int)get_var($line,'unixtime')-(int)$l_unixtime<=3) && get_var($line,'key')==$l_key)
			return 'error:repeat';
	}
	$fp = fopen($_file, 'wb');
	if(!$fp || !flock($fp, LOCK_EX))return 'error';
	$lines[]=$line;
	for($i=(int)$logline;$i<count($lines) && count($lines)>$maxlogs;$i++)
	{
		unset($lines[$i]);$lines = array_values($lines);
	}
	for($i=0;$i<count($lines);$i++)
	{
		$sl=trim($lines[$i]);
		fwrite($fp,$sl.PHP_EOL);
	}
	flock($fp, LOCK_UN);fclose($fp);return 'success';
}
function create_file($_file)
{
	global $tzone,$maxlogs,$logline,$adminkey,$lang;
	if(file_exists($_file))return 'success';
	$fp = fopen($_file, 'wb');if(!$fp || !flock($fp, LOCK_EX))return 'error';
	for($i = 0; $i < $logline; $i++)
	{
		$sline='';
		if($i==0)$sline=$tzone.';'.$maxlogs.';'.$logline.';'.$adminkey.';'.$lang;
		fwrite($fp, $sline);
	}
	flock($fp, LOCK_UN);fclose($fp);return 'success';
}
function save_request_to_file($_file,$_key,$_cmd,$_arg,$_ext)
{
  global $unixtime,$reqtime;
	if($_key=='')return 'error';
	$str='key='.$_key.';cmd='.$_cmd;
	if($_arg!='')$str=$str.';arg='.$_arg;
	if($_ext!='')$str=$str.';ext='.$_ext;
	$str=$str.';unixtime='.$unixtime.';reqtime='.$reqtime;
	return put_line_to_file($_file,trim($str)) ;
}
function get_request_from_file($_file,$_set_processed=false)
{
  global $logline,$reqtime,$key,$dev;
	if($key!=$dev)return AD;
	if(!file_exists($_file))return 'empty';
	$lines = file($_file);
	for($i=(int)$logline;$i<count($lines);$i++)
	{
		$pos = strripos($lines[$i],'response=');
		if($pos===false)
		{
			$str_out=trim($lines[$i]);
			if($_set_processed)if('success'!=replace_line_in_file($_file,$i,$str_out.';response=processed;time='.$reqtime))return 'error' ;
			//echo '<br>$str_out='.trim($lines[count($lines)-1]).'<br>';
			return $str_out ;
		}
	}
	return 'empty';
}
function delete_request_in_file($_file,$ukey,$arg)
{
  global $logline,$reqtime,$key,$dev;
  if($key!=$dev)return AD;
  if(!file_exists($_file))return 'error';
  $lines = file($_file);
  $_ukeyz=str_replace('_',' ',$$_ukey);
  for($i=(int)$logline;$i<count($lines);$i++)
  {
    if((strripos($lines[$i],'key='.$_ukey)!==false || strripos($lines[$i],'key='.$_ukeyz)!==false)&&
      strripos($lines[$i],'unixtime='.$_unixtime)!==false && strripos($lines[$i],'response=')===false)
    {
      return delete_line_in_file($_file,$i);
    }
  }
  return 'not found';
}
function set_response_in_file($_file,$_ukey,$_unixtime,$_response)
{
	global $logline,$reqtime,$key,$dev;
	if($key!=$dev)return AD;
	if(!file_exists($_file))return 'error';
	$lines = file($_file);
	$_ukeyz=str_replace('_',' ',$$_ukey);
	for($i=(int)$logline;$i<count($lines);$i++)
	{
		if((strripos($lines[$i],'key='.$_ukey)!==false || strripos($lines[$i],'key='.$_ukeyz)!==false) &&
					strripos($lines[$i],'unixtime='.$_unixtime)!==false && strripos($lines[$i],'response=')===false)
		{
			if('success'!=replace_line_in_file($_file,$i,trim($lines[$i]).';response='.$_response.';time='.$reqtime))return 'error' ;else return 'success';
		}
	}
	return 'not found';
}
function get_response_from_file($_file,$_key,$_unixtime)
{
	global $logline;
	if(!file_exists($_file))return 'not found';
	$lines = file($_file);
	for($i=(int)$logline;$i<count($lines);$i++)
	{
		if(strripos($lines[$i],'key='.$_key)!==false && strripos($lines[$i],'unixtime='.$_unixtime)!==false)
		{
			$var=get_var($lines[$i],'response');
			if($var=='')return 'processed';
			return $var;
		}
	}
	return 'not found';
}
function dev_name()
{
	$n1=basename(__FILE__);
	$n2=substr ($n1,0,strlen($n1)-strlen('.php'));
	return $n2;
}
function accessForm($str=null,$_input=false,$_button=false,$_admin=false){
?>
<html><body bgcolor='#000000'>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<style type="text/css">
.stxt{
height: 12%;
width: 100%;
font-size: 300%;
text-align: center;
}
._in{
height: 8%;
width: 40%;
background: #010000;
border: 5% solid #0f0f;
text-align: center;
font-size: 300%;
color: rgb(200, 109, 0);
line-height: 50%;
}
.sbutton{
height: 10%;
width: 30%;
background: #000066;
border: 2% solid #0000;
text-align: center;
vertical-align: baseline;
font-size: 300%;
color: rgb(255,0,0);
line-height: 50%;
}
 </style>
 <form align="center" action= <?php echo dev_name().'.php' ?> method= "post">
 <p class='stxt' style='color:rgb(255,0,0)' ><br><?php if($str==null)echo dev_name().' v:'.TRANSPONDER_VERSION ?></p>
 <p class='stxt' style='color: rgb(155,50, 200)' ><?php echo ((string)$str!=(string)''?$str:strtoupper(transl('ENTER ACCESS KEY'))) ?></p>
 <?php //echo '<br>admin='.$_admin;
 if($_input==true || $_admin==true) echo "<p><input class='_in' type= 'text' name= 'guestkey' maxlength='16' autofocus></p>"  ?>
 <?php if($_admin==true) echo "<p><input class='_in' type='text' name='access' value= 'transfer'><input class='_in' type='text' name='cmd' value= 'cmd'>\r
 <input class='_in' type='text' name='arg' value='arg'><input class='_in' type='text' name='ext' value='ext'></p>" ?>
 <?php if($_button==true || $_admin==true) echo "<input class='sbutton' type= 'submit' value= 'SEND' >"  ?>
 </body></html>
 <?php ;} 
?>
