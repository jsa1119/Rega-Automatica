/*
Sistema de Rega Automatizado para 3 Zonas de Rega- este sistema ir� controlar a hora e o tempo de rega de 3 zonas diferentes em simult�neo ou.
Os valores da hora e do tempo desejados ser�o introduzidos por bot�es de press�o. 
Este miniprojecto ter� um LCD, 3 valvulas de �gua, bot�es de press�o, teclados e sistema de
alimenta��o autonomo.

*/

#include <16F877A.h>
#device adc =10
#fuses NOWDT,HS, NOPUT, NOPROTECT, NODEBUG, NOLVP, NOCPD, NOWRT, BROWNOUT

#use delay(clock=20000000)
#use rs232(baud = 9600, xmit = PIN_C6, rcv = PIN_C7, parity = n, bits =8) // paridade conta os numeros dos 1


#define RS     PIN_E0
#define ENABLE PIN_E1
#define WDT_18MS


                                            // Variaveis Globais
int16 caracter, instrucao;
int16 contador=0, horas=0, minutos=0, segundos = 0;
   int16 hu=0, hd=0, mu=0, md=0, su=0, sd=0; 
   char tecla;

                           // variaveis de defini��o da hora da rega
   int dhz1=2, uhz1=3, dmz1=5, umz1=9; // vari�veis de zona 1
   int dhz2=2, uhz2=3, dmz2=5, umz2=9; // vari�veis de zona 2
   int dhz3=2, uhz3=3, dmz3=5, umz3=9; // vari�veis de zona 3
   
                        // variaveis da defini��o de tempo de rega
   int16 mintZ1=2, mintZ2=2, mintZ3=2;
                      // variaveis de acerto das horas      
   int hacdh, hacuh, hacdm, hacum;
         



//rotina de interrup��o Interna com o timer 0
#int_rtcc
void rtcc_interrup(){

contador++;
if(contador == 8333) // 1s = 1/10u
{
contador = 0;
segundos ++;
if(segundos > 59){
minutos++;
segundos = 0;
}
if(minutos >59){
horas++;
minutos = 0;
}
if(horas > 23)
horas = 0;

   

}//fim do if

}// fim da rotina



                                              // Prototipagens das fun�oes do programa
// fun�ao ativa(Enable)

void activa(){
output_high(ENABLE);
delay_ms(20);
output_low(ENABLE);
}
// fun�ao transfere caracter
void transfere_carac(int carac){
output_high(RS);
output_d(carac);
activa();
}
// fun�ao transfere instru�ao
void transfere_inst(int inst){
output_low(RS);
output_d(inst);
activa();
}
// fun�ao que inicializa lcd
void ini_lcd(){
//lcd_load (instrucao, 0, 16);
instrucao = 56; // 8bits 2linhas
transfere_inst(instrucao);
instrucao = 1; // Limpa display
transfere_inst(instrucao);
instrucao = 12;
transfere_inst(instrucao);
instrucao = 2;
transfere_inst(instrucao);

}
                                                                  // prototipagem das fun��es secundarias
  
  void fun_horas(void); // fun��o que controla horas
  int menu(void); // menu do programa
  void hacerto(void); //acerto das horas (horas e minutos)
  void def_hregaZ1(void); // define a hora da rega da 1� zona
  void def_hregaZ2(void); // define a hora da rega da 2� zona
  void def_hregaZ3(void); // define a hora da rega da 3� zona
  void ler_tecla(void); // tecla uso no programa
  




                                                                 // fun�ao principal do programa
