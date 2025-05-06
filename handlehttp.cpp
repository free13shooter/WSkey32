#include "SWparser.h"
//_______________________________________________________________________________________________
extern bool is_maccell_free(int i);//мак-ячейка свободна?
extern int search_mac(uint8_t* mac);//поиск подключенного мак
extern int search_ip(uint32_t ip);//поиск подключенного индекса (номер девайса) мак/ip
extern int save_mac(uint8_t* mac);//записать mac
extern int clear_mac(uint8_t* mac);//стереть mac
//_______________________________________________________________________________________________
const char* PREHTML="<!DOCTYPE html><html><head><meta charset=\"utf-8\" name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
  "<style> html{font-family: Helvetica; display:inline-block;margin: 0px auto; text-align: center;}"
  "p,h5{color:#0D6EFD;} h3{color:black;width:100%;text-align:center;text-shadow:0px 3px 2px rgba(33, 9, 141, 0.466);}"
  ".fx1{text-align:center;font-size:1.2em;}.fx2{font-weight:bold;color: #f4f4f4;font-family: 'Roboto Condensed',sans-serif;text-transform:uppercase;text-shadow: 1px 1px 1px rgb(63 3 75), 0 0 1em blue, 0 0 0.1em blue;}"
  ".fx3{font-size:0.8em;text-align:center;margin:0.5em auto;}code{color:orangered;}"
  "form{display:flex;flex-flow:column nowrap;margin:0 auto;justify-content:center;align-items:center;} input{font-size:1.2em;max-width:min-content;margin:0.5em auto;}"
  "#tx{background-color:black;color:orangered;} input.placeholder{text-align: center;} .console{color:#09e505;opacity:1;}"
  "</style><title>WIFI security portal</title></head><body><center>";

const char* UPLOAD_HTML="<html><head><title></title><meta charset='utf-8'></head>""<style>"  //replace <title></title> !!!
  "body{background:black;color:#0814dc;text-align:center;}"
  "input{padding: 6px 12px;font-size:2em;border-radius: 0.5em;background: orange;}"
  "#v{width:0;height:0.2em;background:lime;background: lime;float: left;}"
  "#b{margin:1em;}"
  "#p,#v{display: none;font-size:2em;color:white;}"
"</style>"

"<form method='POST' enctype='multipart/form-data'><input id='fi' type='file' name='update'/><input id='b' type='submit' value='send'/><span id='p'></span><div id='v'></div></form>" 

//JS script multipart
"<script>"
"document.querySelector('#b').addEventListener('click',function(e) {e.preventDefault();uf();return false;});"   
"async function uf(){"
"var f = document.querySelector('form');"
"var p = document.querySelector('#p');"
"var fi= document.querySelector('#fi').files[0];"
"var v=document.querySelector('#v');p.style.display=v.style.display='block';"
"let d=new FormData(f);"    
"let r=new XMLHttpRequest();"
"r.upload.addEventListener('progress',function(e){"
  "let w=Math.round(e.loaded/e.total*100);p.innerHTML='progress: '+w+'%';v.style.width=w+'%';"
  "r.onreadystatechange=function(){"
      "if(r.readyState==4){p.style.color='red';p.innerHTML='<b>'+(r.status==200?r.responseText:'ошибка загрузки.Status='+r.status)+'<b>';}"
      "else if(r.readyState!=3){p.style.color='red';p.innerHTML='<b>'+r.responseText+'<b>';}"
"}});"
"r.open('POST','/update');"              //'/update'
"r.send(d)"
"}"
"</script>                                                           ";//+reserve

const char* POSTHTML="<h5>dev [rvscode@gmail.com]</h5></center></body></html>";

