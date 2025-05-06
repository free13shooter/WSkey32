
#include "SW_http.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "DNSServer.h"
#include <string>

#include "soc/soc.h" //disable brownout problems
#include "soc/rtc_cntl_reg.h"  //disable brownout problems
//------------------------------------------------------------------------------------------------------
// Сертификат SSL сайта iocontrol.ru, так же его можно получить
// из браузера или при помощи команды в WSL или linux:
// "openssl s_client -showcerts -connect iocontrol.ru:443 </dev/null"
/*
 * #define F(string_literal) (reinterpret_cast<const __FlashStringHelper *>(PSTR(string_literal)))
 * Это макрос, который используется для хранения strings в flash memory , а не в RAM .
 */
//------------------------------------------------------------------------------------------------------
#if (RH_PLATFORM == RH_PLATFORM_ESP8266)
// обработчик прерывания и связанный с ним код должны находиться в ОЗУ на ESP8266,
#define  INTERRUPT_ATTR ICACHE_RAM_ATTR
#elif (RH_PLATFORM == RH_PLATFORM_ESP32)
#define  INTERRUPT_ATTR IRAM_ATTR
#else
#define  INTERRUPT_ATTR
#endif
//------------------------------------------------------------------------------------------------------
extern void setup_OTA(void);
INTERRUPT_ATTR void scan_keys();
String transmit_response(String key, String unixtime, String response); //передать ответ на запрос
//------------------------------------------------------------------------------------------------------
void on_connected(bool info = true);
int STA_action=0;//0-ничего 1-соединиться 2-отсоединиться 3-старт
#define AP_CON_PAUSE_MINUTS  1//пауза для нормальной работы между попытками соединений
#define ACONNECT    1
#define ADISCONNECT 2
//------------------------------------------------------------------------------------------------------
int sta_connections = 0;        //количество активных подключений к серверу/точке доступа

WebServer* server = null;
DNSServer* dnsServer=null;
WiFiClient* client = null;
WiFiClientSecure* sclient = null;
SWparser* parser = null;

bool connected=false;

byte          mac[6];                   //мак устройства
uint8_t       macs[6 * MAX_CLIENTS];    //маки подключенных
unsigned long mac_ms[MAX_CLIENTS];      //время подключения
uint32_t      ips[MAX_CLIENTS];         //ip подключенных
//=========================================
String transponder_version = "";
//=========================================
int conState = 0;               //состояние загрузки клиентом
esp_err_t cl_err = 0; //ошибка клиента
int cl_delay = 0; //вынужденная задержка
unsigned long pconMs = 0;
//=========================================
int ledState0 = LOW;             // состояние светодиода активности,LOW=off
int ledState1 = LOW;             // состояние светодиода активности интернет-клиента,LOW=off
int relayState = LOW;            // состояние реле,HIGH=on
int swState = HIGH;              // состояние кнопки,LOW=pressed
int localUnixtime = 0;           //локальное время
unsigned long localUnixTimeRefresh_LastResponseMs; //отсчет интервала обновления
unsigned long swMillis = 0;      // прошло нажатия
unsigned long ledTimeMs1 = 0;    //время начала засветки светодиода
unsigned long relayMillis = 0;   // отсчет с времени начала удержания
unsigned long remoteTime = 0;    //время опроса
volatile uint32_t interval_10ms = 0; //отсчет в прерывании опроса кнопок
unsigned long errTime = 0;       // время отсчета ошибок

hw_timer_t* scan_timer = NULL;//esp32-hal-timer.h
volatile int antilock = 0;
int antilock_timeoutMs = ANTILOCK_MS;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

volatile uint8_t rst_mode = 0; //1==reset,2==reset+default values
//------------------------------------------------------------------------------------------------------
//мак-ячейка свободна?
bool is_maccell_free(int i)
{
  if ( 0 == macs[i * 6] && 0 == macs[i * 6 + 1] && 0 == macs[i * 6 + 2] &&
       0 == macs[i * 6 + 3] && 0 == macs[i * 6 + 4] && 0 == macs[i * 6 + 5])return true;
  else return false;
}
//поиск подключенного мак
int search_mac(uint8_t* mac)
{
  for (int i = 0; i < MAX_CLIENTS; i++)
  {
    if ( mac[0] == macs[i * 6] && mac[1] == macs[i * 6 + 1] && mac[2] == macs[i * 6 + 2] &&
         mac[3] == macs[i * 6 + 3] && mac[4] == macs[i * 6 + 4] && mac[5] == macs[i * 6 + 5])return i;
  }
  return -1;
}

//поиск подключенного индекса (номер девайса) мак/ip
int search_ip(uint32_t ip)
{
  for (int i = 0; i < MAX_CLIENTS; i++)
  {
    if ( ip == ips[i])return i;
  }
  return -1;
}

//записать mac
int save_mac(uint8_t* mac)
{
  for (int i = 0; i < MAX_CLIENTS; i++)
  {
    if (is_maccell_free(i)) {
      memcpy(&macs[i * 6], mac, 6);
      mac_ms[i] = millis();
      return i;
    }
  }
  return -1;
}
//стереть mac
int clear_mac(uint8_t* mac)
{
  int i;
  if ((i = search_mac(mac)) == -1)return -1;
  memset(&macs[i * 6], 0, 6); mac_ms[i] = 0; ips[i] = 0;
  return i;
}