void main()
{ 
     // declara�ao de variaveis locais
 char i;
 int z1=0, z2=0, z3=0;
 
    port_b_pullups(0xF0);
    set_tris_b(0xF0);// 0 sa�das e 1 entradas
    set_tris_d(0x00);
    ini_lcd();
    
       // Ativa�ao (setup) e identifica�ao dos portos
   setup_timer_0(RTCC_INTERNAL | RTCC_DIV_2);// 8 bits a resolu��o � (256*2*4)20000000 = 102u  
   setup_adc_ports(AN0);
   setup_adc(ADC_clock_internal);
   enable_interrupts(int_rtcc);
   enable_interrupts(global);   
   
                          // Leitura dos estado anterior das vari�veis guardadas no EEPROM nos seus respetivos endere�os
                  // Zona 1
   dhz1 = read_EEPROM (00);
   uhz1 = read_EEPROM (01);
   dmz1 = read_EEPROM (02);
   umz1 = read_EEPROM (03);
   mintZ1 = read_EEPROM (04);   
                  // Zona 2
   dhz2 = read_EEPROM (05);
   uhz2 = read_EEPROM (06);
   dmz2 = read_EEPROM (07);
   umz2 = read_EEPROM (10);  // endere�o 8 e 9? s�o endere�os reservados 
   mintZ2 = read_EEPROM (11);
                  // Zona 3
   dhz3 = read_EEPROM (12);
   uhz3 = read_EEPROM (13);
   dmz3 = read_EEPROM (14);
   umz3 = read_EEPROM (15);
   mintZ3 = read_EEPROM (16);


// ciclo infinito de programa

  while (1)
   {
  
    fun_horas(); // Invoca a fun��o fun_horas
    
   instrucao = 192;
   transfere_inst(instrucao);
   caracter = 'A'; // Representa o menu do programa
   transfere_carac(caracter);
   instrucao = 194;
   transfere_inst(instrucao);
   caracter = 'E'; // Representa o menu do programa
   transfere_carac(caracter);
   
   
   
   
   // Tecla do menu e aceroto das horas
           //hacerto()
    tecla = '#';
  output_high(pin_b0);
  output_low(pin_b1);
  output_high(pin_b2);
  output_high(pin_b3);
     while((input(pin_b7)==0) && (input(pin_b7)==0) && (input(pin_b7)==0)){
   tecla='E';
    }
   
    // menu()
  output_high(pin_b0);
  output_high(pin_b1);
  output_high(pin_b2);
  output_low(pin_b3);

  while((input(pin_b4)==0) && (input(pin_b4)==0) && (input(pin_b4)==0)){
   tecla='A';
   }// fim de tecla do menu
       // invoca��o da fun��o menu
   if(tecla == 'A'){
   i  = menu();
   
   }  
   
   // invoca��o da fun��o hacerto
   if(tecla == 'E'){
      hacerto();
       if(tecla == 'C'){
        horas = (hacdh*10)+ hacuh;
        minutos = (hacdm*10)+ hacum;
       }// fim de if de grava��o da tecla
   }// fim de hacerto
   
   
     // As atuliza��es das horas de regas 

     if(tecla == '1' || tecla == '2' || tecla == '3'){
     switch(i){
     case '1': // op��o 1. Zona1
        def_hregaZ1();
     break;
     
     case '2': // op��o 2. Zona 2
        def_hregaZ2();
     
     break;
     
                                       // Tempo de rega para Zona 1
     case '3': // op��o 3
        def_hregaZ3();

     break;
   
     
     
     }//fim do switch
     instrucao = 1;
     transfere_inst(instrucao);
     
     
  }// fim do if
      
                                  // Controlo de rega da zona 1
     
      if(hd == dhz1){
         if(hu == uhz1){
         if(md == dmz1){
         if(mu == umz1){
         output_high(pin_A2);
         z1=1;
         instrucao = 139;
         transfere_inst(instrucao);
         caracter = 'Z';
         transfere_carac(caracter);
         caracter = '1';
         transfere_carac(caracter);
         caracter = 'O';
         transfere_carac(caracter);
         caracter = 'n';
         transfere_carac(caracter);
          caracter = mintZ1+48;
          transfere_carac(caracter);
         }// 4�
         }// 3�
         }// 2�
      }// 1� if 
      
      if(mu == umz1+mintZ1){
      output_low(pin_A2);
      z1=0;
         
      }// fim de if de low da z1
      
      if(z1 == 0){
         instrucao = 139;
         transfere_inst(instrucao);
         caracter = 'Z';
         transfere_carac(caracter);
         caracter = '1';
         transfere_carac(caracter);
         caracter = 'O';
         transfere_carac(caracter);
         caracter = 'f';
         transfere_carac(caracter);
          caracter = mintZ1+48;
          transfere_carac(caracter);
         
         
      }// fim de if off
      
       // Controlo de rega da zona 2
      
      if(hd == dhz2){
         if(hu == uhz2){
         if(md == dmz2){
         if(mu == umz2){
         output_high(pin_A3);
          z2=1;
         instrucao = 197;
         transfere_inst(instrucao);
         caracter = 'Z';
         transfere_carac(caracter);
         caracter = '2';
         transfere_carac(caracter);
         caracter = 'O';
         transfere_carac(caracter);
         caracter = 'n';
         transfere_carac(caracter);
          caracter = mintZ2+48;
          transfere_carac(caracter);
         }// 4�
         }// 3�
         }// 2�
      }// 1� if 
      
      if(mu == umz2+mintZ2){
      output_low(pin_A3);
      z2 = 0;
      }
      if(z2 == 0){
         instrucao = 197;
         transfere_inst(instrucao);
         caracter = 'Z';
         transfere_carac(caracter);
         caracter = '2';
         transfere_carac(caracter);
         caracter = 'O';
         transfere_carac(caracter);
         caracter = 'f';
         transfere_carac(caracter);
         caracter = mintZ2+48;
         transfere_carac(caracter);
         
      }// fim de if off
      
      
       // Controlo de rega da zona 3
      
      if(hd == dhz3){
         if(hu == uhz3){
         if(md == dmz3){
         if(mu == umz3){
         output_high(pin_C4);
         z3=1;
         instrucao = 203;
         transfere_inst(instrucao);
         caracter = 'Z';
         transfere_carac(caracter);
         caracter = '3';
         transfere_carac(caracter);
         caracter = 'O';
         transfere_carac(caracter);
         caracter = 'n';
         transfere_carac(caracter);
         caracter = mintZ3+48;
         transfere_carac(caracter);
         }// 4�
         }// 3�
         }// 2�
      }// 1� if 
      
      if(mu == umz3 + mintZ3){
      output_low(pin_C4);
      z3 = 0;
      }
      if(z3 == 0){
         instrucao = 203;
         transfere_inst(instrucao);
         caracter = 'Z';
         transfere_carac(caracter);
         caracter = '3';
         transfere_carac(caracter);
         caracter = 'O';
         transfere_carac(caracter);
         caracter = 'f';
         transfere_carac(caracter);
         caracter = mintZ3+48;
         transfere_carac(caracter);
         
      }// fim de if off





   }// fim do while 1   
}// Fim do main


                                                                // Prototipagem das fun��es secundarias do programa
                                 
 //  fun��o horas
