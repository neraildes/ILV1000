// Blk_terminal.h

#ifndef _BLK_TERMINAL_h
#define _BLK_TERMINAL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <HTTPClient.h>
#include <DNSServer.h> 
#include <WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

class Blk_terminalClass
{
 protected:
 public:
	void init();
	void terminalClear();
	void println(String text);
	void print(String text);
	void run();
	void output();
	void showData();
  void connecting();
  void configModeCallback (WiFiManager *myWiFiManager);
  void saveConfigCallback();  
  void s1();
  void s2();
};

extern Blk_terminalClass Blk_terminal;

#endif
