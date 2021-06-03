#include "Blk_BT.h"
#define BLYNK_PRINT Serial
#define BLYNK_USE_DIRECT_CONNECT
#include <BlynkSimpleEsp32_BLE.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include "Controladores.h"
#include "global.h"

char auth[] = "Lq7xXgIvMAK1heKtLH4fJ-s2iS6bfPp-";



extern Controladores SensoresAtuadores[MAXDEVICE];


//Attach virtual serial terminal to Virtual Pin V1
WidgetTerminal terminal(V10);




void Blk_BT_Class::init()
{
  Blynk.setDeviceName("AAABBB"); 
  Blynk.begin(auth);
}

void Blk_BT_Class::terminalClear() {
     terminal.clear();
}

void Blk_BT_Class::println(String text) {
     terminal.println(text);
     terminal.flush();
}

void Blk_BT_Class::print(String text) {
     terminal.print(text);
     terminal.flush();
}


void Blk_BT_Class::run() {
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
 
 
Blk_BT_Class Blk_terminal;


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
