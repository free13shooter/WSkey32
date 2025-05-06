
#ifndef SWPARSER_H_
#define SWPARSER_H_

#include "global.h"
//#define ESP_WIFI_MAX_CONN_NUM (32) //...\packages\esp32\hardware\esp32\1.0.4/tools/sdk/include/esp32/esp_wifi_types.h:256:0
#include "esp_system.h"
#include "FTPClient.h"
#include "esp_wifi.h"
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WebServer.h>
#include <EEPROM.h>
#include <stdio.h>
#include "esp_spi_flash.h"
#include <Update.h>

//------------------------------------------------------------------------------------------------------
#ifndef null
  #define null  0
#endif

#ifndef ULONG
  #define ULONG  unsigned long
#endif

#define COM_BAUD_RATE 115200
//------------------------------------------------------------------------------------------------------
//handleLoad->handleLoad->...[abort/error in handleLoad]...->handleLoad->handleStatus(end/abort) - http load cycle
void handleCaptivePortal();
void handleLogin();
void handleCmd();
void handleUpdateStatus();
void handleUpdate();            
void handleUploadFileStatus();
void handleUploadFile();
void handleFavicon();
void handleRestart();
//------------------------------------------------------------------------------------------------------
extern const char* PREHTML;
extern const char* UPLOAD_HTML;
extern const char* POSTHTML;
extern const char* RETBUTT;
//------------------------------------------------------------------------------------------------------
extern const char* TITLE;//captive portal title
extern const char* VERSION; 
extern const char* LOGO;
extern const char* ICON;
extern const char* VERSION_INFO;
extern const char* CERT;


extern const char* CREATE_CLIENT_FAILED  ;
extern const char* CONNECTION_FAILED     ;
extern const char* RESPONSE_TIMEOUT      ;
extern const char* CONNECTION_TIMEOUT    ;
extern const char* TRANSFER_ERROR        ;
extern const char* NO_AP_CONNECTION      ;
extern const char* AP_CONNECTION_TIMEOUT ;
extern const char* ACCESS_DENIED         ;
extern const char* OTA_IN_PROCESS       ;
//---------------------------multipurpose internet mail extensions--------------------------------------
extern const char* MIME_text_html;//"text/html";
extern const char* MIME_text_plain;//"text/plain";
//------------------------------------------------------------------------------------------------------
void server_send_html(int code=200,String s="OK");
void server_send_fail(String s="FAIL");
void server_send_ok(String s="OK");
void server_send_continue(String s="Continue");
void server_flush();

#define stc(s) (char*)(s).c_str()
#define charv1() (char*)server->arg(1).c_str()
char*  its(int v);//int to char*

char* chcopyt(char* dest,String src,int sizedest,bool terminator=true,char termvalue=0);//копирование с возможной терминацией
char* chcopy(char* dest,String src,int sizedest);//{return chcopyt(dest,src,sizedest,false);//копирование без терминатора

String wl_status_to_string(wl_status_t stat);
String get_argval(String str,String arg,int from=0,String separators=";&");

String scanForKeys(String* s);//сканирование строки на терминальный символ/пробел и заполнение ключей
String do_upload_transponder_FTP(bool set_TimeZone=true,bool set_MaxLogs=true);

//VOLATILE
int   modAntilock(int NewValue);
int modConCntr(int NewValue);
int modIVol( int volatile *v, int NewValue);

String mac_toString(uint8_t* mac, bool hex = true);
//------------------------------------------------------------------------------------------------------
String client_request(String _host="",String _url="",uint32_t _ip=0,uint16_t _port=80,String _action="transponder");
//------------------------------------------------------------------------------------------------------
#define respTimeLimit               10000   //таймаут-лимит клиента (цикла ожидания) для ответа сервера
#define clientRespLimit             3000    //таймаут-лимит клиента для ответа сервера (http(s))
#define connectTimeLimit            1800    //таймаут-лимит клиента (цикла попыток подключений) для ответа сервера
#define errTimeLimit                2000    //таймаут отсчета ошибок

#define connTimeLimit               8000    //таймаут-лимит клиента для подключений к точке доступа

#define ledFlashOnMs                30      //длительность вспышки-индикации работы
#define ledFlashIntervalMs          1500    //интервал вспышки-индикации работы
//------------------------------------------------------------------------------------------------------
//GPIO pins definition 
#ifdef TEST_BOARD
  #define GPIOLED0      13    // RED LED ACTIVE FOR LED IS LOW
  #define _ledRedOff    digitalWrite(GPIOLED0,ledState0=HIGH)  //пульсирующая индикация опроса кнопки,штатная работа
  #define _ledRedOn     digitalWrite(GPIOLED0,ledState0=LOW)
  #define LED_ON        LOW
  #define LED_OFF       HIGH
