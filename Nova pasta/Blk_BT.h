

#ifndef _BLK_BT_H
#define _BLK_BT_H

#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else
  #include "WProgram.h"
#endif

class Blk_BT_Class
{
 protected:


 public:
  void init();
  void terminalClear();
  void println(String text);
  void print(String text);
  void run();
  void showData();
};

#endif
