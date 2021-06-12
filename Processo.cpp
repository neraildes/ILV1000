#include "processo.h"
#include "global.h"
#include <stddef.h>
#include <arduino.h>
#include "Controladores.h"

#include "persistente.h"

#include <HTTPClient.h>
#include <DNSServer.h> 
#include <WebServer.h>
#include "BlynkSimpleEsp32.h"
#include <WiFiManager.h>


char auth[] = "Pd9SZNodRX688FDh_kTitt-nOUkYIrnh";
char ssid[] = "Indka";
char pass[] = "iub950962";


#define BLYNK_PRINT Serial
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
  Blynk.begin(auth, ssid, pass);
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
        //ESP.restart(); //Reinicia ESP após conseguir conexão na rede        
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