#else
  //ACTIVE FOR LEDS IS HIGH
  #define GPIOLED0       13    // RED LED 
  #define _ledRedOff    digitalWrite(GPIOLED0,ledState0=LOW)  //пульсирующая индикация опроса кнопки,штатная работа
  #define _ledRedOn     digitalWrite(GPIOLED0,ledState0=HIGH)
  #define LED_ON        HIGH
  #define LED_OFF       LOW
#endif

//ACTIVE FOR LEDS IS HIGH
#define GPIOLED1       15    // GREEN LED
#define LED_0      GPIOLED0  // RED LED
#define LED_1      GPIOLED1  // GREEN LED
#define LED_RED    GPIOLED0  // RED LED
#define LED_GREEN  GPIOLED1  // GREEN LED

#define GPIORELAY     12  // switch
#define GPIOSW        0   // button
#define GPIOHARDRST   14  // hard reset button (to default),active=LOW

#define _ledRedToggle digitalWrite(GPIOLED0,ledState0=(ledState0==LOW?HIGH:LOW))

#define _ledGreenOff  digitalWrite(GPIOLED1,ledState1=LOW)  
#define _ledGreenOn   digitalWrite(GPIOLED1,ledState1=HIGH) //индикация работы через интернет
#define _ledGreenToggle digitalWrite(GPIOLED1,ledState1=(ledState1==LOW?HIGH:LOW))

#define _relayOn      digitalWrite(GPIORELAY,relayState=HIGH)
#define _relayOff     digitalWrite(GPIORELAY,relayState=LOW)
//кнопка _hardButton - отдельная кнопка для сброса и загрузки настроек по умолчанию._getButton-софткнопка вкл/выкл реле,при удержании просто сброс
#define _getButton    digitalRead(GPIOSW)       //active=LOW
#define _hardButton   digitalRead(GPIOHARDRST)  //hard reset to default!!! active=LOW
//------------------------------------------------------------------------------------------------------
//         ************************************************************************************         |
//------------------------------------------------------------------------------------------------------
struct SWdata {
    uint32_t  Eflag=FSSL;                                   //флаги настроек режимов работы. &1==0-первый запуск 
    char      EdeviceName[64]=DEFAULT_DEVICE_NAME;          //имя устройства.Используется для установки транспондера
    
    uint32_t  EresetInterval=5000;                          //интервал удержания кнопки для сброса      
    uint32_t  ErelayInterval=10000;                         //интервал удерживания реле                 
    
    char      Emasterkey[8]=DEFAULT_MASTERKEY;              //главный ключ                              
    char      Edevelopment[8]=DEFAULT_DEVELOPMENT;          //development acc***                       
    
    char      ESSID[64]=DEFAULT_AP_SSID;                    //точка доступа                             
    char      Epass[64]=DEFAULT_AP_PASS;                    //пароль точки доступа                      
    char      EAPIP[4]=DEFAULT_AP_IP;                       //IP точки                                 
    uint16_t  EAPport=80;                                   //порт точки                             
    
    char      ESSIDClient[64]=DEFAULT_SSID;                 //  точка доступа для подключения клиента к сети                      
    char      EpasswordClient[64]=DEFAULT_PASS;             // пароль точки доступа для подключения клиента к сети "Quantum_13";  
    //-----------new--------
    char      EremoteHost[256]=DEFAULT_REMOTE_SERVER;       //удаленный хост для доступа к интернет-обработчику,по DNS (приоритет)        
    char      EremoteHostDir[64]=DEFAULT_REMOTE_DIR;        //подпапка          
    char      EremoteIP[4]={0,0,0,0};                       //удаленный IP для доступа к транспондеру
    uint16_t  EremotePort=80;                               //удаленный IP-порт для доступа к транспондеру
    int       EremoteIntervalMs=DEFAULT_REMOTE_INTERVAL;    //интервал опроса состояния транспондера на удаленном сервере в миллисекундах (не менее 1000)
    int       ErequestActualSec=REQUEST_ACTUAL_SEC;         //время устаревания запроса
    int       EunixtimeRefresh=UNIXTIME_REFRESH;            //интервал сравнения локального и серверного времени,секунд  
    int       EremoteMaxLogs=DEFAULT_REMOTE_LOG_LEN;        //длина файла логирования в строках.Используется для установки логирования транспондера
    char      EremoteTimeZone[64]=DEFAULT_TIMEZONE;         //временная зона.Используется для установки логирования транспондера
    //----------------------
    char      Eftp_server[256]=DEFAULT_FTP_REMOTE_SERVER;   //удаленный ftp-хост для загрузки транспондера,по DNS
    char      Eftp_dir[256]=DEFAULT_FTP_DIR;                                 
    uint16_t  Eftp_port=21;
    char      Eftp_user[64]=DEFAULT_FTP_USER;
    char      Eftp_pass[64]=DEFAULT_FTP_PASS;
    int       Eftp_delay=DEFAULT_FTP_DELAY;
   //---------------------- 
    char      Etitle[512]=DEFAULT_TITLE;                    //для портала: название компании и т.д.: "<h3>ТРК Варяг</h3><h5>Varjag mass-media company</h5>";//"<h3>WIFI-security access key</h3>;

