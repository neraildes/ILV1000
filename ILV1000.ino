/*-------------------------------------------------------------------
 Projeto : Vacuometro digital
 Cliente : JJCientifica
 Data    : 15 de março de 2021
 Author  : Neraildes da Silva
 Plataforma : ARDUINO
 Placa   : ESP32 VRROM D1 WEMOS
 Device  : ESP32 DEV Module##
 Observa��es:
    Este projeto utiliza um teclado de membrana e displays 74hc595
    Utiliza-se tamb�m de terminal para alterar e visualizar parametros.
    A configura��o do terminal deve ser:
        O terminal pode ser qualquer um, por�m este foi testado no Putty.
        Baud Rate: 115200.
        BackSpace: ControlH
        Controle de fluxo: None (nenhum)
        Estudar o protocolo MQTT para Arduino (Blynk)
        TodoList
            Atualizar Help
                Incluir comando buzzer e relay.
            Fazer objeto que salva status de liofilizando e modo automatico e manual
            Acertar rotina automatica de ajuste de temperatura.
            Colocar fun��es para liofiliza��o manual. "A' e 'B'.
            Evitar Flash de liga��o de aparelho.

----------------------------------------------------------------------*/

#include "Arduino.h"
#include "global.h"
#include "Thread.h"
#include "ThreadController.h"
#include "Shell.h"
#include "EEPROM.h"
#include "Modulo595.h"
#include "Keypad.h"
#include "key.h"
#include "Eeprom_Indka.h"
#include "Sobressalente74HC595.h"
//#include <stdlib.h>
#include "AD_Converter.h"
#include "Persistente.h"
#include "Controladores.h"
#include "versao.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
//#include <esp_ipc.h>

#include "EmonLib.h"             // Include Emon Library
EnergyMonitor emon1;             // Create an instance


char versao[]=FVERSION ;

extern bool flag_multicore;

Sobressalente74HC595 extra74HC595;

Persistente persistente;

//variaveis que indicam o n�cleo
static uint8_t taskCoreZero = 0;
static uint8_t taskCoreOne  = 1;


//-------------------------------------------
hw_timer_t* timer = NULL; // Para o Timer Zero

Controladores SensoresAtuadores[MAXDEVICE];

uint32_t tempoDecorrido = 0;

int DIO  = 26 ;  //Data 
int SCLK = 17 ; //Clock 
int RCLK = 16 ; //Lath 

String blynkDisplay[MAXDEVICE];
Eeprom_Indka hardDisk = Eeprom_Indka();
Modulo595 seteSegmentos(DIO, SCLK, RCLK);


Shell comandos; 


uint8_t enterDoKeypad = 0;


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//             T E C L A D O   D E   M E M B R A N A
const byte qtdLinhas = 4; //QUANTIDADE DE LINHAS DO TECLADO
const byte qtdColunas = 4; //QUANTIDADE DE COLUNAS DO TECLADO

//CONSTRU��O DA MATRIZ DE CARACTERES
char matriz_teclas[qtdLinhas][qtdColunas] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte PinosqtdLinhas[qtdLinhas] = { 12 , 32, 25, 27 }; //PINOS UTILIZADOS PELAS LINHAS
byte PinosqtdColunas[qtdColunas] = { 14,33,15,21 }; //PINOS UTILIZADOS PELAS COLUNAS 

//INICIALIZA��O DO TECLADO
Keypad meuteclado = Keypad(makeKeymap(matriz_teclas), PinosqtdLinhas, PinosqtdColunas, qtdLinhas, qtdColunas);
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ThreadController controller_0 = ThreadController(); //Threads que rodarão no núcleo zero.
ThreadController controller_1 = ThreadController(); //Threads que rodarão no núcleo um.


uint16_t buzzer = 0;

uint8_t minutoProcesso=0;
uint8_t horaProcesso=0;
uint16_t tempoCNT[MAXDEVICE];

char tecla_pressionada = 0;
int8_t displayNumero = 0;
static float decimal = 100.0;
static float temp = 0;
bool flag_zerar = true;
uint8_t funcao = FUNCAO_NONE;
uint8_t funcaoMemo = FUNCAO_NONE;
int8_t autorestore = -1;

extern bool flag_brx;
extern String bufferBlynk;


Thread thGrava10minutos;
Thread thProcesso;
Thread thAutoRelay;
Thread thShell;
Thread thKeypad;
Thread thLerSensor;
Thread thBlynkRun;
Thread thBlynkExibe;
Thread thDisplay;
Thread thRTC;


//================Funções=========================
void startTimer();
void coreTaskZero(void* pvParameters);
void coreTaskOne(void* pvParameters);
void doKeypad();
void doLerSensor();
void doDisplay(void);
void doBlynkExibe();
void doGrava10minutos();
void doRTC();
void doBlynkRun();
void doShell();
void doProcesso();
void doAutoRelay();
void exibeRevisao();
void debugFlags();
void loadDefaultFactory();
void loadDefaultUser();





