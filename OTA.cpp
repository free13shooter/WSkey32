/*
 * make by dem1305
 */

#include "SWparser.h"

extern WebServer* server;
extern WiFiClient* client;
extern SWparser* parser;
bool OTA_enabled=false;
extern const char* _upd_responseHTML;
extern unsigned long ledTimeMs1;      //время начала засветки светодиода
//_______________________________________________________________________________________________
void handleRestart(){
  //Serial.print("\n=>handleRestart");
  if (!OTA_enabled) {
    Serial.printf("\n%s", ACCESS_DENIED);
    server->send(500, MIME_text_html, ACCESS_DENIED);
    server->client().stop();
    return;
  }
  server->sendHeader("Connection", "close");
  server->send(200, "text/plain", "RESRTART");
  ESP.restart();
}
//_______________________________________________________________________________________________
static String _upd_response="";
static int _upd_code=0;

void handleUpdateStatus(){
  //Serial.print("\n=>handleUpdate");
  if (!OTA_enabled) {
    Serial.printf("\n%s", ACCESS_DENIED);
    server->send(500, MIME_text_html, ACCESS_DENIED);
    return;
  }
  //server->sendHeader("Connection", "close");
 
  //server->send((Update.hasError())?200:500, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
  server->send(_upd_code, MIME_text_html, _upd_response);
  if(_upd_response=="<i>:)</i>" && _upd_code==200){
    server-> client().flush();server->client().stop();
    delay(100);
    ESP.restart();
  }
}
//_______________________________________________________________________________________________
void handleUpdate()
{
  //Serial.print("\n=>handleUpload");
  if (!OTA_enabled) {
    Serial.printf("\n%s", ACCESS_DENIED);
    server->send(403, MIME_text_plain,"Forbidden");
    server->client().stop();
    return;
  }
  
  HTTPUpload& upload = server->upload();
  if (upload.status == UPLOAD_FILE_START)
  {
    Serial.printf("\nUpdate: %s\n", upload.filename.c_str());
    if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
      Update.printError(Serial);//start with max available size
      _upd_response=Update.errorString();_upd_code=500;
    }
    else _upd_code=200;
  }
  else if (upload.status == UPLOAD_FILE_WRITE)
  {
    // flashing firmware to ESP
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize && Update.errorString()!="No Error") {
      Update.printError(Serial);
      _upd_response=Update.errorString();_upd_code=500;
      Serial.printf("\nUpdate error=%s",_upd_response.c_str());
    }
    else {
      Serial.printf("\nuploaded= %zu",upload.totalSize);
    }
  }
  else if (upload.status == UPLOAD_FILE_END)
  {
    if(Update.end(true)){
      Serial.printf("\nUpdate Success: %u\nRebooting...\
      \n █████   ██     █ \
      \n██    ██  ██    █ \
      \n██    ██  ████   \
      \n██    ██  ██   ██ \
      \n █████   ██     ██ \n\n", upload.totalSize);//true to set the size to the current progress
      _upd_response="<i>:)</i>";
    }
    else {
      Update.printError(Serial);
      _upd_response=Update.errorString();
      Serial.printf("\nUpdate end status: %s\n", _upd_response);
      if(_upd_response!="No Error")_upd_code=500;
    }
  }

  if(_upd_response!="" && _upd_code==500){Serial.print("\nUPDATE FAILED...\n");OTA_enabled=false;}
  
  if (millis() - ledTimeMs1 >= 500) {
    ledTimeMs1 = millis();
    _ledRedToggle;
  }
}
//_______________________________________________________________________________________________
