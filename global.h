#ifndef GLOBAL_H
#define GLOBAL_H

#include <Arduino.h>

#define MAXDEVICE 4 //Numero máximo de dispositivos

#define TEMPO_PISCA 111

#define LED_BLINK    1

#define TIME_BUZZER 25  //Tempo de buzzer do teclado

//Captura no conversor AD
#define AD_MINIMO 0
#define AD_MEDIO  1
#define AD_MAXIMO 2

//STATUS DO DISPLAY
#define NORMAL     0
#define PISCA      1
#define DINAMICO   2
//#define DINAMICO 2 //Vai mudar para pisca

//ENDERECOS DE MEMORIA PARA SALVAR DADOS NA EEPROM
#define ADD_VOLTIMETRO_SET    0X00  //4 bytes        
#define ADD_VOLTIMETRO_HIS    0X04  //4 bytes        
#define ADD_VOLTIMETRO_OFF    0X08  //4 bytes 
#define ADD_CONDENSADOR_SET   0x0C  //4 bytes
#define ADD_CONDENSADOR_HIS   0x10  //4 bytes
#define ADD_CONDENSADOR_OFF   0x14  //4 bytes
#define ADD_VACUOMETRO_SET    0X18  //4 bytes
#define ADD_VACUOMETRO_HIS    0X1C  //4 bytes
#define ADD_VACUOMETRO_OFF    0X20  //4 bytes
#define ADD_STATUSGEN         0x24  //1 byte
#define ADD_HORA_PROCESSO     0x25  //1 byte
#define ADD_MINUTO_PROCESSO   0x26  //1 byte


//FUNCOES DO TECLADO
#define FUNCAO_NONE        0
#define FUNCAO_SETPOINT    1
#define FUNCAO_HISTERESE   2
#define FUNCAO_CODIGO      3
#define FUNCAO_SHOWMESSAGE 4

//DEFINICOES DOS SENSORES
#define VOLTIMETRO  0
#define CONDENSADOR 1
#define SENSOR_NTC  2
#define VACUOMETRO  3
#define COMUM       4

const String DEVICE[4] = { "VOLTIMETRO",
                           "CONDENSADOR",
                           "SENSOR_NTC",
                           "VACUOMETRO"
                            };
const String SUFIXO[4] = { "V.",
                          "°C.",
                          "°C.",
                          "mmHg."
};

//Sistema para auto reestabelecimento
//caso ocorra desistência de edicao.
#define TEMPO_MAX_EDICAO 60 //Segundos
#define SETA     1
#define MONITORA 2

//-------------------------------------------------------------------------------
//Ciclos para fazer a média de leitura do A/D.
#define CICLOS_DE_MEDIA 10  //Quantas vezes mede?
#define MAXSUAVIZA      25  //Tamanho do vetor para fazer a média

//--------Controlador de Histerese------------------------



//---------------CODIGOS PARA DECODIFICACAO DO TECLADO MEMBRANA------------------
#define CODE_LIOFILIZAR_AUTO  1234
#define CODE_DEFAULT_FACTORE   145
#define CODE_OFFSET_SETAR       10
#define CODE_OFFSET_VIEW        11










#endif
