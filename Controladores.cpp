

#include "Global.h"
#include "Controladores.h"
#include "Eeprom_Indka.h"



extern Controladores SensoresAtuadores[MAXDEVICE];
extern uint16_t tempoCNT[MAXDEVICE];
extern Eeprom_Indka hardDisk ;


float  Controladores::valorDeLeitura(uint8_t tipo) {
       if (tipo == SIMPLES)
           return value;
       else if (tipo == COMPOSTO)
           return (value + offset);                         
       }


//------------------------------------------------------------------------------
void Controladores::relayManager(uint8_t device, uint8_t situacao)
{
    uint8_t auxiliar; 
    //auxiliar=pow(2, device);    
    auxiliar=1;
    auxiliar=(auxiliar<<device);



    //Serial.println();
    //Serial.println(DEVICE[device]);
    //Serial.println(tempoCNT[device]);
  

    if(situacao==HIGH) //Ligado
      {
        if(tempoCNT[device]==0)
          {
          if(relayStado(device)==false) //RELE DESLIGADO ? 
            {
              tempoCNT[device]= hardDisk.EEPROMReadFloat(20 * device + 0x0C);
              if(SensoresAtuadores[device].tempo_ON!=0)
                 {
                 auxiliar=~auxiliar;
                 extra74HC595.chip.value &= auxiliar; //LIGAR RELÊ
                 }
            }
          else
            {
              tempoCNT[device]=hardDisk.EEPROMReadFloat(20 * device + 0x10);
              if(SensoresAtuadores[device].tempo_OFF!=0)
                {                 
                extra74HC595.chip.value |= auxiliar; //Desligar RELÊ
                } 
            }  
          }
      }
    else if(situacao==LOW)
      {
      	extra74HC595.chip.value |= auxiliar; //Desligar Relê
        //tempoCNT[device]=0;        
      }
}



//----------------------------------------------------------------------------
//Faz leitura se o relê especificado está ligado ou desligado
bool Controladores::relayStado(uint8_t device){
     bool retorno;
     uint8_t auxiliar;
     auxiliar=pow(2,device);

     auxiliar = auxiliar & ~extra74HC595.chip.value;
     if(auxiliar>0) retorno=true; else retorno=false;

	 return retorno;
}
