#ifndef SOBRESSALENTE74HC595_H
#define SOBRESSALENTE74HC595_H


#define LIGADO    0
#define DESLIGADO 1

#define PIN_BUZZER 0
#define PIN_CONDENSADOR 1
#define PIN_SENSORNTC 2
#define PIN_VACCUM 3
#define PIN_COMUM 4
#define PIN_RESERVED5 5
#define PIN_RESERVED6 6
#define PIN_RESERVED7 7


typedef union 
{
    uint8_t value;
    struct
    {
        unsigned relay_buzzer      : 1;
        unsigned relay_condensador : 1;
        unsigned relay_aquecimento : 1;  //Fix Inverter no Hardware condensador e aquecimento        
        unsigned relay_vaccum      : 1;      
        unsigned relay_comum       : 1;
        unsigned reserved5 : 1;
        unsigned reserved6 : 1;
        unsigned reserved7 : 1;
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

#define relay_buzzer       extra74HC595.chip.relay_buzzer
#define relay_condensador  extra74HC595.chip.relay_condensador
#define relay_aquecimento  extra74HC595.chip.relay_aquecimento
#define relay_vaccum       extra74HC595.chip.relay_vaccum
#define relay_comum        extra74HC595.chip.relay_comum



#endif
