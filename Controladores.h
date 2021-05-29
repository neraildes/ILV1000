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

//----------------SENTIDO DO RELE-------------------------
#define RELAY_SOBE  11
#define RELAY_DESCE 22


//------------------------controle de potencia utilizando os reles---------------------------------------
#define RELAY_SUBINDO             10 //_____STATUS DA GRANDEZA
#define RELAY_DESCENDO            20 //
#define RELAY_LIGADO              30 //_____DEFINE A FUNCÃO DO ATUADOR.
#define RELAY_DESLIGADO           40 //
//#define Relay_Status(x)           SensoresAtuadores[(x)].power.relayStatus  //NORMAL, PISCA, DINAMICO
//#define Relay_Power(x, y)         SensoresAtuadores[(x)].powerRelay(x, y) //HIGH ou LOW (Liga/Desliga)
#define Relay_Valor_Simples(x)    SensoresAtuadores[(x)].valorDeLeitura(SIMPLES) //Valor real puro
#define Relay_Valor_Composto(x)   SensoresAtuadores[(x)].valorDeLeitura(COMPOSTO) //Valor real mais o offset
#define Relay_SetPoint(x)         SensoresAtuadores[(x)].setpoint  //setpoint
#define Relay_Histerese(x)        SensoresAtuadores[(x)].histerese //histerese
#define Relay_OffSet(x)           SensoresAtuadores[(x)].offset //offset
#define Relay_Tempo_ON(x)         SensoresAtuadores[(x)].tempo_ON //Tempo do relê ligado
#define Relay_Tempo_OFF(x)        SensoresAtuadores[(x)].tempo_OFF //Tempo do relê desligado
#define Relay_Setagem_Baixa(x)    SensoresAtuadores[(x)].setpoint-SensoresAtuadores[(x)].histerese //Limite inferior
#define Relay_Setagem_Alta(x)     SensoresAtuadores[(x)].setpoint+SensoresAtuadores[(x)].histerese //Limite Superior
#define Relay_Modo(x)             SensoresAtuadores[(x)].modo  //LIGADO_SOBE e LIGADO_DESCE
#define Relay_Estado(x)           SensoresAtuadores[(x)].estado //RELAY_LIGADO ou RELAY_DESLIGADO

class  Controladores {
          public:
               float   value=0;       //propriedade que recebe o valor real de leitura
               float   setpoint=0;    //SetPoint do dispositivo
               float   histerese = 0; //Histerese do Dispositivo
               float   offset = 0;    //Offset do Dispositivo
               float   tempo_ON;      //Tempo de relê ligado
               float   tempo_OFF;     //Tempo de relê desligado
               float   temp=0;        //Valor temporário a ser exibido no display
               int8_t  status= NORMAL;//Status que controla a exibição no display (NORMAL/PISCA/DINAMICO)
               int8_t  estado;        //RELAY_SOBE ou RELAY_DESCE;
               char  mensagem[10];    //Mensagem principal a ser exibida no display
               char  mensagem1[10];   //Mensagem secundária a ser exibida no display
               //uint8_t modo;          //RELAY_LIGADO_SOBE ou RELAY_LIGADO_DESCE               
               float valorDeLeitura(uint8_t tipo); //Metodo que retorna o valor lido SIMPLES ou COMPOSTO.
               //void autoRelay(uint8_t device); //Faz relê ciclar em milisegundos e mantém temperatura
               void relayManager(uint8_t device, uint8_t situacao);
               bool relayStado(uint8_t device);
               void Relay_Power(uint8_t device, bool state);
               };



#endif
