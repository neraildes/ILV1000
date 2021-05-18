#ifndef PERSISTENTE_H
#define PERSISTENTE_H

typedef union 
{
    uint8_t value;
    struct
    {
        unsigned flag_processo_auto : 1;        
        unsigned flag_condensador   : 1;
        unsigned flag_vacuo         : 1;
        unsigned flag_aquecimento   : 1;
        unsigned flag_datalog       : 1;
        unsigned reserved5 : 1;
        unsigned reserved6 : 1;                        
        unsigned reserved7 : 1;   
    };
}statusgen_t;

#define flag_processo_auto persistente.statusgen.flag_processo_auto
#define flag_condensador   persistente.statusgen.flag_condensador
#define flag_vacuo         persistente.statusgen.flag_vacuo
#define flag_aquecimento   persistente.statusgen.flag_aquecimento
#define flag_datalog       persistente.statusgen.flag_datalog


typedef struct{
  uint8_t hora;
  uint8_t minuto;
} processoTime_t;

class Persistente{
  public:
    Persistente();
    void init();
    void save();
    void restore();
    statusgen_t statusgen;
    processoTime_t processoTime;
};


#endif
