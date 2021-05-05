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
        unsigned buzzer : 1;
        unsigned rele_condensador : 1;
        unsigned rele_sensor_ntc : 1;
        unsigned rele_vaccum : 1;
        unsigned rele_comum : 1;
        unsigned reserved5 : 1;
        unsigned reserved6 : 1;
        unsigned reserved7 : 1;
    };
}chip_t;


class Sobressalente74HC595 {
public:
    void init();    
    void chipSetValue(uint8_t pin, uint8_t estado);
    void desligaAll();
    //void gravaChip(uint8_t value);
    chip_t chip;
private:
};


#endif