void fun_horas(){

      instrucao = 128;
      transfere_inst(instrucao);
              
              // horas
      hd = horas/10;
      caracter = hd+48;
    transfere_carac(caracter);
      hu = horas%10;
      caracter = hu+48;
    transfere_carac(caracter);
      caracter = ':';
    transfere_carac(caracter);
         
         // minutos
      md = minutos/10;
      caracter = md+48;
    transfere_carac(caracter);
      mu = minutos%10;   
      caracter = mu+48;
    transfere_carac(caracter);
     caracter = ':';
    transfere_carac(caracter);
    
    // segundos
   sd = segundos/10;
   caracter = sd+48;
   transfere_carac(caracter);
   su = segundos%10;
   caracter = su+48;
   transfere_carac(caracter);


} // fim da fun_horas



// fun��o menu do programa
int menu(void){

instrucao = 1;  // limpa lcd e manda o cursor para primeira posiss�o
transfere_inst(instrucao);
instrucao = 128;  
transfere_inst(instrucao);


caracter = '1';
transfere_carac(caracter);
caracter = '-';
transfere_carac(caracter);
caracter = ' ';
transfere_carac(caracter);
caracter = 'Z';
transfere_carac(caracter);
caracter = '1';
transfere_carac(caracter);
instrucao = 136;
transfere_inst(instrucao);
caracter = '2';
transfere_carac(caracter);
caracter = '-';
transfere_carac(caracter);
caracter = ' ';
transfere_carac(caracter);
caracter = 'Z';
transfere_carac(caracter);
caracter = '2';
transfere_carac(caracter);


// linha 2 de lcd
instrucao = 192;
transfere_inst(instrucao);

caracter = '3';
transfere_carac(caracter);
caracter = '-';
transfere_carac(caracter);
caracter = ' ';
transfere_carac(caracter);
caracter = 'Z';
transfere_carac(caracter);
caracter = '3';
transfere_carac(caracter);
instrucao = 200;
transfere_inst(instrucao);
caracter = 'B';                // back 'B' para sair do menu 
transfere_carac(caracter);
caracter = '-';
transfere_carac(caracter);
caracter = ' ';
transfere_carac(caracter);
caracter = 'S';
transfere_carac(caracter);
caracter = 'a';
transfere_carac(caracter);
caracter = 'i';
transfere_carac(caracter);
caracter = 'r';
transfere_carac(caracter);

// tecla de menu
do{
   ler_tecla();
   
}while(tecla!='1' && tecla != '2' && tecla != '3' && tecla != 'B'); // fim do w
transfere_carac(tecla);
instrucao = 1;  // limpa lcd e manda o cursor para primeira posiss�o
transfere_inst(instrucao);  

return(tecla);
} // fim da fun��o menu



 //acerto das horas (horas e minutos)
