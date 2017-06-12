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