String mac_toString(uint8_t* mac, bool hex/* = true*/)
{
  char szRet[25];
  if (hex)sprintf(szRet, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  else sprintf(szRet, "%u:%u:%u:%u:%u:%u", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  //Serial.printf("\n MAC=%s",szRet);
  return String(szRet);
}

void deauth_old_sta()
{
  unsigned long tm = millis();
  unsigned long maxt = 0; int maxti = 0;
  if (OTA_enabled)return;
  if ((parser->data.Eflag & FNOSTACONTROL) != 0)return;

  for (int i = 0; i < MAX_CLIENTS; i++) //сорт.по наибольшему прошедшему интервалу
  {
    if (!is_maccell_free(i) && (tm - mac_ms[i] > maxt)) {
      maxt = tm - mac_ms[i];
      maxti = i;
    }
  }

  if (maxt != 0 && !is_maccell_free(maxti))
  {
    Serial.printf("\n >> deauth[%d]: %s , time=%zu", maxti, mac_toString(&macs[maxti * 6]).c_str(), maxt);
    uint16_t aid = 0;
    const uint8_t tmac[6] = {macs[maxti * 6], macs[maxti * 6 + 1], macs[maxti * 6 + 2], macs[maxti * 6 + 3], macs[maxti * 6 + 4], macs[maxti * 6 + 5]};
    esp_err_t r = esp_wifi_ap_get_sta_aid(tmac, &aid);
    if (ESP_OK != r)
    {
      Serial.printf("\n deauth_old_sta err=%d", r);
      return;
    }

    r = esp_wifi_deauth_sta(aid);
    if (ESP_OK != r)
    {
      Serial.printf("\n deauth_old_sta err=%d", r);
      return;
    }

    Serial.print(" OK");
    return;
  }

  Serial.print("\n deauth_old_sta FAILED !");
}
//------------------------------------------------------------------------------------------------------
// * * * * * * * * * * * * * * * * * * * CONNECTIONS CONTROL * * * * * * * * * * * * * * * * * * * * * *
//------------------------------------------------------------------------------------------------------
//esp_event_legacy.h
void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  int i;
  if ((i = search_mac(info.sta_connected.mac)) == -1)
  {
    if (-1 == (i = save_mac(info.sta_connected.mac)))Serial.printf("\nWiFiStationConnected:save_mac ERROR");
    else {
      sta_connections += 1;
      Serial.printf("\n-> CONNECTED (stations: %d) MAC=%s <-", sta_connections,mac_toString(&macs[i * 6]).c_str());
      if ((parser->data.Eflag & FNOSTACONTROL) == 0 && sta_connections >= MAX_CLIENTS)deauth_old_sta(); //отключение устаревшей станции
    }
    return;
  }

  Serial.printf("\nWiFiStationConnected: mac %d %s already found", i,mac_toString(&macs[i * 6]).c_str());
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  int i;
  if ((i = search_mac(info.sta_disconnected.mac)) != -1)
  {
    String ms = mac_toString(&macs[i * 6]);
    if (-1 == clear_mac(info.sta_disconnected.mac))Serial.printf("\nWiFiStationDisconnected:clear_mac ERROR");
    else {
      sta_connections -= 1;
      Serial.printf("\n-> disconnected(total: %d) MAC=%s <-", sta_connections,ms.c_str());
    }
    return;
  }

  Serial.printf("\nWiFiStationDisconnected: mac %s NOT FOUND", mac_toString(info.sta_disconnected.mac).c_str());
}
//сопоставление mac->ip
void WiFiStaIPassigned(WiFiEvent_t event, WiFiEventInfo_t info) {
  wifi_sta_list_t wifi_sta_list;
  tcpip_adapter_sta_list_t adapter_sta_list;

  memset(&wifi_sta_list, 0, sizeof(wifi_sta_list));
  memset(&adapter_sta_list, 0, sizeof(adapter_sta_list));

  esp_wifi_ap_get_sta_list(&wifi_sta_list);
  tcpip_adapter_get_sta_list(&wifi_sta_list, &adapter_sta_list);

  for (int i = 0; i < adapter_sta_list.num; i++) {

    tcpip_adapter_sta_info_t station = adapter_sta_list.sta[i];

    int ix;
    if ((ix = search_mac(station.mac)) != -1 && station.ip.addr == info.ap_staipassigned.ip.addr)
    {
      ips[ix] = info.ap_staipassigned.ip.addr;
      Serial.printf("\nstation %d : MAC(%s), ip: %s", i, mac_toString(station.mac).c_str() , ip4addr_ntoa(&(station.ip)) );
      Serial.printf(" <<== %d", ix);
      return;
    }
  }

  Serial.print("\nWiFiStaIPassigned: mac NOT FOUND");
}
//------------------------------------------------------------------------------------------------------
static int volatile conAPcntr=-1;//время отсчета, при старте мигание светодиода 3-пауза-3... несостоявшегося соединения
void sta_connected_to_AP(WiFiEvent_t event, WiFiEventInfo_t info){//физическое подключение
  connected=false;
  char ssid_AP[33];memset(ssid_AP,0,33);memcpy(ssid_AP,info.connected.ssid,32);
  Serial.printf("\n -> STA connected to AP %s",ssid_AP);
  modConCntr(0);//start cntr
}
/**
 * устройство получило адрес от ТД
 * C:\Users\dem1305\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.6\tools\sdk\include\lwip\lwip\ip4_addr.h - ip4_addr struct{addr:u32_t}
 */
void sta_got_ip(WiFiEvent_t event, WiFiEventInfo_t info){ //WiFiEventInfo_t == system_event_info_t ->...sdk: esp_event_legacy.h -> tcpip_adapter.h
  modConCntr(-1000);//normal mode
  uint32_t ip=info.got_ip.ip_info.ip.addr;
  IPAddress local_ip(ip);
  Serial.printf("\n -> STA got local IP: %s",local_ip.toString().c_str());
  STA_action=ACONNECT;//0-ничего 1-соединиться 2-отсоединиться
}
/**
 * соединение потеряно
 */
 void sta_lost(WiFiEvent_t event, WiFiEventInfo_t info){
   connected=false;transponder_version = "";
   Serial.print("\n -> STA lost connection to AP ");

   if(STA_action>=3){
    if(STA_action==3){
      Serial.printf("\nattempt connecting to %s. . . . ",parser->restore_String(parser->data.ESSIDClient,sizeof(parser->data.ESSIDClient)).c_str());
    }
    else Serial.print(".");
    if(++STA_action>6){//5 total
      STA_action=0;
      WiFi.setAutoConnect(false);
      esp_wifi_disconnect();
      modConCntr(-1001);//разрешить отсчет AP_CON_PAUSE_MINUTS минут
      return;
    }
    modConCntr(-1);//ждем несколько попыток
    return;
   }

   esp_wifi_disconnect();
   if((parser->data.Eflag&FNOREMOTE)!=0){
    STA_action=0;
    WiFi.setAutoConnect(false);
    return;
   }
   STA_action=3;
   modConCntr(-1);//ждем несколько попыток
   WiFi.setAutoConnect(true);
   esp_wifi_connect();
 }
//------------------------------------------------------------------------------------------------------
bool configAP()//настройка точки доступа
{
  //----------------------AP---------------------
  Serial.print("\nconfiguring AP...");
  WiFi.enableAP(true);
  delay(100);

  IPAddress apIP = IPAddress(parser->data.EAPIP[0], parser->data.EAPIP[1], parser->data.EAPIP[2], parser->data.EAPIP[3]);
  //IPAddress gateway=IPAddress(parser->data.EAPIP[0],parser->data.EAPIP[1],parser->data.EAPIP[2],parser->data.EAPIP[3]);
  IPAddress subnet = IPAddress(255, 255, 255, 0);

  bool r = WiFi.softAPConfig(apIP, apIP, subnet); //ip gate sub
  if (!r) {
    Serial.print("WiFi.softAPConfig ERROR");
    return r;
  }

  Serial.printf("starting AP %s",parser->restore_String(parser->data.ESSID,sizeof(parser->data.ESSID)).c_str());
  r = WiFi.softAP(parser->restore_String(parser->data.ESSID,sizeof(parser->data.ESSID)).c_str(),parser->restore_String(parser->data.Epass,sizeof(parser->data.Epass)).c_str(), 1/*ch*/, 0/*hid*/, MAX_CLIENTS);
  if (!r) {
    Serial.print(" WiFi.softAP ERROR");
    return r;
  }
  delay(500); // Без задержки IP-адрес иногда бывает пустым
  IPAddress myIP = WiFi.softAPIP();
  Serial.print(",AP IP address: "); Serial.print(myIP);
  WiFi.onEvent(WiFiStationDisconnected, SYSTEM_EVENT_AP_STADISCONNECTED);
  WiFi.onEvent(WiFiStationConnected, SYSTEM_EVENT_AP_STACONNECTED); 
  WiFi.onEvent(WiFiStaIPassigned, SYSTEM_EVENT_AP_STAIPASSIGNED);

  
  WiFi.onEvent(sta_connected_to_AP,SYSTEM_EVENT_STA_CONNECTED);//< ESP32 station connected to AP
  WiFi.onEvent(sta_lost,SYSTEM_EVENT_STA_DISCONNECTED);//< ESP32 station lost connection to AP
  WiFi.onEvent(sta_lost,SYSTEM_EVENT_STA_LOST_IP);
  /*
   * Это событие возникает, когда клиент DHCP успешно получает адрес IPV4 от сервера DHCP или когда адрес IPV4 изменяется. 
   * Событие означает, что все готово и приложение может начать свои задачи (например, создание сокетов).
   * IPV4 может быть изменен по следующим причинам:
   * -Клиенту DHCP не удается обновить / повторно привязать адрес IPV4, и IPV4 станции сбрасывается на 0.
   * -Клиент DHCP выполняет повторную привязку к другому адресу.
   * -Статически настроенный IPV4-адрес изменен.
   */
  WiFi.onEvent(sta_got_ip,SYSTEM_EVENT_STA_GOT_IP);//< ESP32 station got IP from connected AP  esp_event_legacy.h
  if((parser->data.Eflag&FNOREMOTE)!=0){
    WiFi.setAutoConnect(false);
    Serial.printf(",подключение к AP %s не задействовано", parser->restore_String(parser->data.ESSIDClient,sizeof(parser->data.ESSIDClient)).c_str());
  }
  Serial.print("...success");
  return true;
  //----------------------------------------------
}
//------------------------------------------------------------------------------------------------------
// * * * * * * * * * * * * * * * * * * * SETUP * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//------------------------------------------------------------------------------------------------------
void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  delay(500);
  Serial.begin(COM_BAUD_RATE);
  delay(500);

  //Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }

  // set the digital pin as output/input:
  pinMode(GPIOLED0, OUTPUT);
  pinMode(GPIOLED1, OUTPUT);
  pinMode(GPIORELAY, OUTPUT);
  pinMode(GPIOSW, INPUT);
  pinMode(GPIOHARDRST, INPUT_PULLUP);//HARD RESET TO DEFAULT GPIO14 !!! (ACTIVE=LOW)

  _ledRedOff;
  _ledGreenOff;
  connected=false;
  
  digitalWrite(GPIORELAY, relayState = LOW);

  memset(macs, 0, sizeof(macs));
  WiFi.setSleep (false); // <--- эта команда отключает режим энергосбережения WiFi и устраняет connected (): Disconnected: RES: 0, ERR: 128 проблему

  Serial.printf("\n_____________________V:%s_____________________\nsetup...",VERSION);
  WiFi.enableAP(false);

  if (parser == null)
  {
#ifdef LOAD_DEFAULT_FLASH_DATA
    parser = new SWparser(true); //ДЕФОЛТ С ПРИНУДИТЕЛЬНОЙ ЗАПИСЬЮ
#else
    parser = new SWparser(digitalRead(GPIOHARDRST) == LOW);
#endif
  }

  WiFi.mode((parser->data.Eflag&FNOREMOTE)==0?WIFI_MODE_APSTA:WIFI_MODE_AP);
  //----------------------AP---------------------
  esp_err_t e = esp_wifi_clear_fast_connect();
  if (e)Serial.printf("\nERROR wifi_clear_fast_connect=%d", e);
  if(!configAP())while(1){_ledRedOn;delay(100);_ledRedOff;delay(200);}
  //----------------------------------------------
  sta_connections = 0;
  if (server == null)
  {
    Serial.printf("\nCreate server,IP:%hhu.%hhu.%hhu.%hhu...", parser->data.EAPIP[0], parser->data.EAPIP[1], parser->data.EAPIP[2], parser->data.EAPIP[3]);
    //адрес больше не используется
    server = new WebServer(80, MAX_CLIENTS);
    if ((parser->data.Eflag&FNOREMOTE)==0 && parser->data.ESSIDClient != "")
    {
      WiFi.begin(parser->data.ESSIDClient, parser->data.EpasswordClient); //сервер WiFi
      Serial.printf("\nset STA connection to AP %s", parser->restore_String(parser->data.ESSIDClient,sizeof(parser->data.ESSIDClient)).c_str());
      STA_action=0;//start
    }
    else {
      Serial.printf("\nподключение к AP %s не задействовано",parser->restore_String(parser->data.ESSIDClient,sizeof(parser->data.ESSIDClient)).c_str());
    }
  }

  if((parser->data.Eflag&FNOREMOTE)==0){
    conAPcntr=-1;
    if((parser->data.Eflag & FSSL)==0 && client == null)
    {
      Serial.print("\nCreate client...");
      client = new WiFiClient();
      if(!client){
        Serial.print("\nerror create client...");while(1){delay(1000);}
      }
    }
    else if((parser->data.Eflag & FSSL || parser->data.Eflag & FSECURE) && sclient == null)
    {
      Serial.print("\nCreate secure client...");
      sclient = new WiFiClientSecure();
      if(!sclient){
        Serial.print("\nerror create secure client...");while(1){delay(1000);}
      }
      if (!(parser->data.Eflag & FSECURE))sclient->setInsecure();else sclient->setCACert(CERT);// Устанавливаем SSL/TLS сертификат
    }
  }
  else conAPcntr=-1000;//рабочий режим сразу
    
  if (server) {
    server->on("/update", HTTP_POST, handleUpdateStatus, handleUpdate);
    server->on("/upload",HTTP_POST,handleUploadFileStatus,handleUploadFile);//upload keys file
    server->on("/", handleCaptivePortal);
    server->on("/cmd", handleCmd);
    server->on("/login",handleLogin);
    server->on("/favicon.ico",handleFavicon);
    server->onNotFound(handleCaptivePortal);

    server->begin();
    Serial.print("\nHTTP server started");
    //----------------------------------------------
    IPAddress apIP = IPAddress(parser->data.EAPIP[0], parser->data.EAPIP[1], parser->data.EAPIP[2], parser->data.EAPIP[3]);
    dnsServer=new DNSServer();
    dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer->start(53, "*", apIP);
    Serial.print("\nDNS starting");
  }
  else Serial.print("\nSERVER NOT CONFIGURED");
  //======================================
  memset(mac, 0, 6); //мак устройства
  memset(macs, 0, 6 * MAX_CLIENTS);
  memset(ips, 0, sizeof(uint32_t)*MAX_CLIENTS);
  WiFi.macAddress(mac);
  if (String(parser->data.EdeviceName) == "")
  {
    memset(parser->data.EdeviceName, 0, sizeof(parser->data.EdeviceName)); 
    sprintf(parser->data.EdeviceName, "%d%d%d%d%d%d", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    parser->saveAllToEEPROM();
  }
  Serial.printf("\nMAC: %d:%d:%d:%d:%d:%d", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]); 
  Serial.printf(" device name:%s",parser->restore_String(parser->data.EdeviceName,sizeof(parser->data.EdeviceName)).c_str());
  Serial.printf("\n*** free heap=%zu , KEYS : %d ***", esp_get_free_heap_size(), parser->data.Ekeys);

  remoteTime = errTime = millis();
  cl_delay = 0;
  int tmax = max(connTimeLimit, respTimeLimit);

  if (antilock_timeoutMs < tmax)antilock_timeoutMs = tmax + 5000;
  if ((parser->data.Eflag & FNOANTILOCK) == 0)Serial.printf("\nANTILOCK:%d ms", antilock_timeoutMs);
  
  //arduino-esp32/cores/esp32/esp32-hal-timer.h
  //таймер сканирования кнопок prescaler:divide clk 80MHz: 2000 clk/per sec
  scan_timer = timerBegin(0/*id 0-3*/, 40000, true/*true=rising,false=falling*/);
  // Attach onTimer function to our timer.
  timerAttachInterrupt(scan_timer, &scan_keys, true);//триггер прерывания по фронту
  // Устанавливаем будильник для вызова функции scan_keys каждые 10 миллисекунд,повторяем тревогу(третий параметр)
  timerAlarmWrite(scan_timer, 20, true);//каждые 10мс
  timerAlarmEnable(scan_timer);// Start an alarm

  wifi_config_t conf_current;
  esp_wifi_get_config(WIFI_IF_AP, &conf_current);
  Serial.printf("\nMAX CONNECTIONS:%d", (int)(conf_current.ap.max_connection));
  Serial.print ("\n_______________________________________________");
}
//------------------------------------------------------------------------------------------------------
// * * * * * * * * * * * * * * * * * * * * * LOOP * * * * * * * * * * * * * * * * * * * * * * * * * * *
//------------------------------------------------------------------------------------------------------
//AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.4\tools\sdk\include\esp32 - esp_err_t
void loop() {

  if (antilock >= 0)
  {
    if (antilock >= 3000)Serial.printf("\n:: antilock=%d", antilock);
    modAntilock(0);
  }
  
  static wl_status_t wstat = WL_DISCONNECTED;

  if(!(parser->data.Eflag & FNOREMOTE)){ //REMOTE ENABLED
    
    //отслеживание зависания соединения с точкой доступа для удаленного управления
    
    if(conAPcntr>=antilock_timeoutMs-1000){//не подключились, таймаут
      //ERROR CONNECTION TO AP
      WiFi.setAutoConnect(false);
      esp_wifi_disconnect();
      modConCntr(-1001);//разрешить отсчет AP_CON_PAUSE_MINUTS минут
      Serial.printf("\nВремя ожидания подключения к точке доступа вышло,пауза %d минут перед повтором",AP_CON_PAUSE_MINUTS);
    }
    else if(conAPcntr+1000<-AP_CON_PAUSE_MINUTS*60000){ //AP_CON_PAUSE_MINUTS минут прошло
      modConCntr(-1);//разрешить отсчёт времени соединения
      WiFi.setAutoConnect(true);
      Serial.printf("\nattempt connecting to %s. . . . ",parser->restore_String(parser->data.ESSIDClient,sizeof(parser->data.ESSIDClient)).c_str());
      esp_err_t e = esp_wifi_connect();
      if (e)Serial.printf("\nERROR STA connect=%d", e);
    }

    if(STA_action==ACONNECT){STA_action=0;on_connected(false);}//0-ничего 1-соединиться 2-отсоединиться
    else if(STA_action==ADISCONNECT){
      STA_action=0;
      WiFi.setAutoConnect(false);
      esp_wifi_disconnect();
    }
   
  }
  else //REMOTE DISABLED
  {
    if(connected || WiFi.status() == WL_CONNECTED) {
      WiFi.setAutoConnect(false);
      esp_err_t e = esp_wifi_disconnect();
      if (e)Serial.printf("\nERROR esp_wifi_disconnect=%d", e);
    }
  }

  wstat = WiFi.status();
  //============================
  if(dnsServer)dnsServer->processNextRequest();
  server->handleClient();
  if(connected && !(parser->data.Eflag & FNOREMOTE) && (parser->data.Eflag&FNOSERVICE)==0 && !parser->upload && !OTA_enabled)RequestTransponder();//пинговать/зарегистрироваться
  //------------------------------------------------------------------
  bool enabled_hardReset = _hardButton == LOW;
  if (!OTA_enabled && rst_mode || enabled_hardReset) //reset [hardware to default]
  {

    for (int n = 0; n < 3; n++) {
      _ledRedOff;
      delay(200);
      _ledRedOn;
      delay(200);
    }
    Serial.println("stop server..."); server->close();
    Serial.println("stop client..."); client->stop();
    Serial.println("stop Access Point..."); WiFi.softAPdisconnect(true);
    if (enabled_hardReset || rst_mode == 2)
    {
      Serial.println("\n--> RESET TO DEFAULT...");
      Serial.println("create new parser with default data...");
      delete parser; delay(250);
      parser = new SWparser(true); //load default
    }
    else Serial.println("\n--> RESET...");
    modAntilock(0);
    esp_restart();//RESET
  }
  rst_mode = 0;
  uint32_t hs = esp_get_free_heap_size();
  if (hs < 2048) {
    Serial.printf("\n*** FREE HEAP LOW !!! (%zu)   ***", hs);/*esp_restart();*/
  }
  //------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------
int modAntilock(int NewValue)
{
  return modIVol(&antilock,NewValue);
}

int modConCntr(int NewValue)
{
  return modIVol(&conAPcntr,NewValue);
}

int modIVol( int volatile *v, int NewValue){
  int volatile *pInt = v;
  int result = *pInt;

  do {
    portENTER_CRITICAL_ISR(&timerMux);
    *pInt = NewValue;
    portEXIT_CRITICAL_ISR(&timerMux);
    result = *pInt;
  } while (result != NewValue);

  return result;
}
//------------------------------------------------------------------------------------------------------
// * * * * * * * * * * * * * * * * * * * INTERRUPT * * * * * * * * * * * * * * * * * * * * * * * * * * *
//------------------------------------------------------------------------------------------------------
//опрос состояний и выполнение функций кнопок
INTERRUPT_ATTR void scan_keys()
{
  static uint32_t ledFlashMs = 0; static uint32_t ledOffTimeMs = 0; static uint32_t hrst_tms = 0;
  interval_10ms += 10; //вызов функции scan_keys каждые 10 миллисекунд
  
  int _bcon=conAPcntr;
  static int _bl=10;static int _cl=0;static int _bp=0;//for led
  if(!(parser->data.Eflag & FNOREMOTE)){ //REMOTE ENABLED
    
    if(_bcon>=0)_bcon+=10;
    else if(_bcon<-1000){ //отсчет 10 минут перед подключением
      _bcon-=10;
    }

    if(_bcon<-1000 || _bcon>=0){
      //on led
      if(_bl>0){
        if(_bcon<-1000){ if(ledState0 == LED_OFF)_ledRedOn; } else {if(ledState1 == LED_OFF)_ledGreenOn;}
        
        _bl+=10; 
        if(_bl>=60)_bl=-10; 
      }
      //off led
      if(_bl<0){
        if(_bcon<-1000){ if(ledState0 == LED_ON)_ledRedOff; } else {if(ledState1 == LED_ON)_ledGreenOff;}
        
        _bl-=10; 
        if(_bl<-150){_bl=0;if(++_cl>=3)_bp=0;else _bl=10;} 
      }
      //pause 1 sec
      if(_bl==0){
        _bp+=10;
        if(_bp>1000){
          _cl=0;_bl=10;
        }
      }
    }

    portENTER_CRITICAL_ISR(&timerMux);
      conAPcntr=_bcon;
    portEXIT_CRITICAL_ISR(&timerMux);
  }

  //if((interval_10ms % 1000)==0)Serial.printf("\n === tick 1 sec,antilock=%d ===",antilock);
  
  if ((parser->data.Eflag & FNOANTILOCK) == 0)
  {
    if (antilock >= 0)
    {
      portENTER_CRITICAL_ISR(&timerMux);
      antilock+=10;
      portEXIT_CRITICAL_ISR(&timerMux);

      if(antilock >= antilock_timeoutMs) //antilock_timeoutMs секунд зависание
      {
        Serial.printf("\n --> ANTILOCK !!! (%d) restart (%d timeout)-->",antilock, antilock_timeoutMs);
        portENTER_CRITICAL_ISR(&timerMux);
        antilock = 0;
        portEXIT_CRITICAL_ISR(&timerMux);
        esp_restart();
      }
    }
  }


  if (_hardButton == LOW && (hrst_tms += 10) >= 2000)rst_mode = 2; else hrst_tms = 0;
  if (rst_mode)return;

  //опрос состояния кнопки
  int inState = _getButton;
  if (ledState1 == HIGH && interval_10ms > ledTimeMs1 + ledFlashOnMs)_ledGreenOff;

  if (inState != swState)
  {
    swState = inState; swMillis = interval_10ms;
    //Serial.printf("swState=%s\n",swState==LOW?"ON":"OFF");
    digitalWrite(GPIOLED0, ledState0 = !swState);
    if (swState == LOW) {
      _relayOn;
      relayMillis = interval_10ms;
    }
  }
  else //состояние кнопки не изменилось
  {
    if (swState == LOW) //кнопка нажата
    {
      relayMillis = interval_10ms;
      //кнопка _hardButton - отдельная кнопка для сброса и загрузки настроек по умолчанию._getButton-софткнопка вкл/выкл реле,при удержании просто сброс
      if (interval_10ms - swMillis >= parser->data.EresetInterval) //reset
      {
        rst_mode = 1;
      }
    }
    else
    {
      if(_bcon==-1000 && interval_10ms >= ledOffTimeMs + ledFlashIntervalMs) //время вспышки/гашения
      {
        if (ledState0 == LED_OFF) {
          _ledRedOn;
          ledFlashMs = interval_10ms;
        }
        else if (ledState0 == LED_ON && interval_10ms >= ledFlashMs + ledFlashOnMs)
        {
          _ledRedOff; ledOffTimeMs = interval_10ms;
        }
      }

      swMillis = interval_10ms;
      if (relayState == HIGH)
      {
        if (interval_10ms >= relayMillis + parser->data.ErelayInterval) {
          relayMillis = interval_10ms;
          _relayOff;
          //****************check any***********************
        }
      }
      else relayMillis = interval_10ms;
    }
  }
}
//------------------------------------------------------------------------------------------------------
String RequestTransponder(String req)//опрос транспондера.Если req="",-производится плановый опрос
{
  //Serial.printf("\nRequestTransponder: cl_err=%d",cl_err);
  static unsigned long unx_ms = 0, unx_summ = 0;

  if (OTA_enabled)return String(F("error: upgrade"));
  if((parser->data.Eflag & FNOREMOTE) || !connected)
  {
    if (localUnixtime > 0)
    {
      unx_summ += millis() - unx_ms; unx_ms = millis();
      unsigned long nx = unx_summ / 1000; if (nx > 0) {
        unx_summ -= nx * 1000;
        localUnixtime += nx;
      }
      //Serial.printf("\nlocal unix time:%d",localUnixtime);
    }
    return String(F("error: not connected to access point"));
  }

  String res = "";
  String request = "";

  if(connected && transponder_version != ACTUAL_TRANSPONDER_VERSION)
  {
    res = String("error: incorrect transponder version (") + transponder_version + ")";
    Serial.printf("\n%s", res.c_str());
    return "";
  }

  //Serial.printf("\nRequestTransponder: cl_err=%d ,is_client_in_process()=%d",cl_err,is_client_in_process());
  if (/*client && is_client_in_process() || */millis() <= remoteTime + (parser->data.EremoteIntervalMs >= 1000 ? parser->data.EremoteIntervalMs : 1000))return ""; //не ошибка
  if (cl_delay > 0)
  {
    cl_delay--;
    remoteTime = millis();
    return "";//не ошибка
  }

  //Serial.printf("\nRequestTransponder: cl_err=%d",cl_err);
  request = req != "" ? req : "receive";
  res = request_on_transponder(request);

  //Serial.printf("\ntransponder response=%s",res.c_str());if(cl_err)Serial.printf(" ,err=%s",strerror(cl_err));

  if (res.indexOf("404 not found") != -1 || res.indexOf("Parse error") != -1 || res.indexOf("syntax error") != -1) //http/1.1 404 not found,Parse error
  {
    Serial.print("\nERROR !!! upload->");
    cl_err = 0;
    if (!FTP_NO_CHANGE_TRANSPONDER)do_upload_transponder_FTP();
    return "";
  }

  if (res == "empty") //дальше не обрабатывать
  {
    if (localUnixTimeRefresh_LastResponseMs == 0 || (millis() > localUnixTimeRefresh_LastResponseMs + parser->data.EunixtimeRefresh * 1000))
    {
      request = "get_unixtime"; res = request_on_transponder(request);
      String unixtime = get_argval(res, "unixtime");
      if (unixtime != "") //success
      {
        //int du=unixtime.toInt()-localUnixtime;
        localUnixtime = unixtime.toInt(); unx_ms = 0;
        //if(du>100)Serial.printf("\nUNIX TIME  NOT EQUAL:DELTA=%d",du);else
        //  Serial.printf("\nlocal unix time refresh: [parser res=\"%s\"]:[unixtime=%d]",res.c_str(),localUnixtime);
        localUnixTimeRefresh_LastResponseMs = unx_ms = millis(); unx_summ = 0;
      }
      else
      {
        localUnixtime = 0;
        Serial.printf("\nERROR unix time response : res=%s", res.c_str());
      }
      return res = "empty";
    }
    else
    {
      unx_summ += millis() - unx_ms; unx_ms = millis();
      unsigned long nx = unx_summ / 1000; if (nx > 0) {
        unx_summ -= nx * 1000;
        localUnixtime += nx;
      }
      //Serial.printf("\nlocal unix time:%d",localUnixtime);
    }

    remoteTime = millis();
    return res;
  }//empty

  if (request == "receive") //новая удаленная команда
  {
    String resp = "";
    //$ukey/*user key*/,$arg/*$unixtime*/,$ext/*$response*/
    String unixtime = get_argval(res, "unixtime");

    String key = get_argval(res, "key");
    if (unixtime != "")
    {
      //проверка на срок давности запроса
      if (localUnixtime > 0 && (localUnixtime - unixtime.toInt() > parser->data.ErequestActualSec))
      {
        Serial.printf("\nrequest:[%s]:[unixtime=%s]:[delay=%d sec]", resp.c_str(), unixtime.c_str(), localUnixtime - unixtime.toInt());
        resp = "too old";
      }
      else resp = parser->process(0, res); //проверка на ключ[+обработка]
      //проверка на предобработку
      if (resp == "set_devicename") //необходимо сначала ответить,а затем сменить имя
      {
        String newname = get_argval(res, "arg");
        if (newname == "") //пустое имя расценивается как обнуление и установка мак по умолчанию в качестве имени
        {
          char dname[sizeof(parser->data.EdeviceName)];
          Serial.printf("\nsize dname=%d", sizeof(parser->data.EdeviceName));
          memset(dname, 0, sizeof(dname)); sprintf(dname, "%d%d%d%d%d%d", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
          newname = dname;
          resp = String("new name is default as MAC=") + newname;
        }
        else {
          newname.replace(" ", "_");
          resp = String("new name=") + newname + " saved to flash";
        }

        res = transmit_response(key, unixtime, resp);
        parser->store_String(parser->data.EdeviceName, sizeof(parser->data.EdeviceName), newname);
        parser->saveAllToEEPROM();
      }
      else//не требуется упреждающей отправки ответа
      {
        res = transmit_response(key, unixtime, resp);
      }
      return res;//remoteTime не обнулять
    }
    //ошибка - ответ не требуется
  }
  
  remoteTime = millis();
  return res;
}
//--------****************************************************************************************--------
String transmit_response(String key, String unixtime, String response) //передать ответ на запрос
{
  String resp = response;
  resp.replace(" ", "_");
  String res = request_on_transponder("set_response", key, unixtime, resp);
  Serial.printf("\nresponse: [parser resp=%s]:[unixtime=%s][local_unixtime=%d]=\"%s\"", resp.c_str(), unixtime.c_str(), localUnixtime, res.c_str());
  if (cl_err)Serial.printf(",err=%s", strerror(cl_err));

  if (res.indexOf("Bad Request") != -1) //HTTP/1.1 400 Bad Request-обработать
  {
    res = request_on_transponder("delete_request", key, unixtime);
    Serial.printf("\nDELETE BAD REQUEST=", res.c_str());
  }
  return res;
}
//------------------------------------------------------------------------------------------------------
String do_upload_transponder_FTP(bool set_TimeZone, bool set_MaxLogs)
{
  String resp = "successful download"; String res;
  modAntilock(-1);
  if (upload_transponder())transponder_version = ACTUAL_TRANSPONDER_VERSION; else {
    transponder_version = "";
    resp = "error : unsuccessful download";
  }
  modAntilock(0);
  if (transponder_version == ACTUAL_TRANSPONDER_VERSION)
  {
    if (set_TimeZone) {
      res = request_on_transponder("set_tzone", "0", String(parser->data.EremoteTimeZone));
      if (res != "success")
      {
        Serial.printf("\nerror: временная зона (%s) транспондера не установлена", parser->data.EremoteTimeZone);
        resp += String(",error : timezone not set");
      }
    }
    modAntilock(0);
    if (set_TimeZone) {
      res = request_on_transponder("set_maxlogs", "", String(parser->data.EremoteMaxLogs));
      if (res != "success")
      {
        Serial.printf("\nerror: длина файла логирования не установлена(%d)", parser->data.EremoteMaxLogs);
        resp += String(",error : max logs not set");
      }
    }
    return resp;
  }
  modAntilock(0);
  return "error : upload fail";
}
//--------------------------------------------------------------------------------------
void on_connected(bool info)//произошло подключение к точке доступа
{
  cl_err = 0;
  STA_action=0;
  if (info)
  {
    Serial.printf("\n---> connected to SSID %s <---", parser->data.ESSIDClient);
    // AP/STA network info
    IPAddress myIP = WiFi.softAPIP(); Serial.print("\nAP IP: "); Serial.print(myIP);
    Serial.print("\nSTA IP: "); Serial.print(WiFi.localIP().toString()); Serial.print(", MASK: "); Serial.print(WiFi.subnetMask().toString());
    Serial.print(", GW: "); Serial.print(WiFi.gatewayIP().toString()); Serial.print(", MAC: "); Serial.print(WiFi.macAddress());
    String _host = parser->data.EremoteHost; int _port = parser->data.EremotePort;
    String s = String("remote host: ") + (_host != "" ? _host : (IPAddress((uint32_t)parser->data.EremoteIP)).toString()) + ":" + its(_port);
    Serial.printf("\n%s", s.c_str());
  }

  String res = request_on_transponder("get_transponder");
  res.toLowerCase();
  if (res.indexOf("error") != -1 && res.indexOf("connection to access point timeout") != -1 || res.indexOf("response timeout") != -1)
  {
    Serial.print("\nconnection timeout...");
    return;
  }
  else if (res.indexOf("404 not found") != -1 || res.indexOf("parse error") != -1 || res.indexOf("syntax error") != -1 || res != String(ACTUAL_TRANSPONDER_VERSION))
  {
    Serial.printf("\nTRANSPONDER CHECK ERROR, return=%s, do upload new...", res.c_str());
    if (!FTP_NO_CHANGE_TRANSPONDER)do_upload_transponder_FTP();
  }
  else transponder_version = res;// NOT toInt();

  if(transponder_version!=ACTUAL_TRANSPONDER_VERSION){
    WiFi.setAutoConnect(false);
    esp_wifi_disconnect();
    Serial.printf("\nERROR : UNCORRECT TRANSPONDER VERSION! (%s)", res.c_str());
  }
  else connected=true;
}
//--------------------------------------------------------------------------------------
