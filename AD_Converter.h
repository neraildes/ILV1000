#ifndef AD_CONVERTER_H
#define AD_CONVERTER_H

#include "global.h"

typedef struct {
	           float   value[MAXSUAVIZA];
			   uint8_t count=0;
               }Suaviza_t;


class AD_Converter {
public:
	AD_Converter();
	float read_pin(uint8_t pin, uint8_t modo);     //pin=pino do arduindo, modo de captura  MINIMO, MAXIMO, MEDIA
	float suaviza(float entrada, uint8_t device);
private:
};


#endif // !AD_Converter
