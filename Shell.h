#ifndef TERMINAL_INDKA_H
#define TERMINAL_INDKA_H

#define PROMPT "Indka>"



class Shell{
  public:
      Shell();
      void init();
      void prompt();
      void empresa();      
      void mostraTempo(); 
      void HelpHandlerThread();
      void blkPrintln(String text);
      void blkPrint(String text); 
      void blkPrint(double num);
      void blkPrintln(double num);
      void blkPrint(double num, uint8_t decimal);
      void blkPrintln(double num, uint8_t decimal);
      void exibeDados();
      String extraiProximoParametro(String* buffer, char caracter);
  private:

};



#endif