// The setup() function runs once each time the micro-controller starts
void setup()
{   
    Serial.begin(115200);
    pinMode(LED_BLINK, OUTPUT);
    pinMode(PIN_BUZZER, OUTPUT);
    pinMode(2, OUTPUT);
    pinMode(34, INPUT);
    pinMode(13, INPUT);
    pinMode(39, INPUT);
    pinMode(36, INPUT);

    seteSegmentos.clearDisplay();     
    extra74HC595.init();
    comandos.init();  
    
    EEPROM.begin(1024);
    analogReadResolution(12);
    //--------------------------------------------------
    persistente.init(); //Inicializa dados persistentes
    startTimer();
    //--------------------------------------------------  
    for (uint8_t i = 0; i < MAXDEVICE; i++) doLerSensor();
    
    
    comandos.empresa();
    comandos.blkPrintln("Aguarde, conectando...");
    exibeRevisao();
    comandos.blkPrintln("Pronto!");

  
   
   
    //--------------------------------------------------
    

    //====================== THREADS PARA O NUCLEO 0 ===========================


    thDisplay.onRun(doDisplay);
    thDisplay.setInterval(0);

    controller_0.add(&thDisplay);  //Displays de sete segmentos






    //====================== THREADS PARA O NUCLEO 1 ===========================
    thKeypad.onRun(doKeypad);
    thKeypad.setInterval(16);
        
    thBlynkRun.onRun(doBlynkRun);
    thBlynkRun.setInterval(2);


    thLerSensor.onRun(doLerSensor);
    thLerSensor.setInterval(500);

    thBlynkExibe.onRun(doBlynkExibe);
    thBlynkExibe.setInterval(1000);

    thShell.onRun(doShell);
    thShell.setInterval(15);

    thRTC.onRun(doRTC);
    thRTC.setInterval(1);

    //-------------------------------Métodos ligados a processo------------------------------------------
    thProcesso.onRun(doProcesso);
    thProcesso.setInterval(350);
    //thProcesso.enabled=false;

    thGrava10minutos.onRun(doGrava10minutos);
    thGrava10minutos.setInterval(1000*60*10); //Grava após 10 minutos
    thGrava10minutos.enabled=false;

    thAutoRelay.onRun(doAutoRelay);
    thAutoRelay.setInterval(400);
    thAutoRelay.enabled=false;    
    //----------------------------------------------------------------------------------------------------    
    controller_1.add(&thBlynkRun);
    controller_1.add(&thAutoRelay);
    controller_1.add(&thLerSensor);
    controller_1.add(&thBlynkExibe);  
    controller_1.add(&thKeypad);   //Teclado de membrana 
    controller_1.add(&thShell);    //Terminal Putty
    controller_1.add(&thRTC);      //RTC
    controller_1.add(&thProcesso); //Execução de Processo
    controller_1.add(&thGrava10minutos);
    //==========================================================================



    
    if((uint32_t)hardDisk.EEPROMReadFloat(20 * 0 + 0x00)==0xFFFFFFFF) 
       loadDefaultFactory();
    else
       loadDefaultUser();   
    
    

    
    
    //cria uma tarefa que ser� executada na fun��o coreTaskTwo, com prioridade 2 e execu��o no n�cleo 0
    //coreTaskTwo: vigiar o bot�o para detectar quando pression�-lo
    xTaskCreatePinnedToCore(
        coreTaskZero,   /* fun��o que implementa a tarefa */
        "coreTaskZero", /* nome da tarefa */
        10000,      /* n�mero de palavras a serem alocadas para uso com a pilha da tarefa */
        NULL,       /* par�metro de entrada para a tarefa (pode ser NULL) */
        1,          /* prioridade da tarefa (0 a N) */
        NULL,       /* refer�ncia para a tarefa (pode ser NULL) */
        taskCoreZero);         /* N�cleo que executar� a tarefa */



    //cria uma tarefa que ser� executada na fun��o coreTaskOne, com prioridade 2 e execu��o no n�cleo 1
      //coreTaskOne: atualizar as informa��es do display
    xTaskCreatePinnedToCore(
        coreTaskOne,   /* fun��o que implementa a tarefa */
        "coreTaskOne", /* nome da tarefa */
        10000,      /* n�mero de palavras a serem alocadas para uso com a pilha da tarefa */
        NULL,       /* par�metro de entrada para a tarefa (pode ser NULL) */
        1,          /* prioridade da tarefa (0 a N) */
        NULL,       /* refer�ncia para a tarefa (pode ser NULL) */
        taskCoreOne);         /* N�cleo que executar� a tarefa */

    
    buzzer = 100;
    /*
    flag_processo_auto=false;
    flag_condensador=false;      //OK
    flag_aquecimento=false;      //OK
    flag_vacuo=false;            //OK
    flag_comum=false ;            
    while(1){
         
       if(flag_comum) 
          SensoresAtuadores[COMUM].relayManager(COMUM,HIGH);    
       else
          SensoresAtuadores[COMUM].relayManager(COMUM,LOW);
    }
    */
    
}
   




















void loop()
{
    //controller_1.run();
}




//essa fun��o ficar� mudando o estado do led a cada 1 segundo
//e a cada piscada (ciclo acender e apagar) incrementar� nossa vari�vel blinked
void coreTaskZero(void* pvParameters) {

    String taskMessage = "Task running on core ";
    taskMessage = taskMessage + xPortGetCoreID();
    //Serial.println(taskMessage);  //log para o serial monitor

    while (true) {
        controller_0.run();
        delay(4);
    }
}

//essa fun��o ser� respons�vel apenas por atualizar as informa��es no 
//display a cada 100ms
void coreTaskOne(void* pvParameters) {
    while (true) {
        controller_1.run();
        delay(1);
    }
}













//00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
//=======================================================================================================================================
void SaidaAutomatica(uint8_t acao)
{
    if (acao == SETA)
    {
        autorestore = TEMPO_MAX_EDICAO;
    }
    else if (acao == MONITORA)
    {
        if (autorestore > 0)
            autorestore--;
        else if (autorestore == 0)
        {
            autorestore = -1;
            for (uint8_t i = 0; i < MAXDEVICE; i++)
            {
                SensoresAtuadores[i].status = NORMAL;
            }
            funcao = FUNCAO_NONE;
            displayNumero = 0;
            flag_zerar = true;
            //buzzer = 100;
        }
    }
}






//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
//RTC do Sistema
uint8_t  hora = 0;
uint8_t  minuto = 0;
uint8_t  segundo = 0;
uint16_t milisegundo = 0;
void Gerenciador_de_Tempo() {
    milisegundo = 0;
    segundo++;
    SaidaAutomatica(MONITORA);
    if (segundo >= 60)
    {
        segundo = 0;
        minuto++;
        if (minuto >= 60)
        {
            minuto = 0;
            minutoProcesso++;
            hora++;
            if (hora >= 24)
            {
                hora++;
                horaProcesso++;
            }
        }

    }

}



//------------------------------------------------------------------------
void doGrava10minutos(){
     if(flag_processo_auto==true)
       {
       Serial.println("Gravando Status (Por tempo 10 minutos)...");
       persistente.save();
       delay(1000);
       }
}

