#include "Arduino.h"
#include "Shell.h"
#include "global.h"
#include "Eeprom_Indka.h"
#include "Thread.h"
#include "ThreadController.h"
#include "Modulo595.h"
#include "Blk_terminal.h"
#include "Sobressalente74HC595.h"
#include "Controladores.h"
#include "Persistente.h"


extern uint8_t  hora;
extern uint8_t  minuto;
extern uint8_t  segundo;
extern uint16_t milisegundo;  

extern Thread thBlynkRun;
extern Thread thAutoRelay;
extern Thread thLerSensor;
extern Thread thBlynkExibe;
extern Thread thDisplay;
extern Thread thKeypad;
extern Thread thShell;
extern Thread thRTC;

//extern hw_timer_t* timer;

extern Controladores SensoresAtuadores[MAXDEVICE];

extern Modulo595 seteSegmentos;

extern Eeprom_Indka hardDisk;

extern int8_t displayNumero;

extern Sobressalente74HC595 extra74HC595;

extern uint16_t buzzer;

extern Persistente persistente;

bool thread_vector[8] = { true, true, true, true, true, true, true, true };

bool flag_hora   = false;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


Shell::Shell(void)
  {

  }


Blk_terminalClass ObjBlynk;
void Shell::init(){
     #ifdef COM_BLYNK_WIFI
     ObjBlynk.init();     
     #endif 
}



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void Shell::empresa()
      {


     blkPrintln("Desenvolvido por:");
     blkPrintln("");
     blkPrintln("        ___             _   _"); 
     blkPrintln("       |_ _|  _ _    __| | | |__  __ _ ");
     blkPrintln("        | |  |   \\  / _` | | / / / _` |");
     blkPrintln("       |___| |_||_| \\__,_| |_\\_\\ \\__,_|");
     blkPrintln("");
     blkPrintln("       INDÚSTRIA DE KITS PARA AUTOMAÇÃO");
     blkPrintln("             Projetos e Engenharia ");
     blkPrintln("                      de ");
     blkPrintln("              Hardwaare e Software");
     blkPrintln("              Ribeirão Preto - SP");
     blkPrintln("                www.indka.com.br");
     blkPrintln("                (16) 99600-9172");
     blkPrintln("");
     //blkPrint(PROMPT);          
     }


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void Shell::mostraTempo(void)
     {
     static uint16_t counter;
     counter++;
     if (counter % 4 == 0)
         {         
         blkPrint("Tempo do sistema : ");
         if (hora < 10) blkPrint("0");
         blkPrint(hora,0);
         blkPrint(":");
         if (minuto < 10) blkPrint("0");
         blkPrint(minuto,0);
         blkPrint(":");
         if (segundo < 10) blkPrint("0");
         blkPrint(segundo,0);         
         blkPrintln("");
         blkPrint("Tempo de processo: ");
         flag_hora = false;
         }
     }





//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
String buffer = "";
char a = 0;
char b = 0;
char c = 0;