void hacerto(){

   
     instrucao = 1;  // limpa lcd e manda o cursor para primeira posiss�o
     transfere_inst(instrucao);
     hacdh = horas/10;
     hacuh = horas%10;
     hacdm = minutos/10;
     hacum = minutos%10;
   do{
    tecla = '#';
    instrucao = 135;
    transfere_inst(instrucao);
    
    
    if(input(pin_C0)== 1){
      delay_ms(100);
      hacdh = hacdh +1;
      }
      if(input(pin_C1)== 1){
      delay_ms(100);
      hacuh = hacuh+1;
      }
      if(input(pin_C2)== 1){
      delay_ms(100);
      hacdm = hacdm+1;
      }
      if(input(pin_C3)== 1){
      delay_ms(100);
      hacum = hacum+1;
      }
      
                                 // controlo da introdu��o da hora
         
      if(hacdh>2)
      hacdh =0;
      if(hacuh>9)
      hacuh = 0;
      if(hacdm>5)
      hacdm =0;
      if(hacum>9)
      hacum = 0;
      
      // limita��o das 23h
      if(hacdh == 2){
      if(hacuh>3)
      hacuh = 0;
      }
      
     
          
          caracter = hacdh+48;
          transfere_carac(caracter);
          caracter = hacuh+48;
          transfere_carac(caracter);
          caracter = ':';
          transfere_carac(caracter);
          caracter = hacdm+48;
          transfere_carac(caracter);
          caracter = hacum+48;
          transfere_carac(caracter);
          
           
    ler_tecla();
  
     
  }while(tecla!='C' && tecla != 'B' );    // fim do while
   

     instrucao = 1;  // limpa lcd e manda o cursor para primeira posiss�o
     transfere_inst(instrucao);
 }//fim do acerto das horas
  
 
 
 
 
 // fun��o que define a hora de rega da zona 1
