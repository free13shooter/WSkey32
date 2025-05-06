#ifndef SWHTTP_H_
#define SWHTTP_H_

#include "SWparser.h"


#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
//#include "protocol_examples_common.h"
//#include "esp_netif.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/esp_debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"
#include "esp_crt_bundle.h"


//------------------------------------------------------------------------------------------------------
extern const char* responseHTML;
String request_on_transponder(String  _request,String  _ukey=""/*user key*/,String  _arg="",String  _ext="",uint32_t _IP4=0,uint16_t _port=0,bool _postCloseConnection=true);
String request_on_transponder(const char* _request,String  _ukey=""/*user key*/,String  _arg="",String  _ext="",uint32_t _IP4=0,uint16_t _port=0,bool _postCloseConnection=true);
bool upload_transponder();


String RequestTransponder(String req="");//опрос транспондера.Если req="",-производится плановый опрос

int client_connect_IP(IPAddress ip, uint16_t port,int32_t timeout);
int client_connect(const char *host, uint16_t port, int32_t timeout);
void client_stop();
void client_flush();
String client_readStringUntil(char c);
int client_print(const char *buf);
int client_available();
String client_request(String _host,String _url,uint32_t _ip,uint16_t _port,String _action,bool _stop_if_error);
//------------------------------------------------------------------------------------------------------
#endif /* SWHTTP_H_ */
