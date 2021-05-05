#include "Arduino.h"
#include "AD_Converter.h"
#include "global.h"

extern uint16_t buzzer;

AD_Converter::AD_Converter() {
    
};

float AD_Converter::read_pin(uint8_t pin, uint8_t modo) {
    float ADCvalue;
    float retorno;
    pinMode(pin, INPUT);
    
    switch(modo)
    {
    case AD_MINIMO:
                  retorno  = 4000.0;
                  ADCvalue = 4000.0;
                  for (uint8_t contador = 0; contador < CICLOS_DE_MEDIA; contador++)
                  {
                      ADCvalue = analogRead(pin);
                      if(ADCvalue<retorno)retorno=ADCvalue;
                  }                  
                  //ADCvalue /= CICLOS_DE_MEDIA;    
                  break;    
    case AD_MEDIO:
                  ADCvalue = 0;
                  for (uint8_t contador = 0; contador < CICLOS_DE_MEDIA; contador++)
                  {
                      ADCvalue += analogRead(pin);
                  }
                  ADCvalue /= CICLOS_DE_MEDIA;    
                  retorno = ADCvalue;
                  break;
    case AD_MAXIMO:  
                  retorno  = 0;
                  ADCvalue = 0;
                  for (uint8_t contador = 0; contador < CICLOS_DE_MEDIA; contador++)
                  {
                      ADCvalue = analogRead(pin);
                      if(ADCvalue>retorno)retorno=ADCvalue;
                  }                  
                  //ADCvalue /= CICLOS_DE_MEDIA;    
                  break;    
    }    
    return retorno;
};

float AD_Converter::suaviza(float entrada, uint8_t device) {
      static Suaviza_t suaviza[MAXDEVICE];

 	  float total;
	  int8_t i;

      for (i = MAXSUAVIZA-1; i > 0; i--) suaviza[device].value[i] = suaviza[device].value[i - 1];
      suaviza[device].value[0] = entrada;

      total = 0;
      for (i = 0; i < suaviza[device].count; i++) total += suaviza[device].value[i]; //Soma todos os valores

	  if (suaviza[device].count > 0) total /= (suaviza[device].count); //Divide pelo total
 
 	  if (suaviza[device].count < MAXSUAVIZA) suaviza[device].count++;

      //if (suaviza[device].count != MAXSUAVIZA)
      //{
      //    return entrada;
      //}         
      //else
      //{
          return total;
      //}
};