char *texto;
uint16_t valor;
extern bool flag_brx;
extern String bufferBlynk;
String  doskey[10];
uint8_t doskeyIndex=0;
uint8_t contador;
void Shell::prompt(void)
{
    static uint8_t index = 0;

    //Relógio em tempo real
    if (a == 0 && flag_hora) mostraTempo();

    #ifdef COM_BLYNK_WIFI
    ObjBlynk.run();
    #endif
    

    if ((Serial.available()) || (flag_brx==true))
    {
        a = Serial.read();
        b = Serial.read();
        c = Serial.read();

        flag_hora = false;

        if (a >= 0x20)
        {
            Serial.write(a);
            buffer += a;
            index++;
        }
        else
        {
            if (a == 8) //backspace
            {
                if (index > 0)
                {
                    // 5 - retorna o nome do terminal
                    index--;
                    Serial.write(127);
                    buffer.remove(buffer.length() - 1, 1);
                }
                else
                {
                    Serial.write(7);//bell
                }
            }
            else if (a == '\r') //enter
            {
                blkPrintln("");
                buffer += '\0';
                index = 0;
            }
            else //--
            {
               
                if ((a == 27) && (b == 91) && (c == 65)) //Subir
                {
                    
                    for (uint8_t i = 1; i < doskey[contador].length(); i++)
                    {
                        Serial.write(8);                       
                    }
                    
                    //Serial.printf("\33[2K\r");
                    blkPrint(PROMPT);
                    Serial.print(doskey[contador]);
                    buffer = doskey[contador];
                    index = doskey[contador].length();
                    contador++;
                }
                else if ((a == 27) && (b == 91) && (c == 66)) //Descer
                {
                    Serial.println("Descer");
                }
                else if ((a == 27) && (b == 91) && (c == 68)) //Esquerda
                {
                    Serial.println("Esquerda");
                }
                else if ((a == 27) && (b == 91) && (c == 67)) //Direita
                {
                    Serial.println("Direita");
                }
                else
                {
                    Serial.println(toascii(a));
                    Serial.println(toascii(b));
                    Serial.println(toascii(c));
                }

                
            }
        } //Caracteres especiais 

    }
    //+ + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +


    //+ + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
    if (flag_brx) {
        flag_brx = false;
        buffer = bufferBlynk;
        a = 13;  //Parametro primeiro do vácuo composto por a,b,c.
    }
    //+ + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +
    buffer.toUpperCase();                       //Descobre sobre quem está tratando para
    for (uint8_t i = 0; i < MAXDEVICE; i++)     //poder lidar com ele genericamente
    {
        if (buffer.startsWith(DEVICE[i]))
        {
            displayNumero = i;
            break;
        }
    }


    if (a == 13) //ENTER
    {
        String parametro;
        //-----------------------------------------------------------------------        
        if (buffer != "")
        {
            String doskeyStr;
            doskeyStr = buffer;
            doskeyStr.toLowerCase();
            for (uint8_t i = 9; i > 0; i--) doskey[i] = doskey[i - 1];
            doskey[0] = doskeyStr;
            doskeyIndex++;
            contador = 0;
        }
        //-----------------------------------------------------------------------
        parametro= extraiProximoParametro(&buffer, ' ');
        a = 0;     //parametro primeiro do teclado composto por a,b,c. 

        if (parametro == "CLS")
        {
            Serial.write(12);
            ObjBlynk.terminalClear();
        }
        else if (parametro == "BUZZER")
        {
            parametro = extraiProximoParametro(&buffer, ' ');
            if (parametro == "")
            {
                blkPrintln("Erro de Sintaxe.");
                blkPrintln("Use 'buzzer <tempo>'");
                blkPrintln("*tempo em milisegundos");
            }
            else
            {
                buzzer = parametro.toInt();
            }
            
        }
        else if (parametro == "RELAY")
        {
            parametro = extraiProximoParametro(&buffer, ' ');
            
            if (parametro == "")
            {
                blkPrintln("Erro de sintaxe.");
                blkPrintln("Use:");
                blkPrintln("relay <comum/");
                blkPrintln("       condensador/");
                blkPrintln("       ntc/");
                blkPrintln("       vaccum/");
                blkPrintln("       <on / off>");
            }            
            else 
            {
                uint8_t releNum;
                uint8_t elevado;
                //releNum=pow(2,elevado);  
                
                if(parametro=="COMUM")
                  {
                  releNum=COMUM;    
                  }                
                else if(parametro=="CONDENSADOR")
                  {
                  releNum=CONDENSADOR;     
                  }
                else if(parametro=="NTC")
                  {
                  releNum=SENSOR_NTC;     
                  }
                else if(parametro=="VACCUM")
                  {
                  releNum=VACUOMETRO;     
                  }
 

                elevado=pow(2,releNum); 
                Serial.println(elevado,BIN);  
                         
                          
                parametro = extraiProximoParametro(&buffer, ' ');
                //Serial.print("Antes statusgen.value  =");Serial.println(persistente.statusgen.value,BIN);
                if (parametro == "ON")
                {                    
                    persistente.statusgen.value |= elevado;
                }
                else if (parametro == "OFF")
                {
                    persistente.statusgen.value &= ~elevado;
                }
                Serial.print("depois statusgen.value =");Serial.println(persistente.statusgen.value,BIN);
                Serial.print("Chip 74HC595    =");Serial.println(extra74HC595.chip.value,BIN);
               
            }            
        }
        else if (parametro=="STATUS")
        {
          uint8_t extra;
          blkPrintln("Status das flags do sistema.");
          for(uint8_t i=0;i<8;i++)
             { 
             blkPrint(DEVICEShow[i]);
             extra=pow(2,i);             
             if((persistente.statusgen.value&extra)!=0)
               blkPrintln(" Ativo");
             else
               blkPrintln(" Inativo");               
             }
        }
        else if (parametro=="DEVICES")
        {
            parametro = extraiProximoParametro(&buffer, ' ');

            if (parametro == "")
            {
                blkPrintln("----------------------------------------");
                blkPrintln("Devices /ALL - Exibe todos os sensores.");
                blkPrintln("Devices Disponíveis:");
                for (uint8_t i = 0; i < MAXDEVICE; i++)
                {
                    blkPrint(i, 0); blkPrint("........"); blkPrintln(DEVICE[i]);
                }
                blkPrintln("/S    - Exibe ou altera o SetPoint.");
                blkPrintln("/H    - Exibe ou altera a Histerese.");
                blkPrintln("/O    - Exibe ou altera o OffSet.");
                blkPrintln("/T1   - Exibe ou altera o Tempo ON.");
                blkPrintln("/T0   - Exibe ou altera o Tempo OFF.");
                blkPrintln("/INFO - Exibe todos os dados do sensor.");
            }
            else if (parametro == "/ALL")
            {
                blkPrintln("--------------------------");
                for (uint8_t i = 0; i < MAXDEVICE; i++)
                {
                    blkPrint(DEVICE[i]);
                    blkPrint(" = ");
                    blkPrint(Relay_Valor_Composto(i),1);
                    blkPrintln(SUFIXO[i]);
                }
            }
        }
        else if (parametro=="THREAD")
        {
            uint8_t threadNum;
            Thread* thPoint;
            String  acao;
            parametro = extraiProximoParametro(&buffer, ' ');
            if(parametro=="") HelpHandlerThread();
            threadNum = parametro.toInt();           

            switch (threadNum)
            {
            case 0: thPoint = &thAutoRelay; break;
            case 1: thPoint = &thLerSensor; break;
            case 2: thPoint = &thBlynkExibe; break;
            case 3: thPoint = &thDisplay; break;
            case 4: thPoint = &thKeypad; break;
            case 5: thPoint = &thShell; break;
            case 6: thPoint = &thRTC; break;
            case 7: thPoint = &thBlynkRun; break;
            default:
                thPoint = NULL;
                HelpHandlerThread();
                break;
            }

            acao = extraiProximoParametro(&buffer, ' ');
            if (acao == "ON")
            {
                blkPrintln("Ligado...");
                thPoint->enabled = true;
                thread_vector[threadNum] = true;
            }
            else if (acao == "OFF")
            {
                blkPrintln("Desligado...");
                thPoint->enabled = false;
                thread_vector[threadNum] = false;
                if (threadNum == 3) seteSegmentos.clearDisplay();
            }
        }
        else if (parametro=="TIME")
        {
            String  hh;
            String  mm;
            String  ss;
            hh = extraiProximoParametro(&buffer, ':');
            mm = extraiProximoParametro(&buffer, ':');
            ss = buffer;

            if ((parametro == "TIME")&&(ss==""))
            {
                flag_hora = true;
            }
            else if (ss!="") //Hora completa a ser setada
            {
                hora = hh.toInt();
                minuto = mm.toInt();
                segundo = ss.toInt();
                milisegundo = 0;
                blkPrintln("Relógio setado com sucesso!");
            }
            else
            {
                blkPrintln("Erro de sintaxe. Utilize 'time hh:mm:ss'.");
            }

        }
        else if ((parametro == DEVICE[displayNumero]) && (buffer != ""))
        {
            parametro = extraiProximoParametro(&buffer, ' ');
            if (parametro == "/INFO")
            {
                blkPrintln("-------------------------------------");
                blkPrint("Atual     = "); blkPrint(Relay_Valor_Simples(displayNumero), 2); blkPrintln(SUFIXO[displayNumero]);
                blkPrint("Composto  = "); blkPrint(Relay_Valor_Composto(displayNumero), 2); blkPrintln(SUFIXO[displayNumero]);
                blkPrintln("- - - - - - - - - - - - - - - - - - - ");
                blkPrint("SetPoint  = "); blkPrint(Relay_SetPoint(displayNumero), 2); blkPrintln(SUFIXO[displayNumero]);
                blkPrint("Histerese = "); blkPrint(Relay_Histerese(displayNumero), 2); blkPrintln(SUFIXO[displayNumero]);
                blkPrint("OffSet    = "); blkPrint(Relay_OffSet(displayNumero), 2); blkPrintln(SUFIXO[displayNumero]);
                blkPrint("Superior  = "); blkPrint(Relay_Setagem_Alta(displayNumero), 2); blkPrintln(SUFIXO[displayNumero]);
                blkPrint("Inferior  = "); blkPrint(Relay_Setagem_Baixa(displayNumero), 2); blkPrintln(SUFIXO[displayNumero]);
                blkPrint("Tempo ON  = "); blkPrint(Relay_Tempo_ON(displayNumero), 2); blkPrintln(" ms.");
                blkPrint("Tempo OFF = "); blkPrint(Relay_Tempo_OFF(displayNumero), 2); blkPrintln(" ms.");
                blkPrintln("-------------------------------------");

            }
            else if (parametro == "/H")
            {                
                parametro = extraiProximoParametro(&buffer, ' ');
                if (parametro == "")
                {
                    blkPrint(hardDisk.EEPROMReadFloat(20 * displayNumero + 4), 1);
                    blkPrintln(SUFIXO[displayNumero]);
                }
                else
                {
                    SensoresAtuadores[displayNumero].histerese = parametro.toFloat();
                    blkPrint("Ajustando Histerese para "); blkPrint(SensoresAtuadores[displayNumero].histerese,2); blkPrintln(SUFIXO[displayNumero]);
                    hardDisk.EEPROMWriteFloat(20 * displayNumero + 4, parametro.toFloat());
                }
            }
            else if (parametro == "/S")
            {                
                parametro = extraiProximoParametro(&buffer, ' ');
                if (parametro == "")
                {
                    blkPrint(hardDisk.EEPROMReadFloat(20 * displayNumero + 0),1);
                    blkPrintln(SUFIXO[displayNumero]);
                }
                else
                {
                    SensoresAtuadores[displayNumero].setpoint = parametro.toFloat();
                    blkPrint("Ajustando SetPoint para "); blkPrint(SensoresAtuadores[displayNumero].setpoint,2); blkPrintln(SUFIXO[displayNumero]);
                    hardDisk.EEPROMWriteFloat(20 * displayNumero + 0, parametro.toFloat());
                }


            }
            else if (parametro == "/O")
            {                                
                parametro = extraiProximoParametro(&buffer, ' ');
                if (parametro == "")
                {
                    blkPrint(hardDisk.EEPROMReadFloat(20 * displayNumero + 8), 1);
                    blkPrintln(SUFIXO[displayNumero]);
                }
                else
                {
                    SensoresAtuadores[displayNumero].offset = parametro.toFloat();
                    blkPrint("Ajustando OffSet para "); blkPrint(SensoresAtuadores[displayNumero].offset,2); blkPrintln(SUFIXO[displayNumero]);
                    hardDisk.EEPROMWriteFloat(20 * displayNumero + 8, parametro.toFloat());
                }
            }


            else if (parametro == "/T1")
            {                                
                parametro = extraiProximoParametro(&buffer, ' ');
                if (parametro == "")
                {
                    blkPrint(hardDisk.EEPROMReadFloat(20 * displayNumero + 0x0C), 1);
                    blkPrintln(SUFIXO[displayNumero]);
                }
                else
                {
                    SensoresAtuadores[displayNumero].tempo_ON = parametro.toFloat();
                    blkPrint("Ajustando Tempo ON para "); blkPrint(SensoresAtuadores[displayNumero].tempo_ON,2); blkPrintln(SUFIXO[displayNumero]);
                    hardDisk.EEPROMWriteFloat(20 * displayNumero + 0x0C, parametro.toFloat());
                }
            }


            else if (parametro == "/T0")
            {                                
                parametro = extraiProximoParametro(&buffer, ' ');
                if (parametro == "")
                {
                    blkPrint(hardDisk.EEPROMReadFloat(20 * displayNumero + 0x10), 1);
                    blkPrintln(SUFIXO[displayNumero]);
                }
                else
                {
                    SensoresAtuadores[displayNumero].tempo_OFF = parametro.toFloat();
                    blkPrint("Ajustando Tempo OFF para "); blkPrint(SensoresAtuadores[displayNumero].tempo_OFF,2); blkPrintln(SUFIXO[displayNumero]);
                    hardDisk.EEPROMWriteFloat(20 * displayNumero + 0x10, parametro.toFloat());
                }
            }






            
        }
        else if ((parametro == DEVICE[displayNumero]) && (buffer == ""))
        {
            blkPrintln("Erro de sintaxe - Digite 'Devices' para mais informações.");
        }
        else if (parametro=="HELP")
        {
                blkPrintln("--------------------------------------------");
                blkPrintln("Digite os seguintes comandos:");
                blkPrintln(" Cls     - Limpa a tela.");
                blkPrintln(" Help    - Exibe esta ajuda.");
                blkPrintln(" Buzzer  - Aciona buzzer por alguns ms.");
                blkPrintln(" Time    - Exibe ou altera o relógio.");
                blkPrintln(" Empresa - Exibe o nome do Fabricante.");
                blkPrintln(" Model   - Exibe o modelo do hardware.");
                blkPrintln(" Devices - Exibe dispositivos disponíveis.");
                blkPrintln(" Thread  - Habilita / Desabilita Thread.");
                blkPrintln(" Relay   - Ativa / Desativa relê.");
                blkPrintln(" Status  - Exibe o status dos reles");
                blkPrintln("--------------------------------------------");
        }
        else if (parametro == "EMPRESA")
        {
                empresa();
        }
        else if (parametro == "MODEL")
        {
                blkPrintln("Hardware modelo ILV1000 revisao 1.0.0.");
        }
        else
        {
               if (parametro.length() > 1)
               {
                    blkPrintln("Comando inválido.");
                    blkPrintln("Digite 'help' para instruções.");
                    blkPrintln("");
                    empresa();
               }
        }
            a = 0;
            if (parametro != "TIME") blkPrint(PROMPT);
            buffer = "";
    }
    
}

 void Shell::HelpHandlerThread() {
         blkPrintln("--------------------------------------------");
         blkPrintln("Habilita e Desabilita Threads.");
         blkPrintln("Utilize 'Thread nn /<ON/OFF>");
         blkPrintln("Lista das threads (nn).");
         blkPrint("00 - AutoRelay....."); thread_vector[0] ? blkPrintln("ON") : blkPrintln("OFF");
         blkPrint("01 - LerSensor....."); thread_vector[1] ? blkPrintln("ON") : blkPrintln("OFF");
         blkPrint("02 - Blynk........."); thread_vector[2] ? blkPrintln("ON") : blkPrintln("OFF");
         blkPrint("03 - Display......."); thread_vector[3] ? blkPrintln("ON") : blkPrintln("OFF");
         blkPrint("04 - KeyPad........"); thread_vector[4] ? blkPrintln("ON") : blkPrintln("OFF");
         blkPrint("05 - Shell........."); thread_vector[5] ? blkPrintln("ON") : blkPrintln("OFF");
         blkPrint("06 - RTC..........."); thread_vector[6] ? blkPrintln("ON") : blkPrintln("OFF");
         blkPrint("07 - Core Blynk...."); thread_vector[7] ? blkPrintln("ON") : blkPrintln("OFF");
         blkPrintln("--------------------------------------------");
 }


 void Shell::blkPrint(String text) {
     Serial.print(text);
     ObjBlynk.print(text);
 }


 void Shell::blkPrintln(String text){
     Serial.println(text);
     ObjBlynk.println(text);
 }

 void Shell::blkPrint(double num) {
     char chrtexto[10];
     String texto;
     dtostrf(num, 1, 3, chrtexto);//variável, minimo, precisão, string.
     texto = chrtexto;
     blkPrint(texto);
 }

 void Shell::blkPrintln(double num) {
     char chrtexto[10];
     String texto;
     dtostrf(num, 1, 3, chrtexto);//variável, minimo, precisão, string.
     texto = chrtexto;
     blkPrintln(texto);
 }


 void Shell::blkPrint(double num, uint8_t decimal) {
     char chrtexto[10];
     String texto;
     dtostrf(num, 1, decimal, chrtexto);//variável, minimo, precisão, string.
     texto = chrtexto;
     blkPrint(texto);
 }

 void Shell::blkPrintln(double num, uint8_t decimal) {
     char chrtexto[10];
     String texto;
     dtostrf(num, 1, decimal, chrtexto);//variável, minimo, precisão, string.
     texto = chrtexto;
     blkPrintln(texto);
 }

 void Shell::exibeDados(){
     ObjBlynk.output();
 }

 String Shell::extraiProximoParametro(String *buffer, char caracter) {
        String parametro;
        uint8_t posicao;
        posicao = buffer->indexOf(caracter);
        parametro = buffer->substring(0, posicao);
        buffer->remove(0, posicao+1);
        return parametro;
 }
