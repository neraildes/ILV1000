#ifndef SOBRESSALENTE74HC595_H
#define SOBRESSALENTE74HC595_H


#define LIGADO    0
#define DESLIGADO 1


#define PIN_COMUM       0
#define PIN_CONDENSADOR 1
#define PIN_SENSORNTC   2
#define PIN_VACUO       3
#define PIN_DATALOG     4
#define PIN_BUZZER      5
#define PIN_RESERVED6   6
#define PIN_RESERVED7   7



typedef union 
{
    uint8_t value;
    struct
    {
        unsigned relay_comum       : 1;        
        unsigned relay_condensador : 1;
        unsigned relay_aquecimento : 1;  //Fix Inverter no Hardware condensador e aquecimento        
        unsigned relay_vacuo      : 1;
        unsigned relay_datalog     : 1; 
        unsigned relay_buzzer      : 1;
        unsigned reserved5         : 1;
        unsigned reserved6         : 1; //flag processo
        
    };
}chip_t;



class Sobressalente74HC595 {
public:
    void init();    
    //void chipSetValue(uint8_t pin, uint8_t estado);
    void desligaAll();
    chip_t chip;
private:
};

#define relay_comum        extra74HC595.chip.relay_comum
#define relay_condensador  extra74HC595.chip.relay_condensador
#define relay_aquecimento  extra74HC595.chip.relay_aquecimento
#define relay_vacuo       extra74HC595.chip.relay_vacuo
#define relay_datalog      extra74HC595.chip.relay_datalog
#define relay_buzzer       extra74HC595.chip.relay_buzzer


#endif
