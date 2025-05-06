
#ifndef GLOBAL_H_
#define GLOBAL_H_
//------------------------------------------------------------------------------------------------------

#define _VERSION_ "4.0" //APPLICATION GLOBAL VALUE
//------------------------------------------------------------------------------------------------------
//релиз
#define RELEASE_BINARY  

//#define FTP_NO_CHANGE_TRANSPONDER   1         //не перезаписывать транспондер


//отладка
#ifndef RELEASE_BINARY
  //#define LOAD_DEFAULT_FLASH_DATA           //если требуется загрузить предустановленные значения
  #define ACCOUNT_FOR_TEST_REMOTE             //тест FTP и доступа
  #define TEST_BOARD                          //конфигурация тестовой платы
#endif

#ifndef FTP_NO_CHANGE_TRANSPONDER
  #define FTP_NO_CHANGE_TRANSPONDER 0
#endif
//------------------------------------------------------------------------------------------------------
//         ************************************************************************************         |
//------------------------------------------------------------------------------------------------------
//флаг 7-го байта 1 бита ключа белый/черный список
#define WBLISTBIT 0 //бит принадлежности б/ч список
#define INBLACK   1

#define NOTFOUND  2
#define ISMASTER  4

//Eflag bits of SWdata
#define FNODEFAULT      1   //только загрузка,без записи предопределенных значений
#define FURL_DBG        2   //выводить отладочную информацию о подключении к удаленному серверу
#define FNOREMOTE       4   //отключить удаленное управление
#define FNOANTILOCK     8   //отключить перезагрузку при обнаружении зависания
#define FNOSTACONTROL   16  //отключить деаунтефикацию при обнаружении лимита подключенных к точке доступа станций
#define FSSL            32  //HTTPS for EremoteHost (transponder)
#define FSECURE         64  //HTTPS Secure mode
#define FKSSL           128 //HTTPS for EhostFile (keys storage)
#define FKSECURE        256 //HTTPS Secure mode for EhostFile (keys storage)
#define FNOSERVICE      512 //отключено обслуживание
#define FC_DBG          1024//выводить отладочную информацию команд

#define FECONN          2048//была ошибка подключения к точке доступа
//..........................................................................
#define MAX_KEYS                    100     //максимальное количество ключей
#define MAX_CLIENTS                 ESP_WIFI_MAX_CONN_NUM //10 по умолчанию,для WiFi сервера точки доступа
#define ANTILOCK_MS                 20000   //время таймера перезагрузки при блокировании

#define DEFAULT_DEVICE_NAME         "lock"           //имя по умолчанию.Если не задано-присваивается мак в строковом представлении
#define DEFAULT_MASTERKEY           "Zhi4E3r"        //главный ключ                              
#define DEFAULT_DEVELOPMENT         "RvSc0dE"        //development acc***                

#define REQUEST_ACTUAL_SEC          20      //время актуальности запроса
#define UNIXTIME_REFRESH            60      //интервал сравнения локального и серверного времени,секунд  
#define DEFAULT_REMOTE_INTERVAL     3000    //интервал опроса состояния транспондера на удаленном сервере в миллисекундах (не менее 1000)
#define DEFAULT_REMOTE_LOG_LEN      64
#define DEFAULT_TIMEZONE            "Europe/Minsk"

#ifdef ACCOUNT_FOR_TEST_REMOTE
  #define DEFAULT_TITLE               "<h3>WIFI-security access key</h3>"
  #define DEFAULT_AP_SSID             "TEST"          //точка доступа
  #define DEFAULT_AP_PASS             "11111111"       //пароль точки доступа                      
  #define DEFAULT_AP_IP               {192,168,4,1}    //IP точки        
  #define DEFAULT_SSID                "Redmi dem1305"   //  точка доступа для подключения клиента к сети                      
  #define DEFAULT_PASS                "Quantum_13"      // пароль точки доступа для подключения клиента к сети "Quantum_13";  
  #define DEFAULT_REMOTE_SERVER       "rvscodeservice.000webhostapp.com"
  #define DEFAULT_REMOTE_DIR          "WSkey"
  #define DEFAULT_FTP_REMOTE_SERVER   "files.000webhost.com"
  #define DEFAULT_FTP_DIR             "/public_html/WSkey/"
  #define DEFAULT_FTP_PORT            21
  #define DEFAULT_FTP_USER            "rvscodeservice"
  #define DEFAULT_FTP_PASS            "Inferno13"
#else
  #define DEFAULT_TITLE               "<h3>ТРК Варяг</h3><h5>Varjag mass-media company</h5>"
  #define DEFAULT_AP_SSID             "WSkey"           //точка доступа
  #define DEFAULT_AP_PASS             "11111111"        //пароль точки доступа                      
  #define DEFAULT_AP_IP               {192,168,4,1}     //IP точки        
  #define DEFAULT_SSID                "VARJAG_2"        //точка доступа для подключения клиента к сети                      
  #define DEFAULT_PASS                "1234567800"      //пароль точки доступа для подключения клиента к сети "Quantum_13";  
  #define DEFAULT_REMOTE_SERVER       "varjag.net"
  #define DEFAULT_REMOTE_DIR          "remote"
  #define DEFAULT_FTP_REMOTE_SERVER   "varjag.net" 
  #define DEFAULT_FTP_DIR             "/public_html/varjag.net/remote"
  #define DEFAULT_FTP_PORT            21
  #define DEFAULT_FTP_USER            "rvscode" 
  #define DEFAULT_FTP_PASS            "rvscode1305"  
#endif

#define DEFAULT_FTP_DELAY           10000
  
#define DEFAULT_KEY_HOST            "rvscodeservice.000webhostapp.com"  //имя хоста для загрузки ключей из сети   (приоритет)                            
#define DEFAULT_KEY_FILE            "swskeys_edited.txt" //имя файла ключей+путь для загрузки ключей из сети                   

#define ACTUAL_TRANSPONDER_VERSION  VERSION //версия транспондера
//------------------------------------------------------------------------------------------------------
#endif
