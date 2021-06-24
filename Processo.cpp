#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include <SPIFFS.h>
#include "processo.h"
#include "global.h"
#include <stddef.h>
#include <arduino.h>
#include "Controladores.h"


#include <WiFi.h> 

#include "persistente.h"

#include <HTTPClient.h>
#include <DNSServer.h> 
#include <WebServer.h>
#include "BlynkSimpleEsp32.h"
#include <WiFiManager.h>

#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson


char auth[] = "t-BT9gRZGcFSuu-JZz4I7FTlUCe_up1Q";
char ssid[] = "Indka";
char pass[] = "iub950962";

//define your default values here, if there are different values in config.json, they are overwritten.
//char mqtt_server[40];
//char mqtt_port[6] = "8080";
const char* blynk_token = {"t-BT9gRZGcFSuu-JZz4I7FTlUCe_up1Q"};

/*Tokens
 * Old Blynk - "Pd9SZNodRX688FDh_kTitt-nOUkYIrnh"
 * New Blynk - "t-BT9gRZGcFSuu-JZz4I7FTlUCe_up1Q"
 */


//flag for saving data
bool shouldSaveConfig = false;



#define BLYNK_PRINT Serial
#define BLYNK_WM_DEBUG    3
WidgetTerminal terminal(V10);
WiFiManager wifiManager;//Objeto de manipulação do wi-fi

extern Controladores SensoresAtuadores[MAXDEVICE];

extern Persistente persistente;

Processo &Processo::getInstance()
{      
  static Processo _instance;
  return _instance;
}



void Processo::init()
{ 
  #ifdef COM_BLYNK_WIFI
  Blynk.begin(auth, ssid, pass);
  #endif
  /*
  //---------------------------------------------------------------------------------
  
  //clean FS, for testing
  //SPIFFS.format();

  //read configuration from FS json
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");

          //mqtt_server = json["mqtt_server"];
          //mqtt_port = json["mqtt_port"];
          blynk_token = json["blynk_token"];
        } else {
          Serial.println("failed to load json config");
        }
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
  //end read



  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  //WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  //WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 8);
  WiFiManagerParameter custom_blynk_token("blynk", "blynk token", blynk_token, 32);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  //WiFiManager wifiManager;

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //add all your parameters here
  //wifiManager.addParameter(&custom_mqtt_server);
  //wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_blynk_token);

  //reset settings - for testing
  wifiManager.resetSettings();

  //0800 7096200
  //40046200

  //set custom ip for portal
  wifiManager.setAPConfig(IPAddress(10,1,1,1), IPAddress(10,1,1,1), IPAddress(255,255,255,0));

  //set minimu quality of signal so it ignores AP's under that quality
  //defaults to 8%
  wifiManager.setMinimumSignalQuality();

  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  //wifiManager.setTimeout(120);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("MEfEvaFaraAzzahra")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  //read updated parameters
  //mqtt_server = custom_mqtt_server.getValue();
  //mqtt_port = custom_mqtt_port.getValue();
  blynk_token = custom_blynk_token.getValue();


  Blynk.config(blynk_token);
  Serial.println(blynk_token);

  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    //json["mqtt_server"] = mqtt_server;
    //json["mqtt_port"] = mqtt_port;
    json["blynk_token"] = blynk_token;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    //end save
  }
  */
  //-----------------------------------------------------------------------------------------------
    
}


void Processo::terminalClear() {
    terminal.clear();
}

void Processo::println(String text) {
     terminal.println(text);
     terminal.flush();
}

void Processo::print(String text) {
    terminal.print(text);
    terminal.flush();
}

void Processo::configModeCallback(WiFiManager *wm){
     Serial.println("Entrou no modo de configuração");
     Serial.println(WiFi.softAPIP());
     Serial.println(wm->getConfigPortalSSID());     
}

void Processo::saveConfigCallback(){
     Serial.println("Configuração Salva.");
}

void Processo::connecting(){
     wifiManager.setAPCallback(configModeCallback);
     wifiManager.setSaveConfigCallback(saveConfigCallback); 
     Serial.println("Abertura Portal"); //Abre o portal
     digitalWrite(26,HIGH); //Acende LED Vermelho
     digitalWrite(32,LOW);
     wifiManager.resetSettings();       //Apaga rede salva anteriormente
      if(!wifiManager.startConfigPortal("JJ Cientifica", "12345678") )
        { //Nome da Rede e Senha gerada pela ESP
        Serial.println("Falha ao conectar"); //Se caso não conectar na rede mostra mensagem de falha
        delay(2000);
        ESP.restart(); //Reinicia ESP após não conseguir conexão na rede
        }
      else{       //Se caso conectar 
        Serial.println("Conectado na Rede!!!");
        ESP.restart(); //Reinicia ESP após conseguir conexão na rede        
      }

 
   if(WiFi.status()== WL_CONNECTED){ //Se conectado na rede
      digitalWrite(32,HIGH); //Acende LED AZUL
   }
   else{ //se não conectado na rede
      digitalWrite(32,LOW); //Apaga LED AZUL
      //Pisca LED Vermelho
      digitalWrite(26,HIGH);
      delay(500);
      digitalWrite(26,LOW);
      delay(500);
      wifiManager.autoConnect();//Função para se autoconectar na rede
   }    


  //callback para quando entra em modo de configuração AP
  //wifiManager.setAPCallback(0); //configModeCallback
  //callback para quando se conecta em uma rede, ou seja, quando passa a trabalhar em modo estação
  //wifiManager.setSaveConfigCallback(0);//saveConfigCallback  

}

void Processo::run() {
	 Blynk.run(); 
}

bool flag_brx;
 String bufferBlynk;
 BLYNK_WRITE(V10)
 {
     bufferBlynk = param.asString();
     flag_brx = true;
 }
 
 BLYNK_READ(V0)
 {
     Blynk.virtualWrite(V0, SensoresAtuadores[0].valorDeLeitura(COMPOSTO));
 }

 BLYNK_READ(V1)
 {
     Blynk.virtualWrite(V1, SensoresAtuadores[1].valorDeLeitura(COMPOSTO));
 }

 BLYNK_READ(V2)
 {
     Blynk.virtualWrite(V2, SensoresAtuadores[2].valorDeLeitura(COMPOSTO));
 }

 BLYNK_READ(V3)
 {
     Blynk.virtualWrite(V3, SensoresAtuadores[3].valorDeLeitura(COMPOSTO));
 }




Processo &Processo = Processo.getInstance();
