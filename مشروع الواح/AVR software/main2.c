
#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>
//*******mp3**********

//******************USART***************************//
#define FOSC F_CPU  // Clock Speed same as controller clock speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1
//**************************
void USART_Init(unsigned int ubrr);
void USART_Transmit(char data);
unsigned char USART_Read(void);
//******************ADC**********
uint16_t analogRead(uint8_t Pin_num); //take pin number you want to read as 0,1,2,3,4,5,6,7 of portA
void print_adc(uint16_t temp);//take vriable of ADC reading
void blink(uint8_t pin);
uint16_t analogReadacurecy(uint8_t pin_num);
// 	*************ADC**************//
//volatile uint16_t  Control_Pin; //connected to AD2 control volume play,pause,loops is AD2
//volatile uint16_t  Track_Pin ; // connected to AD1
volatile uint16_t  Board_Pin ;//connected to AD0
bool readName =1;
//  bool welcome = 1;
//************MP3 PLAYER*************
#define  cmd   3
#define  par1  5
#define  par2  6
#define  checkSumM  7
#define  checkSumL  8
//start byte,version,Length,CMD,Feedback,Para_MSB,Para_LSB,checksum_MSByte,checksum_MSByte,End byte
char stack[10]={0x7E,0xFF,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0xEF};
void sendStack(uint8_t command ,uint8_t parameter1,uint8_t parameter2);
void playTrack(uint8_t folder,uint8_t track);
void enableTrackButtons(uint8_t folder);
void enalbeConroleButtonsWithoutLoopAll(void);
//control functions
void volume(char selector);
void vol_up (void);
void vol_down (void);
void play (void);
void pause (void);
void loopCurrent_on(void);
void loopCurrent_off(void);
void loopAll(void);
void stop(void);
void reset(void);
void sleep(void);



 
int main(void)
{
	 uint8_t flag = 0;
//******************UART*************************//
	USART_Init(MYUBRR);

	//*********LED*********
	DDRB|=(1<<PB1);//output
	DDRB|=(1<<PB2);//output
	PORTB&=~(1<<PB1);  //LOW connected to RED LED pin and GREEN connected directly to ground (state:RED)
	PORTB|=(1<<PB2);	//high connected to green pin
	_delay_ms(1000);
	//welcome meessage
	playTrack(0x01,0x02);
	while (1){
		readName =1;
		flag=0;
 	PORTB&=~(1<<PB1);  //LOW connected to RED LED pin and GREEN connected directly to ground (state:RED)
 	PORTB|=(1<<PB2);	//high connected to green pin
//******************test*********************
// int a =	analogReadacurecy(0);
// int b =	analogReadacurecy(1);
// int c =	analogReadacurecy(2);
// if (a || b || c){
// 	print_adc(a);print_adc(b);print_adc(c);
// }


//play once after initialization 
// if (welcome)
//  {
// 	_delay_ms(1000);//wait for mp3 initialization
// 	welcome=0;
// 	playTrack(0x01,0x02);
//  }
Board_Pin = analogReadacurecy(0);

//***check connected board
if((Board_Pin<=255)&&(Board_Pin>=235))//Al_Masad R=3.3K
 {   
   flag = 1;
 }
else if ((Board_Pin<=335)&&(Board_Pin>=310))//?Al_Ikhlas R=2.2k
   {
     flag = 2;
   }
while (flag == 1){
		PORTB&=~(1<<PB2);PORTB|=(1<<PB1);//(state:green)
		enableTrackButtons(0x05);//Al_Masad R=3.3K
		enalbeConroleButtonsWithoutLoopAll();
		if (analogReadacurecy(0)==0)
		{
			flag=0;
		}
	}

while (flag == 2){
			PORTB&=~(1<<PB2);PORTB|=(1<<PB1);//(state:green)
			enableTrackButtons(0x04);//Al_Ikhlas R=2.2k
			enalbeConroleButtonsWithoutLoopAll();
			if (analogReadacurecy(0)==0)
			{
				flag=0;
			}
}
 
	}
	return 0 ;
}


