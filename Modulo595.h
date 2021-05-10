

#ifndef MODULO595_H
#define MODULO595_H

extern uint8_t  hora;
extern uint8_t  minuto;
extern uint8_t  segundo;
extern uint16_t milisegundo;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class Modulo595{
  public:
      Modulo595(uint8_t clk, uint8_t dta, uint8_t lth);
      void    clearDisplay();
      void    sendDisplayMessage(String text, uint8_t status);
      void    sendDisplay(String text, uint8_t status);
      void    sendDisplay(float nfloat, uint8_t status);
      void    sendPair(uint8_t value, uint8_t q);
      void    sendSingle(uint8_t value);

      void    pulse(void);
      void    show(void);
      uint8_t transistor(uint8_t value);
      uint8_t decode(uint8_t value);
      int8_t  findChar(char txt[10], char c);
  private:
      uint8_t _clk;
      uint8_t _dta;
      uint8_t _lth;
};

#endif
