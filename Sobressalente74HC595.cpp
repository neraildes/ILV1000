#include "Global.h"
#include "Sobressalente74HC595.h"
#include <Eeprom.h>


void Sobressalente74HC595::init() {
	 desligaAll(); 
}

void Sobressalente74HC595::chipSetValue(uint8_t pin, uint8_t estado) {
	switch (pin) 
	{
	case PIN_BUZZER:
		 chip.buzzer = (uint8_t)!estado;
		 break;
	case PIN_CONDENSADOR:		 
		 chip.rele_condensador = (uint8_t)estado;
		 break;
	case PIN_SENSORNTC:
	     chip.rele_sensor_ntc = (uint8_t)estado;
		 chip.rele_comum = (uint8_t)estado;
		 break;
	case PIN_VACCUM:
		 chip.rele_vaccum = (uint8_t)estado;
		 break;
	case PIN_COMUM:
		 chip.rele_comum = (uint8_t)estado;
		 break;
	case PIN_RESERVED5:
	case PIN_RESERVED6:
	case PIN_RESERVED7:
		 break;
	}
	//if(pin!=PIN_BUZZER) gravaChip(chip.value);
}


void Sobressalente74HC595::desligaAll() {
	 chip.value = 0b00011110;
	 //gravaChip(chip.value);
}

/*
void Sobressalente74HC595::gravaChip(uint8_t value){
	 EEPROM.write(ADD_74HC595_FLAGS, value);
}
*/
