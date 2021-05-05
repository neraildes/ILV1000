#ifndef EEPROM_INDKA_H
#define EEPROM_INDKA_H

#include "Arduino.h"
#include "EEPROM_INDKA.h"
#include <eeprom.h>





class Eeprom_Indka
      {
      public:          
        void  EEPROMWriteFloat(uint16_t, float);
        float EEPROMReadFloat(uint16_t);
      private:  
      };

#endif