//*************/UBRR/UCSR/UDR/*************//
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
// unsigned char USART_Read(void){
// 	//RXC be 1 when be ready to recieve
// 	while (!(UCSRA & (1<<RXC)));
// 	//get data from the buffer
// 	return UDR;
// }
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
//*********************ADC********************
//blocking
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
uint16_t analogReadacurecy(uint8_t pin_num){ 
	//return 0 if read is less than 180
	//read values 3 times and return it if equaled
	uint16_t v1=analogRead(pin_num);
	uint16_t v2=analogRead(pin_num);
	uint16_t v3=analogRead(pin_num);
	if ((v1>160)&&(v1=v2=v3))
	{
		return v1;
	}
	return 0;
}
void blink (uint8_t pin){//write pin from PORTB
	//toggle pin state every second
	PORTB^=(1<<pin);_delay_ms(1000);
	PORTB^=(1<<pin);_delay_ms(1000);
	PORTB^=(1<<pin);_delay_ms(1000);
	PORTB^=(1<<pin);_delay_ms(1000);
	PORTB^=(1<<pin);_delay_ms(1000);
	PORTB^=(1<<pin);
}
//**************MP3***********
void sendStack(uint8_t command ,uint8_t parameter1,uint8_t parameter2){
	uint16_t checkSum = (0xFFFF-(0x105+command+parameter1+parameter2)+1);
	uint8_t sumL = (uint8_t)checkSum;//most sig..
	uint8_t sumM = (checkSum>>8);//least siginificant
	stack[cmd]=command;
	stack[par1]=parameter1;
	stack[par2]=parameter2;
	stack[checkSumM]=sumM;
	stack[checkSumL]=sumL;
	for(int i =0;i<10;i++)
	{
		USART_Transmit(stack[i]);
	}
	//unsigned int n = USART_Read();
}
void playTrack(uint8_t folder,uint8_t track){
	sendStack(0x0F,folder,track);
}
void play(){
	sendStack(0x0D,0x00,0x00);
}
void pause()
{
	sendStack(0x0E,0x00,0x00);
}
void set_volume(uint8_t vol){
	sendStack(0x06,0x00,vol);
}
void volume (char selector){
	static unsigned char vol =0x1C;
	if (selector=='u')
	{
		vol+=1;
		if(vol>0x1E) vol = 0x1E;
	}
	else if (selector =='d')
	{
		sendStack(0x06,0x00,vol);
		vol-=1;
		if(vol<0x01) vol = 0x01;
	}
	_delay_ms(100);
}
void vol_up(){
	sendStack(0x04,0x00,0x00);
}
void vol_down(){
	sendStack(0x05,0x00,0x00);
}
void loopCurrent_on(void){
	sendStack(0x19,0x00,0x00);
}
void loopCurrent_off(void){
	sendStack(0x19,0x00,0x01);
}
void loopAll_on(void){
	sendStack(0x11,0x00,0x01);
}
void loopAll_off(void){
	sendStack(0x11,0x00,0x00);
}
void loopCurrent(void){
	static bool is_looping=0 ;
	if(is_looping){
		loopCurrent_off();
		is_looping = 0;
	}
	else {
		loopCurrent_on();
		is_looping = 1;
	}
	
}
void loopAll(void){
	static bool is_looping=0 ;
	if(is_looping){
		loopAll_off();
		is_looping = 0;
	}
	else {
		loopAll_on();
		is_looping = 1;
	}
	_delay_ms(100);
}
void stop(void){
// 	cmd=0x16;par1=0x00;par2=0x00;
	sendStack(0x16,0x00,0x00);
}
void sleep(void){
	sendStack(0x0A,0x00,0x00);	
}
void reset(void)
{
	sendStack(0x0C,0x00,0x00);
}
void enableTrackButtons (uint8_t folder){//write number of folder in SD card related to current Board????
	 uint16_t  Track_Pin = analogReadacurecy(1); 	// connected to AD1 pin in ATmega8A

	//first 6 conditions for first 6 tracks in SD card and for full AYA???
	
	if ((Track_Pin<=920)&&(Track_Pin>=882))//El_Basmalla
	{//in the first folder first track
		playTrack(0x01,0x01);
	} 
	else if (Track_Pin==1023)// All of Surah.
	{
		playTrack(folder,0x01);
	}
	//_______AYAT___________
	else if ((Track_Pin<=230)&&(Track_Pin>=215))//1st AYA in track no. 002
	{
		
		playTrack(folder,0x02);
	}
	
	else if ((Track_Pin<=790)&&(Track_Pin>=755))//2st AYA
	{
		playTrack(folder,0x03);
	}
	
	else if ((Track_Pin<=715)&&(Track_Pin>=680))// 3th AYA
	{
		playTrack(folder,0x04);
	}
	
	else if ((Track_Pin<=655)&&(Track_Pin>=620))// 4th AYA
	{
		playTrack(folder,0x05);
	}
	
	else if ((Track_Pin<=572)&&(Track_Pin>=540)) // 5th AYA
	{
		playTrack(folder,0x06);
	}
	//_____________cut AYAT______________
	else if ((Track_Pin<=525)&&(Track_Pin>=490))// 1st AYA in folder no.007
	{
		playTrack(folder,0x07);
	}
	
	else if ((Track_Pin<=471)&&(Track_Pin>=430))// 2
	{
		playTrack(folder,0x08);
	}
	
	else if ((Track_Pin<=335)&&(Track_Pin>=305))// 3
	{
		playTrack(folder,0x09);
	}
	
	else if ((Track_Pin<=260)&&(Track_Pin>=231))// 4
	{
		playTrack(folder,0x0A);
	}
	
	else if ((Track_Pin<=199)&&(Track_Pin>=180))// 5
	{
		playTrack(folder,0x0B);
	}
	else if (readName) //name of Surah in folder 012
	{
	playTrack(folder,0x0C);
	readName=0;
	}

// 	else {//because if board installed stop any repeated tracks
// 		stop();
// 		loopCurrent_off(); //stop repeating
// 		loopAll_off();
// 		}
}
void enalbeConroleButtonsWithoutLoopAll(void)
{ 
	volatile uint16_t  Control_Pin = analogReadacurecy(2); //connected to AD2 control volume play,pause,loops is AD2

	if ((Control_Pin<=795)&&(Control_Pin>=760))// ??? ???
	{
		volume('d');
		//vol_down();
	}
	else if ((Control_Pin<=720)&&(Control_Pin>=680))
	{
		volume('u');
		//vol_up();
	}
	else if ((Control_Pin<=585)&&(Control_Pin>=540))
	{
		loopCurrent_off();
		pause();
		
	}
		else if ((Control_Pin<=353)&&(Control_Pin>=305))
		{
			
			play();
			
		}
	else if ((Control_Pin<=290)&&(Control_Pin>=240))
	{
		loopCurrent_on();
	}
	
	
}
// void enalbeConroleButtonsLoopALL()
// {volatile uint16_t  Control_Pin = analogReadacurecy(2); //connected to AD2 control volume play,pause,loops is AD2
// 	enalbeConroleButtonsWithoutLoopAll();
// 	if ((Control_Pin<=)&&(Control_Pin>=))
// loopAll();
// }