#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>		
#include <avr/interrupt.h>
#include "def_principais.h"	
#include "LCD.h"
#include "LCD_4bits_messagens.h"
#include "USART.h"
///////
/* LTC1298 Modo de operacao do conversor AD------------------- */
#define CH0 0xC /* Ler Chanel 0 */
#define CH1 0xE /* Ler Chanel 1 */
#define DF0 0x8 /* Ler (Chanel 0 - Chanel 1) */ 
#define DF1 0xA /* Ler (Chanel 1 - Chanel 0) */ 
#define LSB 0 /* Setar ordem LSB primeiro */
#define MSB 1 /* SSetar ordem MSB primeiro */
/* Definicao dos pinos SPI - pinos da placa Arduino ------------------- */
#define DD_SS (1<<DDB2) //pin 10: SS 
#define DD_MOSI (1<<DDB3) //pin 11: MOSI 
#define DD_MISO (1<<DDB4) //pin 12: MISO 
#define DD_SCK (1<<DDB5) //pin 13: SCK
#define habilita_SS() clr_bit(PORTB,PB2) // SS habilita em zero 
#define desabilita__SS() set_bit(PORTB,PB2)
#define amostras 100 // define numero de amostras para media da leitura

    // Funções das Interrupções
    ISR(INT0_vect);
	ISR(TIMER2_OVF_vect);

    // Variaveis
    unsigned int rpm = 0, hz=0;
    unsigned char  estado=0 ,c=0, tm2=0;
	
	
void inic_SPI(); // funcao de inicializacao da comunicacao SPI
unsigned char SPI(unsigned char dado); // funcao de leitura e escrita de dado via SPI
int cont = 0; float flag = 0;
float flag1 = 0;float peso = 0;

unsigned int peso1 = 0; unsigned char digitos[4];
void spi();
void com_usart(); void escreve();
unsigned char tempH,tempL; // variaveis de 8 bits
unsigned int valor16bits; // variavel de 16 bits
unsigned char TEMP[tam_vetor]; // vetor para separacao de caracteres int tempo = 0;
int temp = 0;
	


    //Pinos
	#define BTI PD0
	
	
	