//------------------------------------------------------------------------
uint8_t memoFlags; //fix deve ser inicializado em setup
void doProcesso(){
       
     //Salva estado da liofilização  
     
     if(memoFlags!=(persistente.statusgen.value & 0b11100000))
       {
       Serial.print("Gravando Status (Por mudança de estado.)...");
       Serial.println(persistente.statusgen.value, BIN);
       persistente.save();
       memoFlags=(persistente.statusgen.value & 0b11100000);
       }
      
         
       if(flag_processo_auto)  //INICIOU UM PROCESSO?
         {  
          flag_condensador=true;      //Liga o condensador
          
         if(Relay_Valor_Composto(CONDENSADOR)<Relay_Setagem_Baixa(CONDENSADOR))  
             {           
             flag_vacuo=true;
             flag_aquecimento=true;
             SensoresAtuadores[CONDENSADOR].estado=RELAY_LIGADO;
             }
         else if( Relay_Valor_Composto(CONDENSADOR)<Relay_SetPoint(CONDENSADOR))  
             if(SensoresAtuadores[CONDENSADOR].estado==RELAY_LIGADO)
               {
               flag_vacuo=true;
               }
             else
               {
               flag_vacuo=false;               
               }
         else if( Relay_Valor_Composto(CONDENSADOR)>Relay_SetPoint(CONDENSADOR))
               {
               flag_vacuo=false;
               SensoresAtuadores[CONDENSADOR].estado=RELAY_DESLIGADO;
               }


         if(Relay_Valor_Composto(VACUOMETRO)<Relay_Setagem_Baixa(VACUOMETRO))  
             {           
             flag_aquecimento=true;
             SensoresAtuadores[VACUOMETRO].estado=RELAY_LIGADO;
             }
         else if( Relay_Valor_Composto(VACUOMETRO)<Relay_SetPoint(VACUOMETRO))  
             if(SensoresAtuadores[VACUOMETRO].estado==RELAY_LIGADO)
               {
               flag_aquecimento=true;
               }
             else
               {
               flag_aquecimento=false;               
               }
         else if( Relay_Valor_Composto(CONDENSADOR)>Relay_SetPoint(CONDENSADOR))
               {
               flag_aquecimento=false;
               SensoresAtuadores[VACUOMETRO].estado=RELAY_LIGADO;
               }



               
         if(flag_condensador)SensoresAtuadores[CONDENSADOR].relayManager(CONDENSADOR,HIGH); else  SensoresAtuadores[CONDENSADOR].relayManager(CONDENSADOR, LOW);
         if(flag_vacuo)        SensoresAtuadores[VACUOMETRO].relayManager(VACUOMETRO,HIGH); else  SensoresAtuadores[VACUOMETRO].relayManager(VACUOMETRO,  LOW);       
         ControleDinamicoDeTemperatura();//if(flag_aquecimento)  SensoresAtuadores[SENSOR_NTC].relayManager(SENSOR_NTC,HIGH); else  SensoresAtuadores[SENSOR_NTC].relayManager(SENSOR_NTC,LOW);
         if(flag_comum)                  SensoresAtuadores[COMUM].relayManager(COMUM,HIGH); else  SensoresAtuadores[COMUM].relayManager(COMUM,LOW);
         }
       else          
         {        
         if(flag_condensador)SensoresAtuadores[CONDENSADOR].relayManager(CONDENSADOR,HIGH);else SensoresAtuadores[CONDENSADOR].relayManager(CONDENSADOR, LOW);         
         if(flag_vacuo) SensoresAtuadores[VACUOMETRO].relayManager(VACUOMETRO,HIGH);   else     SensoresAtuadores[VACUOMETRO].relayManager(VACUOMETRO,  LOW);            
         //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
         if(flag_aquecimento) 
            {
            ControleDinamicoDeTemperatura();
            }
         else 
            {
            SensoresAtuadores[SENSOR_NTC].relayManager(SENSOR_NTC,LOW);
            flag_comum=false;
            }
            
         if(flag_comum==true) SensoresAtuadores[COMUM].relayManager(COMUM,HIGH); else SensoresAtuadores[COMUM].relayManager(COMUM,LOW);
         }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
void ControleDinamicoDeTemperatura()
{
     if(flag_aquecimento)
       {
        if(Relay_Valor_Composto(SENSOR_NTC)<Relay_Setagem_Baixa(SENSOR_NTC)) 
          {
          SensoresAtuadores[SENSOR_NTC].relayManager(SENSOR_NTC,HIGH); 
          SensoresAtuadores[SENSOR_NTC].estado=RELAY_LIGADO;
          flag_comum=true;    
          }
        else
        if(Relay_Valor_Composto(SENSOR_NTC)<Relay_SetPoint(SENSOR_NTC))
          {
          if(SensoresAtuadores[SENSOR_NTC].estado==RELAY_LIGADO)
            {
            SensoresAtuadores[SENSOR_NTC].relayManager(SENSOR_NTC,HIGH);      
            flag_comum=true;
            }
          else
            {
            SensoresAtuadores[SENSOR_NTC].relayManager(SENSOR_NTC,LOW);    
            flag_comum=false;
            }
          }              
        else 
        if(Relay_Valor_Composto(SENSOR_NTC)>Relay_SetPoint(SENSOR_NTC)) 
          {
          SensoresAtuadores[SENSOR_NTC].relayManager(SENSOR_NTC,LOW);  
          SensoresAtuadores[SENSOR_NTC].estado=RELAY_DESLIGADO;
          flag_comum=false;      
          }
       }
        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
}






//--------------------------------------------------------------------------------------------------------------------------------------------------------------
void doAutoRelay() {
    static uint8_t escalona=1;
    /*
    if (Relay_Status(escalona) == RELAY_ESTADO_ATIVO)
    {
        //================ CONTROLE DE TEMPO DO RELE 0 =================
        if (Relay_Valor_Composto(escalona) <= Relay_Setagem_Baixa(escalona))
        {
            if (Relay_Modo(escalona) == RELAY_AQUECER)
            {
                Relay_Status(escalona) = RELAY_SOBE;
                Relay_Power(escalona, HIGH);
            }
            else
                if (Relay_Modo(escalona) == RELAY_REFRIGERAR)
                {
                    Relay_Status(escalona) = RELAY_DESCE;
                    Relay_Power(escalona, LOW);
                }
        }
        else if (Relay_Valor_Composto(escalona) > Relay_Setagem_Alta(escalona))
        {
            if (Relay_Modo(escalona) == RELAY_AQUECER)
            {
                Relay_Status(escalona) = RELAY_DESCE;
                Relay_Power(escalona, LOW);
            }
            else
                if (Relay_Modo(escalona) == RELAY_REFRIGERAR)
                {
                    Relay_Status(escalona) = RELAY_SOBE;
                    Relay_Power(escalona, HIGH);
                }
        }
    }
    */
    escalona++;
    if (escalona >= MAXDEVICE) escalona = 0;
}


//========================================================================
void doShell() {
     comandos.prompt();
}







//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

void timerRTC() {
     Gerenciador_de_Tempo(); //alta precis�o
}


void startTimer() {
    // inicializa��o do timer.Parametros:
    /* 0 - sele��o do timer a ser usado, de 0 a 3.
      80 - prescaler. O clock principal do ESP32 � 80MHz. Dividimos por 80 para ter 1us por tick.
    true - true para contador progressivo, false para regressivo
    */
    timer = timerBegin(0, 80, true);

    /*conecta � interrup��o do timer
     - timer � a inst�ncia do hw_timer
     - endere�o da fun��o a ser chamada pelo timer
     - edge=true gera uma interrup��o
    */
    timerAttachInterrupt(timer, &timerRTC, true);

    /* - o timer instanciado no inicio
       - o valor em us para 1s
       - auto-reload. true para repetir o alarme
    */
    timerAlarmWrite(timer, 1000000, true);

    //ativa o alarme
    timerAlarmEnable(timer);
}

void stopTimer() {
    timerEnd(timer);
    timer = NULL;
}








//oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
void Exibe_Display() {
    uint8_t temporario = 0;
    uint16_t prescaller = 0;
    int8_t counter;

    for (counter = MAXDEVICE - 1; counter > -1; counter--)
    {
        if ((funcao == FUNCAO_SETPOINT) || (funcao == FUNCAO_HISTERESE))
        {
            if (SensoresAtuadores[counter].status == DINAMICO)
            {
                if (tempoDecorrido > 0)
                {
                    seteSegmentos.sendDisplay(SensoresAtuadores[counter].mensagem, DINAMICO);
                }
                else
                {
                    seteSegmentos.sendDisplay(SensoresAtuadores[counter].temp, PISCA);
                }
            }
            else if (SensoresAtuadores[counter].status == NORMAL)
            {
                seteSegmentos.sendDisplay(SensoresAtuadores[counter].valorDeLeitura(COMPOSTO), NORMAL);
            }
            else if (SensoresAtuadores[counter].status == PISCA)
            {
                seteSegmentos.sendDisplay(SensoresAtuadores[counter].valorDeLeitura(COMPOSTO), PISCA);
            }
        }
        else if (funcao == FUNCAO_CODIGO)
        {
            if (SensoresAtuadores[counter].status == DINAMICO)
            {
                if (tempoDecorrido > 0)
                {
                    seteSegmentos.sendDisplay(SensoresAtuadores[counter].mensagem, DINAMICO);
                }
                else
                {
                    seteSegmentos.sendDisplay(SensoresAtuadores[counter].temp, PISCA);
                }
            }
            else if (SensoresAtuadores[counter].status == NORMAL)
            {
                seteSegmentos.sendDisplay(SensoresAtuadores[counter].valorDeLeitura(COMPOSTO), NORMAL);
            }
            else if (SensoresAtuadores[counter].status == PISCA)
            {
                seteSegmentos.sendDisplay(SensoresAtuadores[counter].valorDeLeitura(COMPOSTO), PISCA);
            }
        }
        else if (funcao == FUNCAO_SHOWMESSAGE)
        {
            if (SensoresAtuadores[counter].status == DINAMICO)
            {
            if (tempoDecorrido > 0) 
               {
               if ((tempoDecorrido % 1000) < 500)
                   seteSegmentos.sendDisplay(SensoresAtuadores[counter].mensagem1, NORMAL);
               else
                   seteSegmentos.sendDisplay(SensoresAtuadores[counter].mensagem, NORMAL);
               } 
            else
                seteSegmentos.sendDisplay(SensoresAtuadores[counter].valorDeLeitura(COMPOSTO), NORMAL);
            }
            else
            {
            seteSegmentos.sendDisplay(SensoresAtuadores[counter].valorDeLeitura(COMPOSTO), NORMAL);  
            }
        }
        else if (funcao == FUNCAO_NONE)
        {
            if (SensoresAtuadores[counter].status == DINAMICO)
            {
                if (autorestore % 2 == 0)
                {
                    seteSegmentos.sendDisplay(SensoresAtuadores[counter].temp, NORMAL);
                }
                else
                {
                    seteSegmentos.sendDisplay(SensoresAtuadores[counter].mensagem, NORMAL);
                }
            }
            else
            {   
                if(counter==0)
                   seteSegmentos.sendDisplay(SensoresAtuadores[counter].mensagem, NORMAL);
                else   
                   seteSegmentos.sendDisplay(SensoresAtuadores[counter].valorDeLeitura(COMPOSTO), NORMAL);
            }
        }
    }
    if (++prescaller == 1000)
    {
        prescaller = 0;
        temporario++;
    }
    seteSegmentos.sendSingle(extra74HC595.chip.value);
    seteSegmentos.show();
    
}


















//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
void executaTarefa(uint32_t codigo, float parametro) {
    //codigo /= 1000;
    parametro /= 1000.0;
    if(displayNumero==0)
      {
      comandos.blkPrintln("Entrou em display = 0");  
      funcao = FUNCAO_NONE;
      tempoDecorrido=0;
      autorestore=0;
      SensoresAtuadores[displayNumero].status = NORMAL;
      return;  
      }
    else if(codigo==CODE_OFFSET_SETAR) //1
    {
        comandos.blkPrintln("Voce setou o offset em "); comandos.blkPrint(parametro);
        SensoresAtuadores[displayNumero].offset = parametro;
        hardDisk.EEPROMWriteFloat(20 * displayNumero + 8, parametro);        
    }
    else if(codigo==CODE_OFFSET_VIEW) //2
    {
        funcao = FUNCAO_NONE;
        SaidaAutomatica(SETA);
        SensoresAtuadores[displayNumero].status = DINAMICO;
        strcpy(SensoresAtuadores[displayNumero].mensagem,"OFF.S");
        SensoresAtuadores[displayNumero].temp = hardDisk.EEPROMReadFloat(20 * displayNumero + 8);
        comandos.blkPrintln("Voce esta visualizando o Offset");        
    }
    else if(codigo==CODE_SETPOINT_SETAR) //3
    {
        comandos.blkPrint("Voce setou o offset em "); comandos.blkPrintln(parametro);
        strcpy(SensoresAtuadores[displayNumero].mensagem,"SET.P");
        SensoresAtuadores[displayNumero].setpoint = parametro;
        hardDisk.EEPROMWriteFloat(20 * displayNumero + 0, parametro);  //20 * i + 0
    }
    else if(codigo==CODE_SETPOINT_VIEW) //4
    {
        funcao = FUNCAO_NONE;
        SaidaAutomatica(SETA);
        SensoresAtuadores[displayNumero].status = DINAMICO;
        strcpy(SensoresAtuadores[displayNumero].mensagem,"SET.P");
        SensoresAtuadores[displayNumero].temp = hardDisk.EEPROMReadFloat(20 * displayNumero + 0);
        comandos.blkPrintln("Voce esta visualizando o SetPoint"); 
    }




    else if(codigo==CODE_TEMPO_ON_SETAR) //5
    {
        comandos.blkPrintln("Voce setou tempo do rele ligado em "); comandos.blkPrint(parametro);
        strcpy(SensoresAtuadores[displayNumero].mensagem,"SET ");
        SensoresAtuadores[displayNumero].tempo_ON= parametro;
        tempoCNT[displayNumero]=parametro;
        hardDisk.EEPROMWriteFloat(20 * displayNumero + 0x0C, parametro);  //20 * i + 4
    }
    else if(codigo==CODE_TEMPO_ON_VIEW) //6
    {
        funcao = FUNCAO_NONE;
        SaidaAutomatica(SETA);
        SensoresAtuadores[displayNumero].status = DINAMICO;
        strcpy(SensoresAtuadores[displayNumero].mensagem,"----");
        SensoresAtuadores[displayNumero].temp = hardDisk.EEPROMReadFloat(20 * displayNumero + 0x0C);
        comandos.blkPrintln("Voce esta visualizando o Tempo Ligado do Rele.");        
    }



    else if(codigo==CODE_TEMPO_OFF_SETAR) //7
    {
        comandos.blkPrintln("Voce setou tempo do rele desligado em "); comandos.blkPrint(parametro);
        strcpy(SensoresAtuadores[displayNumero].mensagem,"SET ");
        SensoresAtuadores[displayNumero].tempo_OFF= parametro;
        tempoCNT[displayNumero]=parametro;
        hardDisk.EEPROMWriteFloat(20 * displayNumero + 0x10, parametro);  //20 * i + 4
    }
    else if(codigo==CODE_TEMPO_OFF_VIEW) //8
    {
        funcao = FUNCAO_NONE;
        SaidaAutomatica(SETA);
        SensoresAtuadores[displayNumero].status = DINAMICO;
        strcpy(SensoresAtuadores[displayNumero].mensagem,"----");
        SensoresAtuadores[displayNumero].temp = hardDisk.EEPROMReadFloat(20 * displayNumero + 0x10);
        comandos.blkPrintln("Voce esta visualizando o Tempo Desligado do Rele.");        
    }








    
    else if(codigo==CODE_HISTERESE_SETAR) //9
    {
        comandos.blkPrintln("Voce setou a histerese em ");comandos.blkPrint(parametro);
        strcpy(SensoresAtuadores[displayNumero].mensagem,"HIST");
        SensoresAtuadores[displayNumero].histerese = parametro;
        hardDisk.EEPROMWriteFloat(20 * displayNumero + 4, parametro);  //20 * i + 4
    }
    else if(codigo==CODE_HISTERESE_VIEW) //10
    {
        funcao = FUNCAO_NONE;
        SaidaAutomatica(SETA);
        SensoresAtuadores[displayNumero].status = DINAMICO;
        strcpy(SensoresAtuadores[displayNumero].mensagem,"HIST");
        SensoresAtuadores[displayNumero].temp = hardDisk.EEPROMReadFloat(20 * displayNumero + 4);
        comandos.blkPrintln("Voce esta visualizando o Histerese.");        
    }
    else if(codigo==CODE_CONDENSADOR_ON) //20
    {
        comandos.blkPrintln("Voce ligou o condensador.");
        relay_condensador = !true;          
        tempoDecorrido=5000;
        SensoresAtuadores[displayNumero].status = DINAMICO;
        strcpy(SensoresAtuadores[displayNumero].mensagem1,"ON  ");
        funcao = FUNCAO_SHOWMESSAGE;
    }
    else if(codigo==CODE_CONDENSADOR_OFF) //21
    {
        comandos.blkPrintln("Voce desligou o condensador.");      
        relay_condensador = !false;
        tempoDecorrido=5000;
        SensoresAtuadores[displayNumero].status = DINAMICO;
        strcpy(SensoresAtuadores[displayNumero].mensagem1,"OFF ");
        funcao = FUNCAO_SHOWMESSAGE;
    }
    else if(codigo==CODE_VACCUM_ON) //22
    {
        comandos.blkPrintln("Voce ligou a bomba de vácuo.");      
        relay_vaccum = !true;
        tempoDecorrido=5000;
        SensoresAtuadores[displayNumero].status = DINAMICO;
        strcpy(SensoresAtuadores[displayNumero].mensagem1,"ON  ");
        funcao = FUNCAO_SHOWMESSAGE;        
    }
    else if(codigo==CODE_VACCUM_OFF) //23
    {
        comandos.blkPrintln("Voce desligou a bomba de vácuo.");       
        relay_vaccum = !false;
        tempoDecorrido=5000;
        SensoresAtuadores[displayNumero].status = DINAMICO;
        strcpy(SensoresAtuadores[displayNumero].mensagem1,"OFF ");
        funcao = FUNCAO_SHOWMESSAGE;        
    }
    else if(codigo==CODE_AQUECIMENTO_ON) //24
    {
        comandos.blkPrintln("Voce ligou o aquecimento.");       
        relay_aquecimento = !true;
        relay_comum = !true;
        tempoDecorrido=5000;
        SensoresAtuadores[displayNumero].status = DINAMICO;
        strcpy(SensoresAtuadores[displayNumero].mensagem1,"ON  ");
        funcao = FUNCAO_SHOWMESSAGE;        
    }
    else if(codigo==CODE_AQUECIMENTO_OFF) //25
    {
        comandos.blkPrintln("Voce desligou o aquecimento.");       
        relay_aquecimento = !false;
        relay_comum = !false;
        tempoDecorrido=5000;
        SensoresAtuadores[displayNumero].status = DINAMICO;
        strcpy(SensoresAtuadores[displayNumero].mensagem1,"OFF ");
        funcao = FUNCAO_SHOWMESSAGE;        
    }
    else if(codigo==CODE_DATALOG_ON)  //26
    {
        //Colocar função aqui.
        comandos.blkPrintln("Voce ligou o datalog.");       
        tempoDecorrido=5000;
        SensoresAtuadores[displayNumero].status = DINAMICO;
        strcpy(SensoresAtuadores[displayNumero].mensagem1,"ON  ");
        funcao = FUNCAO_SHOWMESSAGE; 
    }
    else if(codigo==CODE_DATALOG_OFF) //27
    {
        //Colocar Funcao aqui.
        comandos.blkPrintln("Voce desligou o datalog.");       
        tempoDecorrido=5000;
        SensoresAtuadores[displayNumero].status = DINAMICO;
        strcpy(SensoresAtuadores[displayNumero].mensagem1,"OFF ");
        funcao = FUNCAO_SHOWMESSAGE; 
    }
    else if(codigo==CODE_LIOFILIZAR_AUTO_ON)  //100
    {
        comandos.blkPrintln("Voce ligou o modo automático do Liofilizador");       
        tempoDecorrido=3000;        
        flag_processo_auto = true;
        strcpy(SensoresAtuadores[displayNumero].mensagem1," ON ");
        funcao = FUNCAO_SHOWMESSAGE;
    }
    else if(codigo==CODE_LIOFILIZAR_AUTO_OFF) //101
    {
        comandos.blkPrintln("Voce desligou o modo automático do Liofilizador");        
        tempoDecorrido=3000;        
        flag_processo_auto = false;
        strcpy(SensoresAtuadores[displayNumero].mensagem1,"OFF ");           
        funcao = FUNCAO_SHOWMESSAGE;
    }
    else if(codigo==CODE_DEFAULT_FACTORE)  //145
    {
        loadDefaultFactory();
    }
    else if(codigo==CODE_NULL)
    {
        funcao = FUNCAO_SHOWMESSAGE;
        SaidaAutomatica(SETA);
        strcpy(SensoresAtuadores[displayNumero].mensagem1,"NULL");
        comandos.blkPrintln("Voce entrou com parâmetro nulo!");       
        buzzer=1000;
        comandos.blkPrintln("Codigo invalido!");
    }


}



//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
void FormaNumero(float* number) {
    float temp;
    if (flag_zerar)
    {
        flag_zerar = false;
        *number = 0;
    }

    (*number) *= 10.0;
    temp = (tecla_pressionada - 48) * 0.001;

    (*number) += temp;
}





//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//SetPoint do Sistema
void Pressionou_a_Tecla_A() {
     if(flag_processo_auto)comandos.blkPrint("Cancelando modo Automático.");
     flag_processo_auto=false; 
     //flag_vacuo=false;
     //flag_aquecimento=false;
     
     if(flag_condensador)
       {
       flag_condensador=false;
       comandos.blkPrintln("Desligando condensador.");
       }
     else
       {
       flag_condensador=true;
       comandos.blkPrintln("Ligando condensador.");          
       }
     
     
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//Histerese do sistema
void Pressionou_a_Tecla_B() {
     if(flag_processo_auto)comandos.blkPrint("Cancelando modo Automático.");
     flag_processo_auto=false;
     //flag_aquecimento=false;
     //flag_condensador=false;
     
     if(flag_vacuo)
       {
       flag_vacuo=false;
       comandos.blkPrintln("Desligando bomba de vácuo.");
       }
     else
       {
       flag_vacuo =true;
       comandos.blkPrintln("Ligando bomba de vácuo.");          
       }     
}

//====================================================================================
//Code do Sistema
void Pressionou_a_Tecla_C() {
    comandos.blkPrintln("Modo de digitação de Código");
    comandos.blkPrintln("Se não souber o código, digite");
    comandos.blkPrintln("no prompt de comandos 'CODES'.");
    funcao = FUNCAO_CODIGO;
    decimal = 0.01;
    for (int8_t i = 0; i < MAXDEVICE; i++)
    {
        SensoresAtuadores[i].status = NORMAL;
        strcpy(SensoresAtuadores[i].mensagem, "CODE");
        SensoresAtuadores[i].temp = 0;
    }
    if (funcaoMemo == FUNCAO_CODIGO)
    {
        if (++displayNumero >= MAXDEVICE) displayNumero = 0;
        flag_zerar = true;
    }
    funcaoMemo = FUNCAO_CODIGO;

    enterDoKeypad = 0;

    if(displayNumero!=0)SensoresAtuadores[displayNumero].status = DINAMICO;
    
    tempoDecorrido = 3000;
}


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//Alterna entre positivo e negativo
void Pressionou_a_Tecla_D() {
    SensoresAtuadores[displayNumero].temp = -SensoresAtuadores[displayNumero].temp;
}



//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//Teclas Numericas
void Pressionou_Tecla_Numerica() {
    if ((funcao == FUNCAO_SETPOINT) || (funcao == FUNCAO_HISTERESE))
        FormaNumero(&SensoresAtuadores[displayNumero].temp);
    else if ((funcao == FUNCAO_CODIGO))
    {
        if (flag_zerar)
        {
            flag_zerar = false;
            SensoresAtuadores[displayNumero].temp = 0;
        }
        FormaNumero(&SensoresAtuadores[displayNumero].temp);
    }


}

//===================================================================================
//Esc do sistema
void Pressionou_Asterisco() {
    flag_zerar = true;
    tempoDecorrido = 0;
    autorestore = -1;
    funcao = FUNCAO_NONE;
    enterDoKeypad = 0;
    comandos.blkPrintln("Função cancelada pelo usuário");
    //PadronizaSensoresAtuadores("", NORMAL, false);
    //PadronizaValordoTempFloat(0);
    for (int8_t i = 0; i < MAXDEVICE; i++)
    {
        SensoresAtuadores[i].status = NORMAL;
        strcpy(SensoresAtuadores[i].mensagem,"");
        SensoresAtuadores[i].temp = SensoresAtuadores[i].histerese;
    }
    if (++displayNumero >= MAXDEVICE) displayNumero = 0;
    flag_zerar = false;
    
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//Enter do Sistema

void Pressionou_Tecla_Sharp()
{
    if (funcao == FUNCAO_SETPOINT)
    {
        SensoresAtuadores[displayNumero].setpoint = SensoresAtuadores[displayNumero].temp;
        SensoresAtuadores[displayNumero].status = NORMAL;
        hardDisk.EEPROMWriteFloat((20 * displayNumero + 0), SensoresAtuadores[displayNumero].setpoint);
        funcao = FUNCAO_NONE;
    }
    else if (funcao == FUNCAO_HISTERESE)
    {
        SensoresAtuadores[displayNumero].histerese = SensoresAtuadores[displayNumero].temp;
        SensoresAtuadores[displayNumero].status = NORMAL;
        hardDisk.EEPROMWriteFloat((20 * displayNumero + 4), SensoresAtuadores[displayNumero].histerese);
        funcao = FUNCAO_NONE;
    }
    else if (funcao == FUNCAO_CODIGO)
    {
        static float codigo;
        static float parametro;
        char texto[10];
        if (flag_zerar) enterDoKeypad = 0;
        if (enterDoKeypad == 0)
        {
            tempoDecorrido = 2000;
            Serial.print("Você digitou o codigo ");
            Serial.println(SensoresAtuadores[displayNumero].temp, 3);
            codigo = round(SensoresAtuadores[displayNumero].temp * 1000);

            switch((uint16_t)codigo)
            {
              case CODE_CONDENSADOR_ON:
              case CODE_CONDENSADOR_OFF:  
                   strcpy(texto,"Cond.");
                   break;
              case CODE_VACCUM_ON:                
              case CODE_VACCUM_OFF:
                   strcpy(texto,"PRES.");
                   break;
              case CODE_AQUECIMENTO_ON:
              case CODE_AQUECIMENTO_OFF:                   
                   strcpy(texto,"CALO.");
                   break;
              case CODE_DATALOG_ON:
              case CODE_DATALOG_OFF:                   
                   strcpy(texto,"D.LOG");
                   break;
              case CODE_DEFAULT_FACTORE: 
                   strcpy(texto,"FACT.");
                   break;                   
              case CODE_LIOFILIZAR_AUTO_ON:
              case CODE_LIOFILIZAR_AUTO_OFF:
                   strcpy(texto,"AUTO");
                   break;   
              case CODE_OFFSET_SETAR: 
              case CODE_OFFSET_VIEW: 
                   strcpy(texto,"OFF.S");
                   break; 
              case CODE_SETPOINT_SETAR: 
              case CODE_SETPOINT_VIEW: 
                   strcpy(texto,"SET.P");
                   break;
              case CODE_HISTERESE_SETAR:
              case CODE_HISTERESE_VIEW:              
                   strcpy(texto,"HIST");
                   break; 
              case CODE_TEMPO_ON_SETAR:
              case CODE_TEMPO_ON_VIEW:
                   strcpy(texto,"T-ON");
                   break;      
              case CODE_TEMPO_OFF_SETAR:
              case CODE_TEMPO_OFF_VIEW:
                   strcpy(texto,"TOFF");
                   break;                                       
              default:
                   codigo = CODE_NULL;
                   strcpy(texto,"CODE");
                   break;              
                                    
            }
         
            strcpy(SensoresAtuadores[displayNumero].mensagem,texto);
            SensoresAtuadores[displayNumero].status = DINAMICO;
            SensoresAtuadores[displayNumero].temp = 0;
            enterDoKeypad++;

            //Lista de comandos sem parâmetros
            switch((uint16_t)codigo)
            {
            case CODE_NULL:  
            case CODE_CONDENSADOR_ON:
            case CODE_CONDENSADOR_OFF:
            case CODE_VACCUM_ON:
            case CODE_VACCUM_OFF:
            case CODE_AQUECIMENTO_ON:
            case CODE_AQUECIMENTO_OFF:
            case CODE_DATALOG_ON:
            case CODE_DATALOG_OFF:    
            case CODE_DEFAULT_FACTORE:
            case CODE_OFFSET_VIEW: 
            case CODE_SETPOINT_VIEW:
            case CODE_HISTERESE_VIEW:
            case CODE_LIOFILIZAR_AUTO_ON:
            case CODE_LIOFILIZAR_AUTO_OFF:
            case CODE_TEMPO_ON_VIEW:
            case CODE_TEMPO_OFF_VIEW:
                 executaTarefa(codigo, 0);
                 flag_zerar=true; //Zera variável de quantidade de entradas;
                 break;         
            }
             
        }
        else
        {
            tempoDecorrido = 2000;
            SensoresAtuadores[displayNumero].status = NORMAL;
            Serial.print("Com o parametro ");
            Serial.println(SensoresAtuadores[displayNumero].temp, 3);
            parametro = round(SensoresAtuadores[displayNumero].temp * 1000);
            flag_zerar=true; //Zera variável de quantidade de entradas;
            executaTarefa(codigo, parametro);
        }





    }
}





/*
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
void SetarInformacaoeIndice(uint8_t index, String Texto){
SensoresAtuadores[displayNumero].mensagem = Texto;
//SensoresAtuadores[displayNumero].index_message = index;
SensoresAtuadores[displayNumero].temp = 0;
}
*/

//======================================================================
//                           T  H  R  E  A  D  S
//======================================================================
//=====================================================================
void doKeypad()
{
    tecla_pressionada = meuteclado.getKey();
    if (tecla_pressionada)
    {
        SaidaAutomatica(SETA); //Define desist�ncia de edi��o
        buzzer = TIME_BUZZER; //Define aviso sonoro.
        switch (tecla_pressionada)
        {
        case 'A':Pressionou_a_Tecla_A(); break;
        case 'B':Pressionou_a_Tecla_B(); break;
        case 'C':Pressionou_a_Tecla_C(); break;
        case '*':Pressionou_Asterisco(); break;
        case 'D':Pressionou_a_Tecla_D(); break;
        case '#':Pressionou_Tecla_Sharp(); break;
        default:Pressionou_Tecla_Numerica(); break;
        }
        //Serial.println(tecla_pressionada);
        tecla_pressionada = 0;
    }
}



AD_Converter ADC_Indka;
void doLerSensor() {
    static uint8_t escalona = 0;
    uint8_t contador;
    //--------------------------------
    float ADCvalue;
    float resistencia;
    float temperatura;
    float calc;
    
    

    //--------------------------------
    if (escalona == 0)
    {
        String tempo;
        char   tempoChar[10];       
        if(hora<10)
          {
          strcpy(tempoChar,"0");
          tempo=String(hora,HEX);
          tempoChar[1]=tempo[0];          
          }
        else
          {
          strcpy(tempoChar,"");  
          tempo=String(hora,DEC);
          tempoChar[0]=tempo[0];            
          tempoChar[1]=tempo[1];           
          }           
        tempo=String(minuto/100.0,DEC);
        tempoChar[2]=tempo[1];
        tempoChar[3]=tempo[2];
        tempoChar[4]=tempo[3];
        tempoChar[5]=0;
        
        strcpy(SensoresAtuadores[escalona].mensagem,tempoChar);
    }
    else if (escalona == 1) // Sensor de NTC
    {

        ADCvalue = ADC_Indka.read_pin(39, AD_MEDIO);       //Faz uma m�dia das entradas
        ADCvalue = ADC_Indka.suaviza(ADCvalue, escalona);  //Suaviza os dados por tend�ncia

        SensoresAtuadores[escalona].value = ((200.0 * ADCvalue) / 4095.0) - 100.0;

      
       
    }

    else if (escalona == 2) //Sensor de V�cuo
    {
        float vaccum;
        ADCvalue = ADC_Indka.read_pin(35, AD_MEDIO);       //Faz uma m�dia das entradas
        ADCvalue = ADC_Indka.suaviza(ADCvalue, escalona);  //Suaviza os dados por tend�ncia

        if (ADCvalue < (615.0 * 4.0))
        {
            vaccum = (4.0 * 1023.0) - (ADCvalue - (454.0 * 4.0));
            vaccum *= 1.9550342131 / 4.0;
            vaccum -= ((ADCvalue - (454.0 * 4.0)) * (5.2 / 4.0));
        }
        else
        {
            vaccum = (4.0 * 1023.0) - (ADCvalue - (616.0 * 4.0));
            vaccum*= 0.828934506 / 4.0;
            vaccum -= ((ADCvalue - (616.0 * 4.0)) * (2.34 / 4.0));
        }

        if (vaccum > 2000.0) vaccum = 2000.0;

        SensoresAtuadores[escalona].value = vaccum;
    }


    else if (escalona == 3) //Sensor PT100 CONDENSAADOR
    {        
        #define B 3975.0 
        ADCvalue = ADC_Indka.read_pin(34, AD_MEDIO);       //Faz uma m�dia das entradas
        ADCvalue = ADC_Indka.suaviza(ADCvalue, escalona);  //Suaviza os dados por tend�ncia


        resistencia = (4095.0 - ADCvalue);
        resistencia *= (10000.0 - ADCvalue);
        temperatura = ADCvalue * 10000.0 / resistencia;
        calc = temperatura;
        temperatura = log(calc);
        temperatura /= B;
        temperatura += 1.0 / (25.0 + 273.15);
        temperatura = 1.0 / temperatura;
        temperatura -= 273.15;
      
        SensoresAtuadores[escalona].value = temperatura;//(3.3 / 4095) * valor;//(((200.0 * valor) / 1024.0) - 110.0) / 10;
    }

    escalona++;
    if (escalona >= MAXDEVICE) escalona = 0;



}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool flag_connected = false;
void doBlynkRun() {
     //Blynk.run();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void doBlynkExibe() { 
     #ifdef COM_BLYNK_WIFI
     comandos.exibeDados(); 
     #endif
}


//thBlynk.onRun(doBlynk);
//thBlynk.setInterval(500);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void doDisplay(void) {
     Exibe_Display();
}



//=====================================================================
//++++++++++++++++++++++
//Relogio de tempo real
void doRTC() {
    milisegundo++;

    for(uint8_t i=0;i<MAXDEVICE;i++) if(tempoCNT[i]>0) tempoCNT[i]--;

    if (tempoDecorrido > 0) tempoDecorrido--;
    if (buzzer > 0)
    {
        buzzer--;
        extra74HC595.chip.value |= 0b00100000;//  chipSetValue(0, LIGADO); //PIN_BUZZER
        //digitalWrite(PIN_BUZZER, HIGH);
    }
    else
    {
        //buzzer--;
        extra74HC595.chip.value &= 0b11011111;
        //extra74HC595.chipSetValue(0, DESLIGADO);
        //digitalWrite(PIN_BUZZER, LOW);
    }

}


void exibeRevisao()
    {   
    bool flag_abandona=false;
        while(1) {
            seteSegmentos.sendDisplay("    ", NORMAL);
            seteSegmentos.sendDisplay("    ", NORMAL);        
            seteSegmentos.sendDisplay("    ", NORMAL);
            flag_abandona=seteSegmentos.sendDisplayMessage("JJ CIENTIFICA", NORMAL);            
            seteSegmentos.sendSingle(extra74HC595.chip.value);
            seteSegmentos.show();
            if(flag_abandona)break;
        }    
        while(1) {
            seteSegmentos.sendDisplay("    ", NORMAL);
            seteSegmentos.sendDisplay("    ", NORMAL);        
            seteSegmentos.sendDisplay("    ", NORMAL);
            flag_abandona=seteSegmentos.sendDisplayMessage(versao, NORMAL);            
            seteSegmentos.sendSingle(extra74HC595.chip.value);
            seteSegmentos.show();
            if(flag_abandona)break;
        }
    }



void debugFlags(){
     Serial.println(extra74HC595.chip.value,BIN);
     Serial.print("flag_comum=");  
     //Serial.println(flag_comum);
     Serial.print("flag_condensador=");
     //Serial.println(flag_condensador);
     Serial.print("flag_aquecimento=");
     //Serial.println(flag_aquecimento);
     Serial.print("flag_vacuo=");
     //Serial.println(flag_vacuo);
}
    
void releLigado(){
     
}




//--------------------------------------------------------------------------------------
void loadDefaultFactory(){

    //------------------COMUM--------------------------------
    hardDisk.EEPROMWriteFloat(ADD_HORARIO_SET,     0.0);  //SetPoint
    hardDisk.EEPROMWriteFloat(ADD_HORARIO_HIS,     1.0);  //Histerese Volts
    hardDisk.EEPROMWriteFloat(ADD_HORARIO_OFF,     0.0);  //OFF Set Volts
    hardDisk.EEPROMWriteFloat(ADD_HORARIO_ATIVO,  1000);  //Tempo ON
    hardDisk.EEPROMWriteFloat(ADD_HORARIO_INATIVO, 0.0);  //Tempo OFF 

    //-----------------CONDENSADOR----------------------------
    hardDisk.EEPROMWriteFloat(ADD_CONDENSADOR_SET,   -15.0);  //SetPoint Condensador
    hardDisk.EEPROMWriteFloat(ADD_CONDENSADOR_HIS,     5.0);  //Histerese Condensador
    hardDisk.EEPROMWriteFloat(ADD_CONDENSADOR_OFF,     0.0);  //Off set condensador
    hardDisk.EEPROMWriteFloat(ADD_CONDENSADOR_ATIVO,  1000);  //Tempo ON
    hardDisk.EEPROMWriteFloat(ADD_CONDENSADOR_INATIVO, 0.0);  //Tempo OFF

    //---------------------VACUOMETRO-------------------------
    hardDisk.EEPROMWriteFloat(ADD_VACUOMETRO_SET, 1300.0);     //SetPoint Vacuometro
    hardDisk.EEPROMWriteFloat(ADD_VACUOMETRO_HIS, 200.0);      //Histerese Vacuometro
    hardDisk.EEPROMWriteFloat(ADD_VACUOMETRO_OFF, 0.0);        //Off Set Vacuometro
    hardDisk.EEPROMWriteFloat(ADD_VACUOMETRO_ATIVO,  1000.0);     //Tempo ON
    hardDisk.EEPROMWriteFloat(ADD_VACUOMETRO_INATIVO,   0.0);   //Tempo OFF    

    //--------------------NTC---------------------------------
    hardDisk.EEPROMWriteFloat(ADD_NTC_SET,     35.0);  //SetPoint NTC
    hardDisk.EEPROMWriteFloat(ADD_NTC_HIS,      1.0);  //Histerese NTC
    hardDisk.EEPROMWriteFloat(ADD_NTC_OFF,      0.0);  //Off set NTC
    hardDisk.EEPROMWriteFloat(ADD_NTC_ATIVO,   10000.0);  //Tempo ON
    hardDisk.EEPROMWriteFloat(ADD_NTC_INATIVO, 20000.0);  //Tempo OFF


    
    loadDefaultUser();   

    persistente.statusgen.value=0b00000000;
    persistente.save();
    
    funcao = FUNCAO_NONE;
    Serial.println("Valores padrao de fabrica carregados!");            
}


void loadDefaultUser(){
     for (uint8_t i = 0; i < MAXDEVICE; i++)
     {
        SensoresAtuadores[i].setpoint  = hardDisk.EEPROMReadFloat(20 * i + 0x00);
        SensoresAtuadores[i].histerese = hardDisk.EEPROMReadFloat(20 * i + 0x04);
        SensoresAtuadores[i].offset    = hardDisk.EEPROMReadFloat(20 * i + 0x08);
        SensoresAtuadores[i].tempo_ON  = hardDisk.EEPROMReadFloat(20 * i + 0x0C);
        SensoresAtuadores[i].tempo_OFF = hardDisk.EEPROMReadFloat(20 * i + 0x10);
        SensoresAtuadores[i].status    = NORMAL;
        SensoresAtuadores[i].estado    = RELAY_DESLIGADO;
        //SensoresAtuadores[i].modo    = RELAY_LIGADO_SOBE;
        //SensoresAtuadores[i].value   = 0.0;
     }  
}
