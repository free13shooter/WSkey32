#include "SW_http.h"

//------------------------------------------------------------------------------------------------------
extern esp_err_t cl_err;            //ошибка клиента
extern String cl_response;
extern String toDo;                 //отложенная команда

extern WebServer* server;
extern WiFiClient* client;
extern SWparser* parser;
extern int cl_delay;
extern unsigned long remoteTime;

//=========================================
extern int conState;                          //состояние загрузки клиентом
extern int attemptsToAPConnection;            //попыток подключений к точке доступа
extern esp_err_t cl_err;                      //ошибка клиента
extern int cl_delay;                          //вынужденная задержка
extern unsigned long pconMs;
extern unsigned long ledTimeMs1;              //время начала засветки светодиода
//------------------------------------------------------------------------------------------------------
/*
const char* _php =  "<?php\r\n"
                        "echo 'REMOTE_ADDR=',isset($_SERVER['HTTP_X_FORWARDED_FOR']) ? $_SERVER['HTTP_X_FORWARDED_FOR'] : $_SERVER['REMOTE_ADDR'];\r\n"
                    "?>";
*/

#include "transponder.h"// R"=====( bla bla bla )=====";

//------------------------------------------------------------------------------------------------------
String request_on_transponder(const char* _request,String  _ukey,String  _arg,String  _ext,uint32_t _IP4,uint16_t _port,bool _postCloseConnection)
{
  return request_on_transponder(String(_request),_ukey,_arg,_ext,_IP4,_port,_postCloseConnection);
}
String request_on_transponder(String _request,String  _ukey,String  _arg,String  _ext,uint32_t _IP4,uint16_t _port,bool _postCloseConnection)
{
  //url example:String("GET ") + "/" + parser->data.EhostFile + " HTTP/1.1\r\n" + "Host: " + parser->data.Ehost + "\r\n"  + "Connection: close\r\n\r\n";
  //requesting URL: GET /48174164150128240.php?access=get_transponder&dev=48174164150128240&key=48174164150128240 HTTP/1.1
  String req=String("GET ") + "/" + parser->data.EremoteHostDir + "/" + parser->data.EdeviceName + ".php?access=" + _request + 
  "&dev=" + parser->data.EdeviceName + "&key=" + parser->data.EdeviceName; 
  if(_ukey!=""){_ukey.replace(" ", "_");req+=String("&ukey=")+_ukey;}
  if(_arg!=""){_arg.replace(" ", "_");req+=String("&arg=")+_arg;}
  if(_ext!=""){_ext.replace(" ", "_");req+=String("&ext=")+_ext;}
  req+=String(" HTTP/1.1\r\nHost: ") + parser->data.EremoteHost + "\r\n" + "User-Agent: esp-idf/1.0 esp32\r\n" + (_postCloseConnection?"Connection: close\r\n\r\n":"\r\n");
  if(parser->data.Eflag&FURL_DBG){Serial.printf("\nrequest: remoteHost=%s",parser->data.EremoteHost);Serial.printf(" , remotePort=%s",its((int)parser->data.EremotePort));Serial.printf("\nurl: %s",req.c_str());}
  cl_err=0;
  
  String r=client_request( String(parser->data.EremoteHost), req, _IP4==0?(uint32_t)parser->data.EremoteIP:_IP4, _port==0?parser->data.EremotePort:_port,"transponder" );                                     

  if(parser->data.Eflag&FURL_DBG)Serial.printf("\nrequest_on_transponder:response=\"%s\"",r.c_str());
  return r;
}
//------------------------------------------------------------------------------------------------------
bool upload_transponder()
{
  Serial.printf("\n>>> upload_transponder [user=%s ftp_delay=%d] >>>",(String(parser->data.Eftp_user)).c_str(),(int)parser->data.Eftp_delay);
  
  //return true;//DBG
  ESP32_FTPClient ftp (parser->data.Eftp_server,parser->data.Eftp_port,parser->data.Eftp_user,parser->data.Eftp_pass, parser->data.Eftp_delay, 2);
  if(!ftp.OpenConnection()){Serial.print("\nupload  ERROR");return false;}
  // Get directory content
  if(!ftp.InitFile("Type A")){Serial.print("\n!ftp.InitFile  ERROR");return false;}
  String list[128];
  //if(parser->data.Eftp_dir!="" && !ftp.ChangeWorkDir(parser->data.Eftp_dir)){Serial.print("\n!ftp.ChangeWorkDir  ERROR");ftp.CloseConnection();return false;}// /public_html/varjag.net/remote
  // Create the file new and write a string into it
  if(!ftp.InitFile("Type I")){Serial.print("\n!ftp.InitFile(to write) ERROR");ftp.CloseConnection();return false;};
  String fn=String(parser->data.EdeviceName) + ".php";
  if(!ftp.NewFile(fn.c_str())){Serial.print("\n!ftp.NewFile  ERROR");ftp.CloseConnection();return false;};
  if(!ftp.Write(transponder_php)){Serial.print("\n!ftp.Write  ERROR");ftp.CloseConnection();return false;};
  ftp.CloseFile();

  ftp.CloseConnection();
  Serial.print("\n<<< upload complete <<<");
  return true;
}
//_______________________________________________________________________________________________
int client_connect_IP(IPAddress ip, uint16_t port,int32_t timeout){
  if(sclient)return sclient->connect(ip,port,timeout);else if(client) return client->connect(ip,port,timeout);
  Serial.print("\nERROR: !sclient & !client");
  return 0;
}
int client_connect(const char *host, uint16_t port, int32_t timeout){
  if(sclient)return sclient->connect(host, port,timeout);else if(client)return client->connect(host,port,timeout);
  Serial.print("\nERROR: !sclient & !client");
  return 0;
}

