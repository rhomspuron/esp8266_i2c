
/*********************************************************************
ESP8266ARS: Arduino project for ALBA Remote Sensors base on ESP8266. 

Author(s): Roberto J. Homs Puron <rhoms@cells.es>, 
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

#include "com.h"

WifiCom::WifiCom(){
  serverClient = WiFiClient();
}

WifiCom::WifiCom(WiFiClient &other){
  serverClient = WiFiClient(other);
}

bool WifiCom::isConnected(){
  return serverClient.connected();
}

bool WifiCom::readCmd(){
  int index = 0;
  char inChar;
  bool newCmd = false;
  while(serverClient.available() > 0){
    newCmd = true;
    if(index < MAX_READ_BUFFER){
      inChar = serverClient.read(); // Read a character
      if (inChar == '\n' or inChar == '\r'){
        readBuffer[index] = '\0';
        break;
      }
      readBuffer[index] = inChar; // Store it
      index++; // Increment where to write next
      readBuffer[index] = '\0'; // Null terminate the string
    }
    else{
      newCmd = false;
      serverClient.write("Command too long.\n");
    }
  }
  if(index > 0 and newCmd and !isCmdValid()){
       newCmd = false;
       serverClient.write("Wrong command\n");
  }
  return newCmd;
}

Cmds WifiCom::getCommand(){
  if (!readCmd()){
    readBuffer[0]='\0';
    cmd = NONE;
  }
  return cmd;
}

WifiCom& WifiCom::operator=(WiFiClient& other){
  serverClient = other;
  return *this;
}
bool WifiCom::isCmdValid(){
  String str(readBuffer);
  str.toUpperCase();
  
  //TODO Implement the protocol
  if (str == "TEMP")
    cmd = TEMP;
  else if (str == "SENSORS")
    cmd = SENSORS;
  else if (str == "STATE")
    cmd = STATE;
  else if(str == "CONFIG")
    cmd = CONFIG;
  else if (str == "FIND")
    cmd = FIND;
  else if(str == "FOUND")
    cmd = FOUND;
  else{
    cmd = NONE;
  }
  return cmd==NONE? false: true;
}

void WifiCom::sendData(String data){
  serverClient.write(data.c_str());
}




