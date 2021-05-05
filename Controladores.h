#ifndef CONTROLADORES_H
#define CONTROLADORES_H

#include "Arduino.h"
#include "Sobressalente74HC595.h"
#include "Global.h"
#include "math.h"
extern Sobressalente74HC595 extra74HC595;

//---------TIPO DE LEITURA DE TEMPERATURA----------------
#define SIMPLES  100
#define COMPOSTO 200

//------------------------controle de potencia utilizando os reles---------------------------------------
#define RELAY_SOBE                10
#define RELAY_DESCE               20
#define RELAY_ESTADO_INATIVO      30
#define RELAY_ESTADO_ATIVO        40
#define RELAY_AQUECER             50
#define RELAY_REFRIGERAR          60
#define Relay_Status(x)           SensoresAtuadores[(x)].power.relayStatus  //SOBE ou DESCE
#define Relay_Power(x, y)         SensoresAtuadores[(x)].power.powerRelay(x, y) //HIGH ou LOW (Liga/Desliga)
#define Relay_Valor_Simples(x)    SensoresAtuadores[(x)].valorDeLeitura(SIMPLES) //Falor real mais o offset
#define Relay_Valor_Composto(x)   SensoresAtuadores[(x)].valorDeLeitura(COMPOSTO) //Falor real mais o offset
#define Relay_SetPoint(x)         SensoresAtuadores[(x)].setpoint  //setpoint
#define Relay_Histerese(x)        SensoresAtuadores[(x)].histerese //histerese
#define Relay_OffSet(x)           SensoresAtuadores[(x)].offset //offset
#define Relay_Setagem_Baixa(x)    SensoresAtuadores[(x)].setpoint-SensoresAtuadores[(x)].histerese //Limite inferior
#define Relay_Setagem_Alta(x)     SensoresAtuadores[(x)].setpoint+SensoresAtuadores[(x)].histerese //Limite Superior
#define Relay_Modo(x)             SensoresAtuadores[(x)].modo  //Aquece ou Resfria

typedef struct {
               bool    relayStatus;
               uint8_t relayPin;
               void powerRelay(int8_t pin, uint8_t funcao)
                    {
                    uint8_t auxiliar; 
                    auxiliar=pow(2,pin);
                    if(funcao==HIGH)
                      {     
                      extra74HC595.chip.value &= ~auxiliar;
                      }
                    else if(funcao==LOW)
                           {                          
                           extra74HC595.chip.value |= auxiliar;
                           }                                             
                    }
               uint8_t relaySentido;
               uint8_t modo;
               } tpower;

class  Controladores {
          public:
               float   value=0;
               float   setpoint=0;  
               float   histerese = 0;
               float   offset = 0;                           
               float   temp=0;

               int8_t  status= NORMAL; 

               char  mensagem[10]; 
               char  mensagem1[10];
               uint8_t modo;
               tpower  power;
               float   valorDeLeitura(uint8_t tipo); 
               };





#endif
