#include "Global.h"
#include "Controladores.h"

float  Controladores::valorDeLeitura(uint8_t tipo) {
       if (tipo == SIMPLES)
           return value;
       else if (tipo == COMPOSTO)
           return (value + offset);                         
       }


//--------------------------------------------------------------------
void  autoRelay(uint8_t device)
{

      //================ CONTROLE DE TEMPO DO RELE 0 =================
      if (Relay_Valor_Composto(device) <= Relay_Setagem_Baixa(device))
      {
          if (modo == RELAY_LIGADO_SOBE)
          {
              sentido = RELAY_SOBE;
              Relay_Manager(device, HIGH);  //ligar
          }
          else
              if (modo == RELAY_LIGADO_DESCE)
              {
                  sentido = RELAY_DESCE;
                  Relay_Manager(device, LOW);  //Desligar
              }
      }
      else if (Relay_Valor_Composto(device) > Relay_SetPoint(device))
      {
          if (modo == RELAY_LIGADO_SOBE)
          {
              sentido = RELAY_DESCE;
              Relay_Manager(device, LOW); //DESLIGA
          }
          else
              if (modo == RELAY_LIGADO_DESCE)
              {
                  sentido = RELAY_SOBE;
                  Relay_Manager(device, HIGH); //LIGA
              }
      }  
}


//------------------------------------------------------------------------------
void relayManager(uint8_t device, uint8_t situacao)
{
    uint8_t auxiliar; 
    auxiliar=pow(2, device);

    if(situação==HIGH) //Ligado
      {
        if(tempoCNT==0)
          {
          if(relayStado(device)==false) //RELE DESLIGADO ? 
            {
              tempoCNT= hardDisk.EEPROMReadFloat(20 * device + 0x0C);
              extra74HC595.chip.value &= ~auxiliar; //LIGAR RELÊ
            }
          else
            {
              tempoCNT=hardDisk.EEPROMReadFloat(20 * device + 0x10);
              extra74HC595.chip.value |= auxiliar; //Desligar RELÊ
            }  
          }
      }
    else if(situação==LOW)
      {
      	extra74HC595.chip.value |= auxiliar; //Desligar Relê
      }  
                      
}


/*
if(funcao==HIGH)
  {     
  extra74HC595.chip.value &= ~auxiliar;
  }                      
else if(funcao==LOW)
       {                          
       extra74HC595.chip.value |= auxiliar;
       } 
*/



//----------------------------------------------------------------------------
//Faz leitura se o relê especificado está ligado ou desligado
bool relayStado(uint8_t device){
     bool retorno;
     uint8_t auxiliar;
     auxiliar=pow(2,device);

     auxiliar = auxiliar & ~extra74HC595.chip.value;
     if(auxiliar>0) retorno=true; else retorno=false;

	 return retorno;
}