//[Programa]--------------------------------------------------------------------------------------
int main()
{
	 unsigned char digitos[tam_vetor];	//declaração da variável para armazenagem dos digitos
	DDRD = 0b11000000;			// Pinos de entrada e saída
	PORTD= 0b00011111;			//habilita o pull-up		 
	DDRC = 0b00111111;			//display
	PORTC= 0b00000000;			//desliga o display
		
	float Vx;  // valor AD do peso a ser calculado
	unsigned int Pxint;
	
    float P2=255;  // valor ref kg max de calibracao
	float P1=0;   // valor ref kg min de calibracao
	float Px;  // valor kg a ser calculado
	float V2=4095; // valor digital AD max de calibracao
	float V1=2;  // valor digital AD min de calibracao
	
	float conversor;
	USART_Inic(MYUBRR);
    inic_SPI();
	    
	inic_LCD_4bits();			//inicializa o LCD
	cmd_LCD(0x01,0); 			//desloca cursor para a segunda linha
	
	//Interrupção INT0 e INT1
    EICRA= 0b00001010;
    EIMSK= 0b00000011;
    
    //Timer 0-PWM
    TCCR0A = 0b10000011;
    TCCR0B = 0b00000000;
	OCR0A=0;
	
   
    //Timer 1-Contagem de Pulsos
    TCCR1A= 0b00000000;
   	TCCR1B= 0b10000111;
	TIMSK1= 0b00000000;
	 
	 
	 //Timer 2- Contagem de tempo ~10ms
    TCCR2A= 0b00000010;
    TCCR2B= 0b00000111;
    TIMSK2= 0b00000001;
	
while(1)
{	spi();
	          Vx=0;
			  Pxint=0;
	          Vx=valor16bits; 
	          Px=P2-((V2-Vx)*(P2-P1)/(V2-V1));
	          Pxint=Px*1000;   
			  
         switch(estado){
	      case 0:{cmd_LCD(0x84,0);	
		     escreve_LCD_Flash(msg_bem);
		     cmd_LCD(0xC5,0);	
		     escreve_LCD_Flash(msg_vindo);
		     _delay_ms(2000);
			 estado=1;
			 break;}	
			   
           case 1:
		   {  cmd_LCD(0x80,0); 
              escreve_LCD_Flash(msg_limp);
              cmd_LCD(0x80,0); 
		      escreve_LCD_Flash(msg_pronto);
		      cmd_LCD(0xc0,0); 
		      escreve_LCD_Flash(msg_limp);
		      if(!tst_bit(PIND,BTI))
			   {estado=2;
			   sei();
			   cmd_LCD(0x80,0); 
		       escreve_LCD_Flash(msg_limp);
			   cmd_LCD(0xc0,0); 
		       escreve_LCD_Flash(msg_limp);
			   TCCR0A = 0b10000011;  
		       TCCR0B = 0b00000011;}
		       break;
		   }			   
		   
      case 2:{OCR0A=Px;
		       cmd_LCD(0x80,0);escreve_LCD_Flash(msg_vlo);
		                         ident_num(rpm,digitos);
			                     cmd_LCD(0x87,0);
								 cmd_LCD(digitos[4],1);
			                     cmd_LCD(digitos[3],1);							 
								 cmd_LCD(digitos[2],1);
			                     cmd_LCD(digitos[1],1);
								 cmd_LCD(digitos[0],1);
								 cmd_LCD(' ',1);
			                     cmd_LCD('R',1);
								 cmd_LCD('P',1);
			                     cmd_LCD('M',1);
			  cmd_LCD(0xc0,0);escreve_LCD_Flash(msg_freq);
		                         ident_num(hz,digitos);
			                     cmd_LCD(0xC7,0);
								 cmd_LCD(digitos[4],1);
			                     cmd_LCD(digitos[3],1);							 
								 cmd_LCD(digitos[2],1);
			                     cmd_LCD(digitos[1],1);
								 cmd_LCD(digitos[0],1);
								 cmd_LCD(' ',1);
			                     cmd_LCD('H',1);
								 cmd_LCD('z',1);
			                     break;} 
			case 3:
			{TCCR0A = 0b00000000;
             TCCR0B = 0b00000000;
             OCR0A=0;
			 TCNT1=0;
			 estado=1;}	  
	  
  }
} 
	
}

	  
//======================================================================================	

ISR(INT0_vect){
TIMSK2= 0b00000000;
estado=3;}

ISR(TIMER2_OVF_vect){
 if(tm2<50)
	{tm2++;}    
 else
    {rpm=(TCNT1/24)*120;
	  hz=(24*((3.1415*rpm)/30))/60;
	 TCNT1=0;
	 tm2=0;	
     estado=2;}
 TCNT2=100;	 
}

void inic_SPI(){
	    DDRB = DD_MOSI | DD_SCK | DD_SS; //configurac?a?o dos pinos de entrada e sai?da da SPI
        PORTB |= ((1<<DD_SS)|(1<<DD_MOSI) | (1<<DD_SCK)); // estado inicial dos pinos
        SPSR &= ~(1<<SPI2X); // status do registrador duplex em zero
        SPCR &= ~((1<<SPIE) | (1<<CPHA) | (1<<DORD)| (1<<CPOL)) ; // bits com zero
        SPCR |= (1<<SPE) | (1<<MSTR) | (1<<SPR1) | (1<<SPR0); // bits com 1: habilita interrupcao dado ajustado na subida e amostragem na descida do sinal de clock
		}		
   
unsigned char SPI(unsigned char dado){
        SPDR = dado;
        while(!(SPSR & (1<<SPIF))); 
		return SPDR;
        //envia um byte
        //espera envio
        //retorna o byte recebido
        }
	
void spi(){
    _delay_ms(500); // intervalo valor16bits=0; // inicia variavel em zero 0XXXXXXX XXXXX000
    habilita_SS(); 
	SPI(CH0 | MSB);
    tempH=SPI(0x00); 
	tempL=SPI(0x00);
    desabilita__SS();
    // inicio de loop de amostras
    // define canal de leitura AD e sequencia dos bits //envia vazio recebe MSB sequencia: 7 bits //envia vazio recebe LSB sequencia: 5 bits
    valor16bits = (tempH<<5) | ( tempL >>3); // Aglutina MSB+LSB descartando os zeros recebidos e soma-se
    _delay_ms(1); // intervalo entre as amostras   
  }              