void def_hregaZ1(){
 int dhoras,uhoras, dminutos, uminutos;    
 
// definir a hora de rega da zona 1
 
  do{
    tecla = '#';

    
    if(input(pin_C0)== 1){
      delay_ms(100);
      dhz1= dhz1 +1;
      }
      if(input(pin_C1)== 1){
      delay_ms(100);
      uhz1 = uhz1+1;
      }
      if(input(pin_C2)== 1){
      delay_ms(100);
      dmz1 = dmz1+1;
      }
      if(input(pin_C3)== 1){
      delay_ms(100);
      umz1 = umz1+1;
      }
      
                                 // controlo da introdu��o da hora
         
      if(dhz1>2)
      dhz1 =0;
      if(uhz1>9)
      uhz1 = 0;
      if(dmz1>5)
      dmz1 =0;
      if(umz1>9)
      umz1 = 0;
      
      // limita��o das 23h
      if(dhz1 == 2){
      if(uhz1>3)
      uhz1 = 0;
      }
      
     // Definir tempo de rega
      if(mintZ1 >9){
      mintZ1 = 0;
      }
                              
          instrucao = 128;
          transfere_inst(instrucao);
          caracter = 'Z';
          transfere_carac(caracter);
          caracter = 'o';
          transfere_carac(caracter);
          caracter = 'n';
          transfere_carac(caracter);
          caracter = 'a';
          transfere_carac(caracter);
          caracter = '1';
          transfere_carac(caracter);
          
          instrucao = 136;
          transfere_inst(instrucao);
          dhoras = dhz1;
          caracter = dhoras+48;
          transfere_carac(caracter);
          uhoras = uhz1;
          caracter = uhoras+48;
          transfere_carac(caracter);
          caracter = ':';
          transfere_carac(caracter);
          dminutos= dmz1;
          caracter = dminutos+48;
          transfere_carac(caracter);
          uminutos = umz1;
          caracter = uminutos+48;
          transfere_carac(caracter);
          instrucao = 192;
          transfere_inst(instrucao);
          caracter = 'T';
          transfere_carac(caracter);
          caracter = '_';
          transfere_carac(caracter);
          caracter = 'R';
          transfere_carac(caracter);
          caracter = 'e';
          transfere_carac(caracter);
          caracter = 'g';
          transfere_carac(caracter);
          caracter = 'a';
          transfere_carac(caracter);
          instrucao = 202;
          transfere_inst(instrucao);
          caracter = mintZ1+48;
          transfere_carac(caracter);
           
    ler_tecla();
  
     
  }while(tecla!='C' && tecla != 'C' && tecla != 'C' && tecla != 'C');    // fim do while
   
                          // As vari�veis s�o guardados nos endere�os
      write_eeprom(00,dhz1); // 0
      write_eeprom(01,uhz1); // 1
      write_eeprom(02,dmz1); // 2
      write_eeprom(03,umz1); // 3
   
                              
                                         // Definir tempo de rega da zona 1
                                         
       do{
          instrucao = 192;
          transfere_inst(instrucao);
          caracter = 'T';
          transfere_carac(caracter);
          caracter = '_';
          transfere_carac(caracter);
          caracter = 'R';
          transfere_carac(caracter);
          caracter = 'e';
          transfere_carac(caracter);
          caracter = 'g';
          transfere_carac(caracter);
          caracter = 'a';
          transfere_carac(caracter);
          instrucao = 202;
          transfere_inst(instrucao);
    
    if(input(pin_C0)== 1 && input(pin_C0)== 1 && input(pin_C0)== 1 && input(pin_C0)== 1){
      delay_ms(100);
      mintZ1 = mintZ1 + 1;
      }
        
      if(mintZ1 > 9){
      mintZ1 = 0;
      }
      

          caracter = mintZ1+48;
          transfere_carac(caracter);
            
     
    ler_tecla();
  
     
  }while(tecla!='C' && tecla != 'C' && tecla != 'C' && tecla != 'C');    // fim do while
      
      write_eeprom(04,mintZ1); // Vari�vel mintZ1 � guardado no endere�o 4
}//fim da fun��o def_hrega da zona 1

 
// definir a hora de rega da zona 2

 void def_hregaZ2(){
 int dhoras,uhoras, dminutos, uminutos;
  do{
    tecla = '#';
    
    if(input(pin_C0)== 1){
      delay_ms(100);
      dhz2= dhz2 +1;
      }
      if(input(pin_C1)== 1){
      delay_ms(100);
      uhz2 = uhz2+1;
      }
      if(input(pin_C2)== 1){
      delay_ms(100);
      dmz2 = dmz2+1;
      }
      if(input(pin_C3)== 1){
      delay_ms(100);
      umz2 =umz2+1;
      }
                                  // controlo da introdu��o das horas
               
      if(dhz2>2)
      dhz2 =0;
      if(uhz2>9)
      uhz2 = 0;
      if(dmz2>5)
      dmz2 =0;
      if(umz2>9)
      umz2 = 0;
      
      // limita��o das 23h
      if(dhz2 == 2){
      if(uhz2>3)
      uhz2 = 0;
      }
          instrucao = 128;
          transfere_inst(instrucao);
          caracter = 'Z';
          transfere_carac(caracter);
          caracter = 'o';
          transfere_carac(caracter);
          caracter = 'n';
          transfere_carac(caracter);
          caracter = 'a';
          transfere_carac(caracter);
          caracter = '2';
          transfere_carac(caracter);
      
          instrucao = 136;
          transfere_inst(instrucao);
          dhoras = dhz2;
          caracter = dhoras+48;
          transfere_carac(caracter);
          uhoras = uhz2;
          caracter = uhoras+48;
          transfere_carac(caracter);
          caracter = ':';
          transfere_carac(caracter);
          dminutos= dmz2;
          caracter = dminutos+48;
          transfere_carac(caracter);
          uminutos = umz2;
          caracter = uminutos+48;
          transfere_carac(caracter);
          instrucao = 192;
          transfere_inst(instrucao);
          caracter = 'T';
          transfere_carac(caracter);
          caracter = '_';
          transfere_carac(caracter);
          caracter = 'R';
          transfere_carac(caracter);
          caracter = 'e';
          transfere_carac(caracter);
          caracter = 'g';
          transfere_carac(caracter);
          caracter = 'a';
          transfere_carac(caracter);
          instrucao = 202;
          transfere_inst(instrucao);
          caracter = mintZ2+48;
          transfere_carac(caracter);
        
    
    
   
     
    ler_tecla();
  
     
  }while(tecla!='C' && tecla != 'C' && tecla != 'C' && tecla != 'C');    // fim do while
  
          // As vari�veis s�o guardados nos endere�os
      write_eeprom(05,dhz2); // 5
      write_eeprom(06,uhz2); // 6
      write_eeprom(07,dmz2); // 7
      write_eeprom(10,umz2); // 10
       
                                    // Tempo de rega para Zona 2
      do{
          instrucao = 192;
          transfere_inst(instrucao);
          caracter = 'T';
          transfere_carac(caracter);
          caracter = '_';
          transfere_carac(caracter);
          caracter = 'R';
          transfere_carac(caracter);
          caracter = 'e';
          transfere_carac(caracter);
          caracter = 'g';
          transfere_carac(caracter);
          caracter = 'a';
          transfere_carac(caracter);
          instrucao = 201;
          transfere_inst(instrucao);
     
    if(input(pin_C0)== 1){
      delay_ms(100);
      mintZ2 = mintZ2 +1;
      }
        
      if(mintZ2 > 9){
      mintZ2 = 0;
      }

          caracter = mintZ2+48;
          transfere_carac(caracter);
            
     
    ler_tecla();
  
     
  }while(tecla!='C' && tecla != 'C' && tecla != 'C' && tecla != 'C');    // fim do while

   write_eeprom(11,mintZ2); // Vari�vel mintZ1 � guardado no endere�o 11
}//fim da fun��o def_hrega da zona 2