void client_stop(){if(sclient)sclient->stop();else client->stop();}

void client_flush(){if(sclient)sclient->flush();else client->flush();}

String client_readStringUntil(char c){
  if(sclient)return sclient->readStringUntil(c);else return client->readStringUntil(c);
}

int client_print(const char *buf){
  if(sclient)return sclient->print(buf);else return client->print(buf);//sclient->print(_url.c_str());else client->print(_url.c_str());
}

int client_available(){if(sclient)return sclient->available();else return client->available();}
//-------------------------------------------CLIENT-----------------------------------------------------
//...\packages\esp32\hardware\esp32\1.0.4\tools\sdk\include\lwip\lwip\errno.h - extern int errno;
String client_request(String _host,String _url,uint32_t _ip,uint16_t _port,String _action)
{
  //url example:String("GET ") + "/" + parser->data.EhostFile + " HTTP/1.1\r\n" + "Host: " + parser->data.Ehost + "\r\n"  + "Connection: close\r\n\r\n";
  String line,sres;
  static bool begfile=false;static bool _ch_mod=false;
  static ULONG requestTime=0;
  int inkeys=parser->data.Ekeys;
  int b=-1,e=-1;
  
  char c[9]="";c[8]=0;begfile=false;

  //Serial.print("\n=> client_request:");
     
  if(_host=="" && _ip==0){Serial.print("\nERROR : не указаны IP и DN удаленного узла.");return "error : null destination";}//empty addr
        
  if(WiFi.status() != WL_CONNECTED){
   cl_err=esp_wifi_connect();
    if(cl_err){
     sres=String(NO_AP_CONNECTION)+" "+parser->data.ESSIDClient+" ,проверьте настройки точки доступа.Ошибка="+esp_err_to_name(cl_err);
     Serial.printf("\n%s",sres);
     return NO_AP_CONNECTION;
    }
  }
        
  pconMs=millis();
       
  while(WiFi.status() != WL_CONNECTED){
   server->handleClient();//обработка клиентов сервера
   modAntilock(0);
          
   if(!WiFi.status() == WL_CONNECTED && millis()>pconMs+(ULONG)connTimeLimit)//неудачно
   {
     sres=String("Не могу подключиться к \"")+parser->data.ESSIDClient+"\" ,истек таймаут ("+its(connTimeLimit)+" мс.).Проверьте настройки точки доступа";
     Serial.printf("\n%s",sres.c_str());
     client_stop();
     return AP_CONNECTION_TIMEOUT;
   }
  }
  
  pconMs=millis();errno=0;

  //connect
  //err 261==-6 (Illegal value,ERR_VAL).вызов int WiFiClient::connect(const char *host, uint16_t port, int32_t timeout) [275] ...\packages\esp32\hardware\esp32\1.0.4\tools\sdk\include\lwip\lwip\err.h
  while( !(_host!=""?client_connect(_host.c_str(), _port,connectTimeLimit):client_connect_IP(IPAddress(_ip), _port,connectTimeLimit)) ) 
  {
    modAntilock(0);server->handleClient();//обработка клиентов сервера
    
    if(millis()>pconMs+(ULONG)/*respTimeLimit*/clientRespLimit){
      sres=String("Не могу подключиться к удаленному ")+_host+":"+its(_port)+" ,истек таймаут ответа (" + its(/*respTimeLimit*/clientRespLimit)+" мс.)";
      client_stop();
      Serial.printf("\n%s",sres.c_str());
      return CONNECTION_TIMEOUT;
    }
  }
  
  client_print(_url.c_str());
  requestTime=millis();
  if(_ch_mod || (parser->data.Eflag&FURL_DBG))Serial.printf("\nrequest URL: %s",_url.c_str());//DBG
  
  //waiting response
  pconMs=millis();modAntilock(0);
  
  while( (sclient?sclient->available():client->available()) == 0 ){
    modAntilock(0);
    server->handleClient();//обработка клиентов сервера
         
    if((sclient?sclient->available():client->available()) == 0 && millis()>pconMs+(ULONG)/*respTimeLimit*/clientRespLimit)//respTimeLimit=10000 clientRespLimit=3000
    {
      sres=String("Истек таймаут ожидания ответа удаленного узла (")+its(/*respTimeLimit*/clientRespLimit)+" мс.)";
      client_stop();
      _ledGreenOff;
      Serial.printf("\n%s",sres.c_str());
      return RESPONSE_TIMEOUT;
    }
  }
          
  _ledGreenOn;
  ledTimeMs1=pconMs=millis();

  //sres=String("подключен к удаленному ")+(_host!=""?_host:(IPAddress(_ip)).toString())+":"+its(_port)+" ,время отклика="+its((int)(millis()-requestTime))+" мс.";
  sres=String("ping=")+its((int)(millis()-requestTime));
  Serial.printf("\n*** %s ***\n",sres.c_str());        
  sres="";

  while((sclient?sclient->available():client->available()))
  {
    modAntilock(0);
    line=client_readStringUntil('\r');
    if(_ch_mod || (parser->data.Eflag&FURL_DBG))Serial.print(line.c_str());//received line from server
          
    String tl=line;tl.toLowerCase();
    if(-1!=tl.indexOf("404 not found") || -1!=tl.indexOf("400 bad request"))//ошибка 
    {
      sres=String("error: ")+line;
      Serial.printf("\n%s ,on data output and attempt of switch mode...",sres.c_str());
      client_stop();

      if(sclient != null)//------------------------------------------------------------------
      {
        delete(sclient);sclient=null;
        Serial.print("\nCreate client...");
        client = new WiFiClient();
        if(!client){
          Serial.print("\nerror create client,restart...");delay(1000);ESP.restart();
        }
        else parser->data.EremotePort=80;
      }
      else if(client != null)
      {
        delete(client);client=null;
        Serial.print("\nCreate secure client...");
        sclient = new WiFiClientSecure();
        if(!sclient){
          Serial.print("\nerror create secure client,restart...");delay(1000);ESP.restart();
        }
        if (!(parser->data.Eflag & FSECURE))sclient->setInsecure();else sclient->setCACert(CERT);// Устанавливаем SSL/TLS сертификат
        parser->data.EremotePort=443;
      }
      else Serial.print("\nerror: WTF? no clients,restart...");
      
      _ch_mod=true;
      return sres;
    }
    else if(_ch_mod){
      modAntilock(0);
      Serial.print("\nno errors, disable data output and change mode...");
      _ch_mod=false;
      if(sclient)parser->data.Eflag |=(uint32_t) FSSL;else parser->data.Eflag &=~(uint32_t) FSSL;
      parser->saveAllToEEPROM();//save port and SSL mode
    }//-------------------------------------------------------------------------------------
    
    modAntilock(0);server->handleClient();//обработка клиентов сервера
     
    if(_action=="transponder")
     {
       //Serial.print(line.c_str());
       if(b==-1)
       {
         String tl=String("<transponder")+parser->data.EdeviceName+">";//tl.toLowerCase();
         b=line.indexOf(tl);
         if(b!=-1)
         {
            sres=line.substring(b+tl.length());
            //Serial.printf("\ntransponder: begin =%d,string=\"%s\"",b,sres.c_str());
         }
        }
        if(e==-1)
        {
          sres+=line;
          e=sres.indexOf(String("</transponder")+parser->data.EdeviceName+">");
          if(e!=-1)
          {
            sres=sres.substring(0,e);
            //Serial.printf("\ntransponder: end=%d, string=\"%s\"",e,sres.c_str());
          }
        }
     }//if(_action=="transponder")
     else sres+=line;//any action

  }//if (client->available()) 

  //Serial.printf("\nno awailable res transponder:action=%s, b=%d,e=%d,string=\"%s\" ",_action,b,e,sres.c_str());

  client_stop();
        
  if(_action=="transponder" && (e==-1 || b==-1))
  {
     //Serial.printf("\nres transponder:b=%d,e=%d,string=\"%s\" ",b,e,sres.c_str());
     //if(b==-1 || e==-1)sres="";
     sres= "error : transponder not getted";
  }
   
  return _action=="loadfromhost"?"OK":sres;
}//void client_request(...)
//_______________________________________________________________________________________________
