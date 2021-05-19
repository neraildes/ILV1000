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
#define RELAY_LIGADO_SOBE         30 //_____DEFINE A FUNCÃO DO ATUADOR.
#define RELAY_LIGADO_DESCE        40 //
#define Relay_Status(x)           SensoresAtuadores[(x)].power.relayStatus  //NORMAL, PISCA, DINAMICO
#define Relay_Power(x, y)         SensoresAtuadores[(x)].power.powerRelay(x, y) //HIGH ou LOW (Liga/Desliga)
#define Relay_Valor_Simples(x)    SensoresAtuadores[(x)].valorDeLeitura(SIMPLES) //Valor real puro
#define Relay_Valor_Composto(x)   SensoresAtuadores[(x)].valorDeLeitura(COMPOSTO) //Valor real mais o offset
#define Relay_SetPoint(x)         SensoresAtuadores[(x)].setpoint  //setpoint
#define Relay_Histerese(x)        SensoresAtuadores[(x)].histerese //histerese
#define Relay_OffSet(x)           SensoresAtuadores[(x)].offset //offset
#define Relay_Setagem_Baixa(x)    SensoresAtuadores[(x)].setpoint-SensoresAtuadores[(x)].histerese //Limite inferior
#define Relay_Setagem_Alta(x)     SensoresAtuadores[(x)].setpoint+SensoresAtuadores[(x)].histerese //Limite Superior
#define Relay_Modo(x)             SensoresAtuadores[(x)].modo  //LIGADO_SOBE e LIGADO_DESCE

typedef struct {
               bool    relayStatus;
               uint8_t relayPin;
               void powerRelay(int8_t device, uint8_t funcao)
                    {
                        
                        //================ CONTROLE DE TEMPO DO RELE 0 =================
                        if (Relay_Valor_Composto(device) <= Relay_Setagem_Baixa(device))
                        {
                            if (Relay_Modo(device) == RELAY_LIGADO_SOBE)
                            {
                                Relay_Status(device) = RELAY_SOBE;
                                Relay_Power(device, HIGH);
                            }
                            else
                                if (Relay_Modo(device) == RELAY_LIGADO_DESCE)
                                {
                                    Relay_Status(device) = RELAY_DESCE;
                                    Relay_Power(device, LOW);
                                }
                        }
                        else if (Relay_Valor_Composto(device) > Relay_SetPoint(device))
                        {
                            if (Relay_Modo(device) == RELAY_LIGADO_SOBE)
                            {
                                Relay_Status(device) = RELAY_LIGADO_DESCE;
                                Relay_Power(device, LOW);
                            }
                            else
                                if (Relay_Modo(device) == RELAY_REFRIGERAR)
                                {
                                    Relay_Status(device) = RELAY_SOBE;
                                    Relay_Power(device, HIGH);
                                }
                        }













                    
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
               float   value=0;       //propriedade que recebe o valor real de leitura
               float   setpoint=0;    //SetPoint do dispositivo
               float   histerese = 0; //Histerese do Dispositivo
               float   offset = 0;    //Offset do Dispositivo
               float   tempo_ON;      //Tempo de relê ligado
               float   tempo_OFF;     //Tempo de relê desligado
               float   temp=0;        //Valor temporário a ser exibido no display
               int8_t  status= NORMAL;//Status que controla a exibição no display (NORMAL/PISCA/DINAMICO)
               int8_t  sentido;       //RELAY_SOBE ou RELAY_DESCE;
               char  mensagem[10];    //Mensagem principal a ser exibida no display
               char  mensagem1[10];   //Mensagem secundária a ser exibida no display
               uint8_t modo;          //RELAY_LIGADO_SOBE ou RELAY_LIGADO_DESCE
               //tpower  power;         //Método que aciona o rele do dispositivo
               float valorDeLeitura(uint8_t tipo); //Metodo que retorna o valor lido SIMPLES ou COMPOSTO.
               void autoRelay(uint8_t device); //Faz relê ciclar em milisegundos e mantém temperatura
               void relayManager(uint8_t device, uint8_t situacao);
               uint8_t relayStado(uint8_t device);
               };





#endif
