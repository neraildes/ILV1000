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

#endif
