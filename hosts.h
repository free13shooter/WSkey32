#ifndef hosts_h
#define hosts_h

#include <WiFiUdp.h>
//--------------------------------------------------------
struct hostn{
  String name;
  String ip;
};

extern IPAddress apIP;

hostn hosts[]={
  /*"google.com","8.8.4.4",
  "gstatic.com","8.8.4.4",
  "dns.msftncsi.com","131.107.255.255",
  "docs.google.com", "216.239.32.10",
  "ssl.gstatic.com", "108.177.122.94",
  "msftconnecttest.com", "52.164.206.56",
  "google.ru", "8.8.4.4",
  "google.by", "8.8.8.8",
  "connectivitycheck.gstatic.com", "172.217.215.94",
  "clients3.google.com/generate_204", "74.125.21.139",
  "connect.rom.miui.com/generate_204","74.125.21.139"*/
};

uint32_t ntoh32(String h){
  if(h=="")return 0;
  String hst=h;
  int ix=h.indexOf(":");if(ix==-1)ix=h.indexOf("/");
  if(ix!=-1)hst=h.substring(0, ix);
  
  int count=sizeof(hosts)/sizeof(hostn);
  for(int i=0;i<count;i++){
    if(hst==hosts[i].name){
      IPAddress a;a.fromString(hosts[i].ip.c_str());
      Serial.printf("\nntoh32: DNS hosts(%s) => name= %s, ip= %s",hst.c_str(), hosts[i].name.c_str(), a.toString().c_str());
      return a;
    }
  }
  return 0;
}

IPAddress ntoh(String h){
 if(h=="")return IPAddress((uint32_t)0);
 return IPAddress(ntoh32(h));
}
//--------------------------------------------------------
#endif
