#include "Global.h"
#include "EEPROM.h"
#include "Persistente.h"
#include "Sobressalente74HC595.h"


extern Sobressalente74HC595 extra74HC595;

extern uint8_t memoFlags;

Persistente::Persistente(){
  
}

void Persistente::init(){
     restore(); //Restaura StatusGen.
     extra74HC595.chip.value = statusgen.value;
     memoFlags = statusgen.value; 
}

void Persistente::save(){
  EEPROM.write(ADD_STATUSGEN, statusgen.value);
  EEPROM.write(ADD_HORA_PROCESSO, processoTime.hora);
  EEPROM.write(ADD_MINUTO_PROCESSO, processoTime.minuto);
  EEPROM.commit();
}

void Persistente::restore(){
  statusgen.value = EEPROM.read(ADD_STATUSGEN);
  processoTime.hora = EEPROM.read(ADD_HORA_PROCESSO);
  processoTime.minuto = EEPROM.read(ADD_MINUTO_PROCESSO);
}
