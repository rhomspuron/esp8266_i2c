
/*********************************************************************
ESP8266ARS: Arduino project for ALBA Remote Sensors base on ESP8266. 

Autors: Roberto J. Homs Puron <rhoms@cells.es>, 
        Alberto Rubio Garcia <arubio@cells.es>
        Sergio Astorga Sanchez <sastorga@cells.es>
        
Copyright (C) 2017, CELLS / ALBA Synchrotron

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

**************************************************************/

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

