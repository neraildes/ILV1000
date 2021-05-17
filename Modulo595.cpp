#include "Arduino.h"
#include "Modulo595.h"
#include "global.h"
#include "stdio.h"
#include "string.h"
#include "Sobressalente74HC595.h"
#include "Blk_terminal.h"


extern Sobressalente74HC595 extra74HC595;

//=============================================================================================================
Modulo595::Modulo595(uint8_t dta, uint8_t clk, uint8_t lth)
    {
    _clk=clk;
    _dta=dta;
    _lth=lth;  
    pinMode(_clk, OUTPUT);
    pinMode(_dta, OUTPUT);
    pinMode(_lth, OUTPUT);
    digitalWrite(_clk, 0);
    digitalWrite(_dta, 0);
    digitalWrite(_lth, 0);
    }

//=============================================================================================================
void Modulo595::clearDisplay()
     {    
     for(uint16_t i=0;i<250;i++)
         {
         sendDisplay("    ", NORMAL);
         sendDisplay("    ", NORMAL);
         sendDisplay("    ", NORMAL);
         sendDisplay("    ", NORMAL);
         sendSingle(0b00011110);
         show();
         delay(1);
         }
     }    

uint8_t Modulo595::sendDisplayMessage(String text, uint8_t status)
{
     String TextOut;

     TextOut="    "+text;
     TextOut+="        ";
     if(sendDisplay(TextOut, status))
       {
       sendDisplay(TextOut, status);
       sendDisplay(TextOut, status);
       sendDisplay(TextOut, status);
       sendDisplay(TextOut, status); 
       return 1;
       }
    return 0;   
}


     

//====================================================================================
uint8_t Modulo595::sendDisplay(String text, uint8_t status){
   #define REPEAT 180
   static int8_t i=0;
   static int8_t q=0;
   static int8_t cnt=0;
   static int16_t repeat=0;  
   static uint8_t vector[4];   
   int8_t  ponto=-1;
   int8_t  pointer;
   uint8_t shift=0;
   char  textOut[50];
   uint8_t indexT=0;    
   uint8_t indexO=0;

   //text="13.45";
   
   pointer=vector[cnt];
   

   if((text.length()>8)&&(q==0))
     {
     if(++repeat==REPEAT)
       {
       repeat=0;
       if(text[pointer+6]==0)
         {
         pointer=0;
         vector[cnt]=pointer;
         return 1;     
         }
       pointer++; 
       vector[cnt]=pointer;
       }
     }
   else
     {
     if((vector[cnt]>8)&&(text.length()<8)) pointer=0;  
     }

   
   //textOut="";
   indexT=0;
   indexO=0;
   while(1)
      {
        
      if(text[indexT+1]=='.')
        {
        textOut[indexO]=codigo(text[indexT])&codigo('.'); 
        indexT+=1;
        }
     else
        {
        textOut[indexO]=codigo(text[indexT]);
        }
       indexT++;
       indexO++;               
       if(indexT>=text.length())break;
      }      
      textOut[indexO]=0;
  
   sendPair(textOut[pointer+i],3-q);
     
   if(++cnt==4)
     {
      cnt=0;
      q++;
      q%=4;
      i++;
      if(i>3)i=0;
     }

   return 0;
}


//=============================================================================================================
void Modulo595::sendDisplay(float nfloat, uint8_t status)
     {
     char   chrtexto[10];
     String strtexto;
     dtostrf(nfloat, 4, 4, chrtexto);//variável, minimo, precisão, string.
     if(status==PISCA)
       {
         if (milisegundo % TEMPO_PISCA < (TEMPO_PISCA / 2)) strcpy(chrtexto,"    ");
       }    
     chrtexto[5]=0;  
     strtexto = chrtexto;        
     sendDisplay(strtexto, NORMAL);
     }
        


//=============================================================================================================
void Modulo595::sendPair(uint8_t value, uint8_t q)
     {
     sendSingle(value);//decode(value)
     sendSingle(transistor(q));
     }

//=============================================================================================================
void Modulo595::sendSingle(uint8_t value)
     {
     for(int8_t i=7; i>=0; i--)
         {
         digitalWrite(_dta,bitRead(value,i)); 
         pulse();
         }
     }

//=============================================================================================================
void Modulo595::pulse(void)
    { 
    //delay(3);
    digitalWrite(_clk, HIGH);
    delayMicroseconds(2);
    digitalWrite(_clk, LOW);
    //delayMicroseconds(10);
    }    

