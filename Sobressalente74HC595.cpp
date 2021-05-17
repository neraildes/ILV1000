#include "Global.h"
#include "Sobressalente74HC595.h"
#include <Eeprom.h>

extern Sobressalente74HC595 extra74HC595;


void Sobressalente74HC595::init() {
	   desligaAll(); 
}

/*
void Sobressalente74HC595::chipSetValue(uint8_t pin, uint8_t estado) {
	switch (pin) 
	{
	case PIN_BUZZER:
		 relay_buzzer = !estado;
		 break;
	case PIN_CONDENSADOR:		 
		 relay_condensador = estado;
		 break;
	case PIN_SENSORNTC:
	   relay_ntc = estado;
		 break;
	case PIN_VACCUM:
		 relay_vaccum = estado;
		 break;
	case PIN_COMUM:
		 relay_comum = (uint8_t)estado;
		 break;
	case PIN_RESERVED5:
	case PIN_RESERVED6:
	case PIN_RESERVED7:
		 break;
	}
	//if(pin!=PIN_BUZZER) gravaChip(chip.value);
  
}
*/

void Sobressalente74HC595::desligaAll() {
	   chip.value = 0b00011110;
}
