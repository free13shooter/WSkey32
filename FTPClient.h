#ifndef FTPCL_H_
#define FTPCL_H_

#include <WiFiClient.h>
//------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------
class ESP32_FTPClient
{
  private:
  void WriteClientBuffered(WiFiClient* cli, unsigned char * data, int dataLength);
  char outBuf[128];
  unsigned char outCount;
  WiFiClient client;
  WiFiClient dclient;
  uint8_t verbose;

  template<typename T>
  		void FTPdbg(T msg) {
    	if(verbose == 2) Serial.print(msg);
		}
  
  template<typename T>
  		void FTPdbgn(T msg) {
    	if(verbose == 2) Serial.println(msg);
		}

    template<typename T>
    void FTPerr(T msg) {
    if(verbose == 1 || verbose == 2) Serial.print(msg);
  }

  char* userName;
  char* passWord;
  char* serverAdress;
  uint16_t port;
  bool _isConnected = false;
  unsigned char clientBuf[1500];
  size_t bufferSize = 1500;
  uint16_t timeout = 10000;
  WiFiClient* GetDataClient();
  
  public:
  ESP32_FTPClient(char* _serverAdress, uint16_t _port, char* _userName, char* _passWord, uint16_t _timeout = 10000, uint8_t _verbose = 1);
  ESP32_FTPClient(char* _serverAdress, char* _userName, char* _passWord, uint16_t _timeout = 10000, uint8_t _verbose = 1);
  bool OpenConnection();
  void CloseConnection();
  bool isConnected();
  bool NewFile (const char* fileName);
  bool AppendFile( char* fileName);
  bool WriteData (unsigned char * data, int dataLength);
  void CloseFile ();
  bool GetFTPAnswer (char* result = NULL, int offsetStart = 0);
  void GetLastModifiedTime(const char* fileName, char* result);
  void RenameFile(char* from, char* to);
  bool Write(const char * str);
  bool InitFile(const char* type);
  bool ChangeWorkDir(const char * dir);
  void DeleteFile(const char * file);
  void MakeDir(const char * dir);
  bool ContentList(const char * dir, String * list);
  bool ContentListWithListCommand(const char * dir, String * list);
  void DownloadString(const char * filename, String &str);
  void DownloadFile(const char * filename, unsigned char * buf, size_t length, bool printUART = false);
};
//------------------------------------------------------------------------------------------------------
#endif //FTPCL_H_