// definir a hora de rega da zona 3
   
 void def_hregaZ3(){
 int dhoras,uhoras, dminutos, uminutos;
 
// definir a hora de rega da zona 2

  do{
    tecla = '#';

    
    if(input(pin_C0)== 1){
      delay_ms(100);
      dhz3= dhz3 +1;
      }
      if(input(pin_C1)== 1){
      delay_ms(100);
      uhz3 = uhz3+1;
      }
      if(input(pin_C2)== 1){
      delay_ms(100);
      dmz3 = dmz3+1;
      }
      if(input(pin_C3)== 1){
      delay_ms(100);
      umz3 =umz3+1;
      }
                                  // controlo da introdu��o das horas
               
      if(dhz3>2)
      dhz3 =0;
      if(uhz3>9)
      uhz3 = 0;
      if(dmz3>5)
      dmz3 =0;
      if(umz3>9)
      umz3 = 0;
      
      // limita��o das 23h
      if(dhz3 == 2){
      if(uhz3>3)
      uhz3 = 0;
      }
      
          instrucao = 128;
          transfere_inst(instrucao);
          caracter = 'Z';
          transfere_carac(caracter);
          caracter = 'o';
          transfere_carac(caracter);
          caracter = 'n';
          transfere_carac(caracter);
          caracter = 'a';
          transfere_carac(caracter);
          caracter = '1';
          transfere_carac(caracter);
      
          instrucao = 136;
          transfere_inst(instrucao);
          dhoras = dhz3;
          caracter = dhoras+48;
          transfere_carac(caracter);
          uhoras = uhz3;
          caracter = uhoras+48;
          transfere_carac(caracter);
          caracter = ':';
          transfere_carac(caracter);
          dminutos= dmz3;
          caracter = dminutos+48;
          transfere_carac(caracter);
          uminutos = umz3;
          caracter = uminutos+48;
          transfere_carac(caracter);
          instrucao = 192;
          transfere_inst(instrucao);
          caracter = 'T';
          transfere_carac(caracter);
          caracter = '_';
          transfere_carac(caracter);
          caracter = 'R';
          transfere_carac(caracter);
          caracter = 'e';
          transfere_carac(caracter);
          caracter = 'g';
          transfere_carac(caracter);
          caracter = 'a';
          transfere_carac(caracter);
          instrucao = 202;
          transfere_inst(instrucao);
          caracter = mintZ3+48;
          transfere_carac(caracter);
        
    
    
   
     
    ler_tecla();
  
     
  }while(tecla!='C' && tecla != 'C' && tecla != 'C' && tecla != 'C');    // fim do while
   
      // As vari�veis s�o guardados nos endere�os
      write_eeprom(12,dhz3); // 12
      write_eeprom(13,uhz3); // 13
      write_eeprom(14,dmz3); // 14
      write_eeprom(15,umz3); // 15
   
   
                                    // Tempo de rega para Zona 3
     do{
          instrucao = 192;
          transfere_inst(instrucao);
          caracter = 'T';
          transfere_carac(caracter);
          caracter = '_';
          transfere_carac(caracter);
          caracter = 'R';
          transfere_carac(caracter);
          caracter = 'e';
          transfere_carac(caracter);
          caracter = 'g';
          transfere_carac(caracter);
          caracter = 'a';
          transfere_carac(caracter);
          instrucao = 202;
          transfere_inst(instrucao);
    
    if(input(pin_C0)== 1){
      delay_ms(100);
      mintZ3 ++;
      }
         
      if(mintZ3 > 9){
      mintZ3 = 0;
      }

          caracter = mintZ3+48;
          transfere_carac(caracter);
            
     
    ler_tecla();
  
     
  }while(tecla!='C' && tecla != 'C' && tecla != 'C' && tecla != 'C');    // fim do while
  
  write_eeprom(16,mintZ3); // Vari�vel mintZ1 � guardado no endere�o 16
}//fim da fun��o def_hrega da zona 3



