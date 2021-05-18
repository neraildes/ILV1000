//#include "Arduino.h"
#include <eeprom.h>
#include "EEPROM_Indka.h"

extern uint16_t buzzer;

void Eeprom_Indka::EEPROMWriteFloat(uint16_t address, float dado)
       {       
       EEPROM.write(address+0, *((uint8_t*)&dado+0));
       EEPROM.write(address+1, *((uint8_t*)&dado+1));
       EEPROM.write(address+2, *((uint8_t*)&dado+2));
       EEPROM.write(address+3, *((uint8_t*)&dado+3));       
       EEPROM.commit();
       }

float Eeprom_Indka::EEPROMReadFloat(uint16_t address)
       {       
       float resultado=0;
       *(((uint8_t*)&resultado)+0) = EEPROM.read(address+0);
       *(((uint8_t*)&resultado)+1) = EEPROM.read(address+1);
       *(((uint8_t*)&resultado)+2) = EEPROM.read(address+2);
       *(((uint8_t*)&resultado)+3) = EEPROM.read(address+3);  
       return resultado;
       }