    char      Ehost[256]=DEFAULT_KEY_HOST;                  //имя хоста для загрузки ключей из сети   (приоритет)                            
    char      EhostFile[64]=DEFAULT_KEY_FILE;               //имя файла ключей+путь для загрузки ключей из сети                   
    char      EhostIP[4]={0,0,0,0};                         //хост для загрузки ключей из сети
    uint16_t  EhostPort=80;                                 //порт хоста для загрузки ключей из сети
    /*
     * приоритет загрузки имеет имя хоста
     * */
    //----------------------
    char    Einfo[256]="Для получения ключа обратитесь к администратору.Длительность разблокировки 5 секунд."; //                 
    
    int     Ekeys=0;              // всего ключей(unsigned int 4 байта)                                                                 
    char    Ekey[MAX_KEYS*8];     //ключи 800 байтов (7-знаковые и 1 байт-статусный)                                                    
    //end (max EEPROM=4095)                                
};//byte array

//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------
class SWparser;
extern WebServer* server;
extern WiFiClient* client;
extern WiFiClientSecure* sclient;
extern SWparser* parser;
//=========================================
extern String transponder_version;
extern const char* access_denied;
extern bool connected;
extern int STA_action;//0-ничего 1-соединиться 2-отсоединиться 3-старт

extern int ledState0;            // ledState used to set the LED ,LOW=off
extern int ledState1;            // ledState used to set the LED ,LOW=off
extern int relayState;           // HIGH=on
extern int swState;              // LOW=pressed
extern int localUnixtime;

extern unsigned long relayMillis ;          // отсчет с времени начала удержания
extern volatile uint32_t interval_10ms ;    //отсчет в прерывании опроса кнопок
extern unsigned long ledTimeMs1 ;           //время начала засветки светодиода

extern byte          mac[6];                   //мак устройства
extern uint8_t       macs[6 * MAX_CLIENTS];    //маки подключенных
extern unsigned long mac_ms[MAX_CLIENTS];      //время подключения
extern uint32_t      ips[MAX_CLIENTS];         //ip подключенных

extern bool OTA_enabled;
extern const char* OTA_Index;

extern unsigned long localUnixTimeRefresh_LastResponseMs;

extern volatile int antilock;
extern hw_timer_t* scan_timer;
//------------------------------------------------------------------------------------------------------
class SWparser
{
  public: 
  SWdata data;
  String key,cmd,argv,ext,com;//текущая команда
  bool upload;
  
  public:
    SWparser(bool defaultSettings=false)
    {
      init(defaultSettings);
    };
   
    ~SWparser()
    {
      EEPROM.end();
    }
    
public:
  int scom(const char* command);//выборка команды по типу чтение/запись,можно применять для проверки последней команды в урезанном формате
  String saveKeyArrayToEEPROM(String arr);
  void init(bool defaultSettings=false);
  String process(WebServer* _server/*priority*/,String remote="",bool html=false);//обработка команд.remote-команда пришла из транспондера либо портала. html-данные нужны под вывод в html.
  //загрузка masterkey,keys,SSID,password
  void loadAllFromEEPROM();
  void saveAllToEEPROM();

  int store_String(char* dest_ar,int dest_len,String src_data);//записать значение в строку.
  void store_IP_String(char* dest_IP,String src_IP);//сохранет IP из строки в массив char[4]
  uint16_t store_Port(uint16_t& dest_Port,String src_Port);//сохранет порт с проверкой
  String restore_String(char* src_ar,int src_len);//получить значение из строки
  String restore_IP_String(char* src_ar);//получить строковое представление
  uint32_t store_uint32_t(uint32_t& dest,String src);
  String restore_uint32_t(uint32_t src);
  
  String substractKey(int index);
  int searchKey(String key);//вернет [индекс][8] или -1
  String checkKey(String key);
  String toBlackList(String key);
  String deleteKey(String key);//вернет ответ OK/ERR_ARGVAL-не надено
  String addKey(String key,bool _saveToEEPROM=false);//вернет ответ
  void   sendChar(char* ar);//отослать строку char
  String sendStringMasterdata(String delimiter="&");//передать строковое представление основных настроек
  String loadStringMasterdata(WebServer* _server);//получить строковое представление основных настроек и загрузить в ОЗУ
  String getRemoteUrl();//передать данные по удаленному хосту транспондера
  String get_all_keys(bool view=false);//передает все ключи
  String get_flags(bool html=false);//get device mode
  String loadKeysFromRemoteStorage(int32_t timeout=3000);
  void   loop_upload(uint32_t minut=5);
  String parse_for_keys(uint8_t* buf, size_t len);//сканировать на ключи
  String get_sync(String check_key_value_result);//получить параметры синхронизации
};//class SWkey
//------------------------------------------------------------------------------------------------------
#endif /* SWPARSER_H_ */
