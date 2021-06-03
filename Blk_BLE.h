

#ifndef _BLK_BLE_H
#define _BLK_BLE_H

#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else
  #include "WProgram.h"
#endif

class Blk_BLE_Class
{
 public:
  void init();
  void terminalClear();
  void println(String text);
  void print(String text);
  void run();
  void showData();
 protected:
};

#endif
