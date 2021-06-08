#include "Controladores.h"
#include "global.h"

 


#ifdef COM_BLYNK_WIFI
#include "Blk_terminal.h"
#define BLYNK_PRINT Serial
#include "WiFi.h"
#include "WiFiClient.h"
#include "BlynkSimpleEsp32.h"

char auth[] = "Pd9SZNodRX688FDh_kTitt-nOUkYIrnh";
char ssid[] = "Nera";
char pass[] = "iub950962";
WidgetTerminal terminal(V10);

WiFiManager wifiManager;//Objeto de manipulação do wi-fi

extern bool Flag_SetWifi;

extern Controladores SensoresAtuadores[MAXDEVICE];

void Blk_terminalClass::init()
{
  Blynk.begin(auth, ssid, pass); 
}

void Blk_terminalClass::terminalClear() {
    terminal.clear();
}

void Blk_terminalClass::println(String text) {
     terminal.println(text);
     terminal.flush();
}

void Blk_terminalClass::print(String text) {
    terminal.print(text);
    terminal.flush();
}


void Blk_terminalClass::run() {
	 Blynk.run();
}


// You can send commands from Terminal to your hardware. Just use
// the same Virtual Pin as your Terminal Widget
 bool flag_brx;
 String bufferBlynk;
 BLYNK_WRITE(V10)
 {
     bufferBlynk = param.asString();
     flag_brx = true;
 }
 
 
 Blk_terminalClass Blk_terminal;


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



void Blk_terminalClass::connecting(){
 
      Serial.println("Abertura Portal"); //Abre o portal
      digitalWrite(26,HIGH); //Acende LED Vermelho
      digitalWrite(32,LOW);
      wifiManager.resetSettings();       //Apaga rede salva anteriormente
      if(!wifiManager.startConfigPortal("JJ Cientifica", "12345678") ){ //Nome da Rede e Senha gerada pela ESP
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
}




//callback que indica que o ESP entrou no modo AP
void Blk_terminalClass::configModeCallback(WiFiManager *myWiFiManager) {  
  Serial.println("Entrou no modo de configuração");
  Serial.println(WiFi.softAPIP()); //imprime o IP do AP
  Serial.println(myWiFiManager->getConfigPortalSSID()); //imprime o SSID criado da rede
}
 
//Callback que indica que salvamos uma nova rede para se conectar (modo estação)
void Blk_terminalClass::saveConfigCallback() {
  Serial.println("Configuração salva");
} 




void Blk_terminalClass::s1(){
  //callback para quando entra em modo de configuração AP  
  wifiManager.setAPCallback(0);  
}

void Blk_terminalClass::s2(){
  //callback para quando se conecta em uma rede, ou seja, quando passa a trabalhar em modo estação
  wifiManager.setSaveConfigCallback(0);   
}

 
















 

#endif
