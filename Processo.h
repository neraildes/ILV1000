#pragma once

#ifndef PROCESSO_H
#define PROCESSO_H



#include "global.h"
#include "arduino.h"

#include "WiFi.h"
#include "WiFiClient.h"
//#include "BlynkSimpleEsp32.h"


//WidgetTerminal terminal(V10);


#include <HTTPClient.h>
#include <DNSServer.h> 
#include <WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager


class Processo : public WiFiManager
                 // public Blynk         
{
   private:
       Processo() = default; 
        
   public:
       static Processo &getInstance();       
       Processo(const Processo &) = delete; // no copying
       Processo &operator=(const Processo &) = delete; 
       //- - - - - - - - - - - - - - - - - - - - - - - - - -
       uint16_t value;

       void init();
       void terminalClear();
       void println(String text);
       void print(String text);
       void run();
       void connecting();
       void static saveConfigCallback();       
       void static configModeCallback(WiFiManager *wm);
};

extern Processo &processo;

#endif