//=============================================================================================================
void Modulo595::show(void)
    {
    //delayMicroseconds(100);
    digitalWrite(_lth, HIGH);
    //delayMicroseconds(100);
    digitalWrite(_lth, LOW);
    //delayMicroseconds(100);
    }

//=============================================================================================================
uint8_t Modulo595::codigo(uint8_t value){
   
   switch (value)
          {
          case '0': return 0xC0; break; //03
          case '1': return 0xF9; break; //9f
          case '2': return 0XA4; break; //24
          case '3': return 0XB0; break;
          case '4': return 0X99; break;
          case '5': return 0X92; break;
          case '6': return 0X82; break;
          case '7': return 0XF8; break;
          case '8': return 0X80; break;
          case '9': return 0X90; break;
          case 'a':
          case 'A': return 0x88; break;
          case 'b':
          case 'B': return 0x83; break;
          case 'C': return 0xC6; break;
          case 'c': return 0xA7; break;          
          case 'd':
          case 'D': return 0xA1; break;
          case 'e':
          case 'E': return 0x86; break;
          case 'f':
          case 'F': return 0x8E; break;
          case 'g':
          case 'G': return 0x82; break;
          case 'h':
          case 'H': return 0x89; break;
          case 'I': return 0xF9; break;
          case 'i': return 0xDF; break;
          case 'j':          
          case 'J': return 0xF1; break;
          case 'k':
          case 'K': return 0xBF; break;
          case 'l':
          case 'L': return 0xC7; break;
          case 'm':
          case 'M': return 0xBF; break;
          case 'n':
          case 'N': return 0xAB; break;
          case 'O': return 0xC0; break;
          case 'o': return 0xA3; break;
          case 'p':
          case 'P': return 0x8C; break;
          case 'q':
          case 'Q': return 0x98; break;
          case 'r':
          case 'R': return 0xAF; break;
          case 's':
          case 'S': return 0x92; break;
          case 't':
          case 'T': return 0x87; break;
          case 'u':
          case 'U': return 0xC1; break;
          case 'v':
          case 'V': return 0xBF; break;
          case 'x':
          case 'X': return 0xBF; break;
          case 'y':
          case 'Y': return 0x91; break;
          case 'z':
          case 'Z': return 0xBF; break;
          case 'w':
          case 'W': return 0xBF; break;
          case '-': return 0XBF; break;
          case '.': return 0X7F; break;
          case ' ': return 0xFF; break;
          case '_': return 0xF7; break;
          default : return value; break;

          }
    return 0;
}

//=============================================================================================================
uint8_t Modulo595::transistor(uint8_t value)
    {
      switch (value)
      {
      case 0: return 1 ; break;
      case 1: return 2 ; break;
      case 2: return 4 ; break;
      case 3: return 8; break;
      default: return 0; break;
      }
    }


int8_t Modulo595::findChar(char txt[10], char c) {
    int8_t resposta;
    resposta = -1;
    for(int8_t i=0;i<strlen(txt);i++)
       { 
        if (txt[i] == c)
           {
            resposta = i;
           } break;
       }
    return resposta;
}

//=============================================================================================================
/*
  LED_0F[0] = 0xC0; //0
  LED_0F[1] = 0xF9; //1
  LED_0F[2] = 0xA4; //2
  LED_0F[3] = 0xB0; //3
  LED_0F[4] = 0x99; //4
  LED_0F[5] = 0x92; //5
  LED_0F[6] = 0x82; //6
  LED_0F[7] = 0xF8; //7
  LED_0F[8] = 0x80; //8
  LED_0F[9] = 0x90; //9
  LED_0F[10] = 0x88; //A
  LED_0F[11] = 0x83; //b
  LED_0F[12] = 0xC6; //C
  LED_0F[13] = 0xA1; //d
  LED_0F[14] = 0x86; //E
  LED_0F[15] = 0x8E; //F
  LED_0F[16] = 0xC2; //G
  LED_0F[17] = 0x89; //H
  LED_0F[18] = 0xF9; //I
  LED_0F[19] = 0xF1; //J
  LED_0F[20] = 0xC3; //L
  LED_0F[21] = 0xA9; //n
  LED_0F[22] = 0xC0; //O
  LED_0F[23] = 0x8C; //P
  LED_0F[24] = 0x98; //q
  LED_0F[25] = 0x92; //S
  LED_0F[26] = 0xC1; //U
  LED_0F[27] = 0x91; //Y
  LED_0F[28] = 0xFE; //hight -  
*/
