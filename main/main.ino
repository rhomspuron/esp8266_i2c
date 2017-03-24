#include "com.h"
#include "sensors.h"
#include "functions.h"


char ssid[100];
char password [100];
const int server_port = 23;
bool has_client = false;

WiFiServer server(23);
WifiCom com;
double e1[] = {2,1};
double e2[] = {1,2,4};
ExpExpression ep1(e1, 1);
ExpExpression ep2(e2,2);
SimulatorSensor st1(&ep1);
BasicSensor st2(&ep2);

BasicSensor* sensor[] = {&st1,&st2};
double sensor_values[2];

void setup() {
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ <20) {
    Serial.print(".");
    delay(500);
  }
  if (i == 21) {
    Serial.print("\nCould not connect to"); Serial.println(ssid);
    while(1) delay(1000);
  }
  server.begin();
  server.setNoDelay(true);
  
  Serial.print("Ready!\nUse 'telnet ");
  Serial.print(WiFi.localIP());
  Serial.println(" 23' to connect");  
}

void loop() {
  uint8_t i;
  double value;
  String temps;
  if (server.hasClient()){
    Serial.println("Has client");
    WiFiClient tmp = server.available();
    com = tmp;
    tmp.stop();
    has_client = true;
  }
  while(has_client){
    if (com.isConnected()){
      Cmds cmd = com.getCommand();
      if (cmd != NONE)
        Serial.println(cmd);
        switch (cmd){
          case TEMP: 
            com.sendData(readTemps(sensor, 2));
            
            break;
            
            
          case CONFIG: com.sendData(String("Not implemented.\n")); break;
          case DI: com.sendData(String("DI1 High; DI2 Low;\n")); break;
         
        }
    }
    else
      has_client = false;
   }
}


