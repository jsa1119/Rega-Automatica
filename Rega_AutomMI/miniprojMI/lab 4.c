#include <16F877A.h>
#fuses NOWDT,HS, NOPUT, NOPROTECT, NODEBUG, NOLVP, NOCPD, NOWRT, BROWNOUT

#use delay(clock=20000000)

#define RS     PIN_E0
#define ENABLE PIN_E1
//#define LSB PIN_A2

int caracter;
int instrucao;
int16 vetor[6]={1234,1235,1236,1237,1238,1239};

void activa()
{
  output_high(ENABLE);
  delay_ms(20);
  output_low(ENABLE);
}


void transfere_carac(int carac)
{
  output_high(RS);
  output_d(carac);
  activa();
}


void transfere_inst(int inst)
{
  output_low(RS);
  output_d(inst);
  activa();
}


void main()
{
   int i;
   int j;
   int unidades,dezenas,centenas;


   set_tris_d(0x00);




  instrucao=56;
  transfere_inst(instrucao);
  instrucao=1;
  transfere_inst(instrucao);
  instrucao=12;
  transfere_inst(instrucao);
  instrucao=2;                // colocação do cursor na 1º linha
  transfere_inst(instrucao);

  while (1)
   {

      caracter = 'M';
      transfere_carac(caracter);
      caracter = 'i';
      transfere_carac(caracter);
      caracter = 'c';
      transfere_carac(caracter);
      caracter = 'r';
      transfere_carac(caracter);
      caracter = 'o';
      transfere_carac(caracter);
      caracter = '=';
      transfere_carac(caracter);
//!      for(i=1;i<=255;i++)
//!      {
//!        delay_ms(100);
//!
i=1234;
         centenas=i/100;
         caracter =centenas+48;
         transfere_carac(caracter);
         dezenas=(i%100)/10;
         caracter =dezenas+48;
         transfere_carac(caracter);
         unidades=(i%100)%10;
         caracter =unidades+48;
         transfere_carac(caracter);
         
         if

//!         instrucao=135;                // colocação do cursor na 7º posição da linha 1
//!         transfere_inst(instrucao);
//!      }

      instrucao=2;                // colocação do cursor na 1º linha
      transfere_inst(instrucao);

   }
}
