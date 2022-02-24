/*
 * empty.c
 *
 * Created: 11/14/2020 1:12:50 AM
 * Author : mohamed
 */ 
#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>

uint16_t analogRead(uint8_t Pin_num){//take variable of ADC reading
	
	ADCSRA|=(1<<ADEN); //Enable ADC
	ADCSRA|=(1<<ADPS0)|(1<<ADPS1);//Prescaler 8 divider if freq more than 1MHz you need to change it
	ADMUX |=(1<<ADLAR); //uncomment for 8 bit resolution at this case it is 10bit (1024)
	ADMUX =(Pin_num<<MUX0);//choose pin to read
	ADCSRA|=(1<<ADSC); //start the conversion
	while(! (ADCSRA & (1<<ADIF)));//wait to finish
	//uint16_t value = ADC;
	return ADC;
	ADCSRA |=(1<<ADIF);//over write the flag write 1
}
void USART_Init(unsigned int ubrr)
{
	/****set baud rate*****/
	UBRRH = (unsigned char)(ubrr>>8);
	UBRRL = (unsigned char) ubrr ;
	/****enable receiver and transmitter******/
	UCSRB = (1<<RXEN)|(1<<TXEN);
	/**set frame format :8data,2stop bit*/
	UCSRC  = (1<<URSEL)|/*|(1<<USBS)*/(1<<UCSZ1)|(1<<UCSZ0);
}
void USART_Transmit(char data)
{
	/*wait for empty buffer*/
	//UCSRA is the main register
	//UDRE be 1 after finishing sending
	while (!(UCSRA & (1<<UDRE)));
	/*put data into buffer*/
	UDR = data ;
}
unsigned char USART_Read(void){
	//RXC be 1 when be ready to recieve
	while (!(UCSRA & (1<<RXC)));
	//get data from the buffer
	return UDR;
}
void print_adc(uint16_t value){//take pin number you want to read as 0,1,2,3,4,5,6,7
	uint8_t one_pos = 0; /*ones position*/ uint8_t ten_pos = 0; /*tens position*/
	uint8_t hund_pos = 0; /*hundreds position*/uint8_t thous_pos = 0; /*thousands posi*/
	//***************ADC value to decimal******//
	one_pos   = value % 10; value /= 10;ten_pos   = value % 10;
	value /= 10 ;hund_pos  = value % 10;thous_pos = value / 10;
	//ASCII conversion add 0x30 to each number to get acsii equivalent
	one_pos  +=0x30; ten_pos   +=0x30;
	hund_pos +=0x30; thous_pos +=0x30;
	//sending the 4 digits over usart
	USART_Transmit(thous_pos); USART_Transmit(hund_pos);
	USART_Transmit(ten_pos);   USART_Transmit(one_pos);
	USART_Transmit('\n');
}
int main(void)
{
    /* Replace with your application code */
    while (1) 
    {
		int v =analogRead(1);
		print_adc(v);
		_delay_ms(1000);
    }
}

