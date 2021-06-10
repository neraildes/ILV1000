#include "singleton.h"
#include "global.h"
#include <stddef.h>
#include <arduino.h>

Singleton &Singleton::getInstance()
{      
  static Singleton _instance;
  return _instance;
}

Singleton &Singleton = Singleton.getInstance();
