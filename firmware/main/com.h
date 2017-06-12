#ifndef COM_H
#define COM_H

#include <ESP8266WiFi.h>
#include "stringstream.h"

#define MAX_READ_BUFFER 20

enum Cmds{NONE=0,TEMP,STATE,CONFIG,FIND,FOUND,SENSORS};

class WifiCom {
  public:
    WifiCom();
    WifiCom(WiFiClient &other);
    bool isConnected();
    Cmds getCommand();
    WifiCom& operator=(WiFiClient&);
    void sendData(String data);
    
  private:
   WiFiClient serverClient;
   char readBuffer[MAX_READ_BUFFER];
   bool readCmd();
   bool isCmdValid(); 
   Cmds cmd;
   
};

#endif

