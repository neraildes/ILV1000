#include "Global.h"
#include "Controladores.h"

float  Controladores::valorDeLeitura(uint8_t tipo) {
       if (tipo == SIMPLES)
           return value;
       else if (tipo == COMPOSTO)
           return (value + offset);                         
       }