// ler teclado

 void ler_tecla()
{

  tecla='#'; // inicializa��o de ler tecla

  output_low(pin_b0);
  output_high(pin_b1);
  output_high(pin_b2);
  output_high(pin_b3);

  while((input(pin_b4)==0) && (input(pin_b4)==0) && (input(pin_b4)==0)){
   tecla='1';
   }

  while((input(pin_b5)==0) && (input(pin_b5)==0) && (input(pin_b5)==0)){
   tecla='2';
   }

  while((input(pin_b6)==0) && (input(pin_b6)==0) && (input(pin_b6)==0)){
   tecla='3';

   }

  while((input(pin_b7)==0) && (input(pin_b7)==0) && (input(pin_b7)==0)){
   tecla='F';
   }

  output_high(pin_b0);
  output_low(pin_b1);
  output_high(pin_b2);
  output_high(pin_b3);

  while((input(pin_b4)==0) && (input(pin_b4)==0) && (input(pin_b4)==0)){
   tecla='4';
   }

  while((input(pin_b5)==0) && (input(pin_b5)==0) && (input(pin_b5)==0)){
   tecla='5';
   }

  while((input(pin_b6)==0) && (input(pin_b6)==0) && (input(pin_b6)==0)){
   tecla='6';
    }

  while((input(pin_b7)==0) && (input(pin_b7)==0) && (input(pin_b7)==0)){
   tecla='E';
    }


  output_high(pin_b0);
  output_high(pin_b1);
  output_low(pin_b2);
  output_high(pin_b3);


  while((input(pin_b4)==0) && (input(pin_b4)==0) && (input(pin_b4)==0)){
   tecla='7';
   }

   while((input(pin_b5)==0) && (input(pin_b5)==0) && (input(pin_b5)==0)){
   tecla='8';
   }

   while((input(pin_b6)==0) && (input(pin_b6)==0) && (input(pin_b6)==0)){
   tecla='9';
   }

   while((input(pin_b7)==0) && (input(pin_b7)==0) && (input(pin_b7)==0)){
   tecla='D';
   }


  output_high(pin_b0);
  output_high(pin_b1);
  output_high(pin_b2);
  output_low(pin_b3);

  while((input(pin_b4)==0) && (input(pin_b4)==0) && (input(pin_b4)==0)){
   tecla='A';
   }

  while((input(pin_b5)==0) && (input(pin_b5)==0) && (input(pin_b5)==0)){
   tecla='0';
   }

  while((input(pin_b6)==0) && (input(pin_b6)==0) && (input(pin_b6)==0)){
   tecla='B';
   }

  while((input(pin_b7)==0) && (input(pin_b7)==0) && (input(pin_b7)==0)){
   tecla='C';
   }



 }//fim da fun��o ler_tecla
 