const char* RETBUTT="<form method='GET' action='/'><button id='tx' type='submit'>continue</button></form>";
//=========================multipurpose internet mail extensions=================================//
const char* MIME_text_html="text/html";
const char* MIME_text_plain="text/plain";
//_______________________________________________________________________________________________//
void server_send_html(int code,String s){server->send(code, MIME_text_html, s.c_str());}
void server_send_fail(String s){server->send(500, MIME_text_html, s.c_str());}
void server_send_ok(String s){server->send(200, MIME_text_html, s.c_str());}
void server_send_continue(String s){server->send(100, MIME_text_html, s.c_str());}
void server_flush(){server->client().flush();}
//_______________________________________________________________________________________________//
void send_headers_html(){
  server->sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server->sendHeader("Pragma", "no-cache");
  server->sendHeader("Expires", "-1");
  server->sendHeader("Location", String("http://") + IPAddress(parser->data.EAPIP[0], parser->data.EAPIP[1], parser->data.EAPIP[2], parser->data.EAPIP[3]).toString(), true);
  server->setContentLength(CONTENT_LENGTH_UNKNOWN);
}
//_______________________________________________________________________________________________//
void _portal(String response=""){
  //if(parser->data.Eflag & FC_DBG)Serial.printf("\n > captive portal uri=%s <",server->uri());
  
  if (OTA_enabled){server->send(200, MIME_text_html, OTA_IN_PROCESS);server->client().stop();return;}
  
  send_headers_html();
  String page =PREHTML;
  page += "<div class='fx1'>";
  if(strlen(parser->data.Etitle)>0)page+=parser->data.Etitle;
  page += "<img src=\"data:image/png;base64,"; page += LOGO; page += "\" alt=\"Logo\"/>";
  page +="<div class='fx3'>";page+=VERSION_INFO;page+="</div>";
  if(response)page +="<div class='fx3'><b>";page+=response;page+="</b></div>";
  page += "<br><b><span class='fx2'>Введите код авторизации</span></b><br><br>";
  page += "<form method='POST' action='login'><input name='key' type='text' placeholder='access key'><input id='tx' type='submit' value='submit'></form>";
  page += POSTHTML;
  
  server->send(200, MIME_text_html, page.c_str());
  server->client().stop();
}
void handleCaptivePortal(){_portal();}
//_______________________________________________________________________________________________
void handleFavicon(){
  //Serial.printf("\n=>handleFavicon:Content-Length=%s",String(strlen(LOGO)).c_str());
  //String page=PREHTML;page+="<link href=\"data:image/x-icon;base64,";page+=ICON;page += "\" rel=\"icon\" type=\"image/x-icon\" />";page+=POSTHTML;
  String page="<link href=\"data:image/x-icon;base64,";page+=ICON;page += "\" rel=\"icon\" type=\"image/x-icon\" />";
  server->send(200, MIME_text_html, page);
  server->client().stop();
}
//_______________________________________________________________________________________________
//в случае отсутствия ключей любая строка интерпретируется как команда
void handleLogin(){
  //if(parser->data.Eflag & FC_DBG)Serial.printf("\n * LOGIN : args=%d", server->args());
  
  if (OTA_enabled){server->send(200, MIME_text_html, OTA_IN_PROCESS);server->client().stop();return;}

  String resp="",check="";
  
  for(int i=0;i<server->args();i++){
    String nam=server->argName(i);nam.toLowerCase();
    
    if(nam=="key")
    {
      String key=server->arg(i);
      key.replace("%26", "&");
      String remote=key;bool command=false;

      if(remote.indexOf("&")!=-1){//есть ключ/команда вначале и команда дальше
        remote.replace("%3D", "=");
        if(parser->data.Ekeys){
          key=remote.substring(0,remote.indexOf("&"));
          String t=remote;
          if(remote.indexOf("key=")==-1)remote=String("key=")+key;
          remote+=t.substring(t.indexOf("&"));
        }
        else {//сразу команда
          key=parser->restore_String(parser->data.Emasterkey,sizeof(parser->data.Emasterkey));
          remote=String("key=")+key+"&cmd="+remote;
        }
        Serial.printf("\n login: replaced: remote=%s",remote.c_str());
        command=true;
      }

      if(!parser->data.Ekeys){//0 keys==MASTER
        check="ISMASTER";
        if(!command && parser->checkKey(key)=="NOTFOUND"){ //если не ключ-то команда,иначе просто открыть
          remote=String("key=")+parser->restore_String(parser->data.Emasterkey,sizeof(parser->data.Emasterkey))+String("&cmd=")+key;
          key=parser->restore_String(parser->data.Emasterkey,sizeof(parser->data.Emasterkey));
          command=true;
        }
      }
      else{// (keys>0): key=server arg "key"
        check=parser->checkKey(key);
      }
      
      if(check=="NOTFOUND")resp="ключ не найден";
        else if(check=="INBLACK")resp="ключ в чёрном списке";
          else{if(!command){
              if(parser->data.Eflag&FNOSERVICE)resp="управление отключено";else {relayMillis = interval_10ms;_relayOn;resp="OK";}
            }
          }
          
      if(parser->data.Eflag & FC_DBG)Serial.printf("\n login: key=%s, keys total=%d, check=%s, remote=%s",key.c_str(),parser->data.Ekeys,check.c_str(),remote.c_str());
      if(check=="ISMASTER"){
        if(command){
          resp = parser->process(0,remote,true);
          if(parser->data.Eflag & FC_DBG)Serial.printf("\n --> handleLogin: response=%s", resp.c_str());
        }
        //------------
        send_headers_html();
        String page =PREHTML;page+="<style>body{font-family:Consolas;background:black;}</style>";
        page+="<div class='console'>";page+=resp;page+="</div>";
        //page+="<br><br><div>";page+=RETBUTT;//page+="</div>";
        page+="<br><br><form method='POST' action='login'><input name='key' type='text' value='";page+=(parser->data.Ekeys)?key+"&cmd=":"";page+="'>";
        page+="<input id='tx' type='submit' value='submit'></form>";
        page+=POSTHTML;
        //------------
        server->send(200, MIME_text_html, page.c_str());
        server->client().stop();
        if (resp.indexOf("...restore") != -1 && resp.indexOf("default...") != -1) {
          delete parser;
          parser = new SWparser(true);
          esp_restart();
        }
        else if (resp.indexOf("...reset...") != -1)esp_restart();
        
        return;
      }
    }
  }
  _portal(resp);//снова на портал
}
//_______________________________________________________________________________________________
void handleCmd()
{
  //Serial.print("\n=>handleCmd:");
  if(OTA_enabled){server->send(200, MIME_text_html, OTA_IN_PROCESS);server->client().stop();return;}
  _ledRedOn;
  //Serial.print("\n\t uri=");Serial.print(server->uri());Serial.print(", args=");int i=server->args();Serial.print(i);
  if ((parser->data.Eflag & FNOSTACONTROL) == 0)
  {
    uint32_t c_ip = server->client().remoteIP();
    int ix;
    if ((ix = search_ip(c_ip)) == -1)
    {
      Serial.print("\n=>handleCmd: ip not found !");
    }
    else
    {
      if (!is_maccell_free(ix))
      {
        //Serial.print("\n client ip = ");Serial.print(server->client().remoteIP());
        mac_ms[ix] = millis(); //обновление времени
      }
    }
  }

  String resp = parser->process(server);

  if (2 == parser->scom("devicename")) //проверка изменения имени по последней команде
  {
    String newname = parser->argv;

    if (newname == "") //пустое имя расценивается как обнуление и установка мак по умолчанию в качестве имени
    {
      char dname[sizeof(parser->data.EdeviceName)];
      memset(dname, 0, sizeof(parser->data.EdeviceName)); sprintf(dname, "%d%d%d%d%d%d", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
      newname = dname;
      resp = String("new name is default as MAC=") + newname;
    }
    else {
      newname.replace(" ", "_");
      resp = String("new name=") + newname + " saved to flash";
    }

    parser->store_String(parser->data.EdeviceName, sizeof(parser->data.EdeviceName), newname);
    parser->saveAllToEEPROM();
  }

  if(parser->data.Eflag & FC_DBG)Serial.printf("\n --> handleCmd:response=%s", resp.c_str());
  server->send(200, "text/raw", resp.c_str());
  server->client().stop();
  if (resp.indexOf("...restore") != -1 && resp.indexOf("default...") != -1) {
    delete parser;
    parser = new SWparser(true);
    esp_restart();
  }
  else if (resp.indexOf("...reset...") != -1)esp_restart();

  _ledRedOff;
}
//_______________________________________________________________________________________________
static int _starting_w_keys=0;

void handleUploadFileStatus(){
  //Serial.print("\n=>handleUploadFileStatus");
  if (!parser->upload) {
    Serial.printf("\n%s", ACCESS_DENIED);
    server->sendHeader("Connection", "close");
    server->send(500, MIME_text_html, ACCESS_DENIED);
    server->client().flush();
    server->client().stop();
    return;
  }
  //server->sendHeader("Connection", "close");
  HTTPUpload& uploadfile = server->upload();
  
  Serial.print("\nupload status=");
  String response="";int code=200;
  switch(uploadfile.status){
    case UPLOAD_FILE_START:Serial.print("START");break;
    case UPLOAD_FILE_WRITE:Serial.print("WRITE");break;
    case UPLOAD_FILE_END:Serial.print("END");parser->upload=false;
      response = "<h3><span style='color:lime'>File was successfully uploaded</span></h3>"; 
      response += "<h5>Uploaded File Name: "; response += uploadfile.filename+"</h5>";
      response += "<h6>File Size: "; response += String((int)uploadfile.totalSize) + "</h6><br>"; 
      
      break;
    case UPLOAD_FILE_ABORTED:Serial.print("ABORTED");default: code=500;parser->upload=false;response = "<h3><span style='color:red'>File upload FAILED</span></h3>";break;
  }
  
  server->send(code, MIME_text_html , response);
  if(code==200 && parser->upload==false && _starting_w_keys!=parser->data.Ekeys){parser->saveAllToEEPROM();Serial.printf("\n\tKeys total: %d",parser->data.Ekeys);}//end
}

void handleUploadFile(){ 
  String response="";
  
  //Serial.print("\n=>handleUploadFile");
  
  if(!parser->upload)server->send(500, MIME_text_html, ACCESS_DENIED);
  
  HTTPUpload& uploadfile = server->upload(); // See https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer/srcv
                                            // For further information on 'status' structure, there are other reasons such as a failed transfer that could be used
  if(uploadfile.status == UPLOAD_FILE_START)
  {
    String filename = uploadfile.filename;
    Serial.print("\nUpload File Name: "); Serial.println(filename);
     _starting_w_keys=parser->data.Ekeys;
  }
  else if (uploadfile.status == UPLOAD_FILE_WRITE)
  {
    if (!uploadfile.currentSize) {
      response="FAILED";
    }
    else Serial.printf("\nuploaded= %zu",uploadfile.totalSize);
    
    if(response==""){
      parser->parse_for_keys(uploadfile.buf, uploadfile.currentSize); // received bytes to the file
    }

    if(response.indexOf("error")!=-1 || parser->data.Ekeys>=MAX_KEYS){response="STOP: ";response+=parser->data.Ekeys>=MAX_KEYS?"MAX KEYS !":"REPEAT KEYS !";}

    if(response!=""){
       Serial.printf("\n%s",response.c_str());
     }  
  } 
  else if (uploadfile.status == UPLOAD_FILE_END)
  {
    
  }
}
