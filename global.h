#ifndef GLOBAL_H
#define GLOBAL_H

#include <Arduino.h>

//#define COM_BLYNK_WIFI

//#define DEBUG;

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


//ENDERECOS DE MEMORIA PARA SALVAR DADOS NA EEPROM
#define ADD_HORARIO_SET         0X00  //4 bytes        
#define ADD_HORARIO_HIS         0X04  //4 bytes        
#define ADD_HORARIO_OFF         0X08  //4 bytes 
#define ADD_HORARIO_ATIVO       0X0C
#define ADD_HORARIO_INATIVO     0x10

#define ADD_CONDENSADOR_SET     0x14  //4 bytes
#define ADD_CONDENSADOR_HIS     0x18  //4 bytes
#define ADD_CONDENSADOR_OFF     0x1C  //4 bytes
#define ADD_CONDENSADOR_ATIVO   0x20
#define ADD_CONDENSADOR_INATIVO 0x24

#define ADD_NTC_SET             0X28  //4 bytes
#define ADD_NTC_HIS             0X2C  //4 bytes
#define ADD_NTC_OFF             0X30  //4 bytes
#define ADD_NTC_ATIVO           0x34 
#define ADD_NTC_INATIVO         0x38

#define ADD_VACUOMETRO_SET      0X3C  //4 bytes
#define ADD_VACUOMETRO_HIS      0X40  //4 bytes
#define ADD_VACUOMETRO_OFF      0X44  //4 bytes
#define ADD_VACUOMETRO_ATIVO    0x48
#define ADD_VACUOMETRO_INATIVO  0x4C

#define ADD_STATUSGEN           0x50  //1 byte
#define ADD_HORA_PROCESSO       0x54  //1 byte
#define ADD_MINUTO_PROCESSO     0x58  //1 byte

//(20 * i + 0x00) SETPOINT
//(20 * i + 0x04) HISTERESE
//(20 * i + 0x08) OFFSET
//(20 * i + 0x0C) ATIVO
//(20 * i + 0x10) INATIVO


//FUNCOES DO TECLADO
#define FUNCAO_NONE        0
#define FUNCAO_SETPOINT    1
#define FUNCAO_HISTERESE   2
#define FUNCAO_CODIGO      3
#define FUNCAO_SHOWMESSAGE 4

//DEFINICOES DOS SENSORES
#define COMUM       0
#define CONDENSADOR 1
#define SENSOR_NTC  2
#define VACUOMETRO  3


const String DEVICE[4] = { "COMUM",
                           "CONDENSADOR",
                           "SENSOR_NTC",
                           "VACUOMETRO"
                            };
const String SUFIXO[4] = { ".",
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
#define CODE_NULL                  0
#define CODE_OFFSET_SETAR          1
#define CODE_OFFSET_VIEW           2
#define CODE_SETPOINT_SETAR        3
#define CODE_SETPOINT_VIEW         4
#define CODE_HISTERESE_SETAR       5
#define CODE_HISTERESE_VIEW        6
#define CODE_TEMPO_ON_SETAR        7
#define CODE_TEMPO_ON_VIEW         8
#define CODE_TEMPO_OFF_SETAR       9
#define CODE_TEMPO_OFF_VIEW       10

#define CODE_CONDENSADOR_ON       20
#define CODE_CONDENSADOR_OFF      21
#define CODE_VACCUM_ON            22
#define CODE_VACCUM_OFF           23
#define CODE_AQUECIMENTO_ON       24
#define CODE_AQUECIMENTO_OFF      25
#define CODE_DATALOG_ON           26
#define CODE_DATALOG_OFF          27

#define CODE_LIOFILIZAR_AUTO_ON  100
#define CODE_LIOFILIZAR_AUTO_OFF 101
#define CODE_DEFAULT_FACTORE     145











#endif
