#include <avr/io.h>
#include <util/delay.h>

#ifndef F_CPU
#define F_CPU 16000000UL           /* Clock Frequency = 16Mhz */
#endif

#define SPEED 9600

#define X_STEP (1 << 0)
#define X_DIR  (1 << 1)

#define Y_STEP (1 << 2)
#define Y_DIR  (1 << 5)

#define Z_STEP (1 << 6)
#define Z_DIR  (1 << 7)

int modeX = 0;
int stepX = 0;

int modeY = 0;
int stepY = 0;

int modeZ = 0;
int stepZ = 0;

#define A1(N)	do { PORTC &= ~(_BV(PC5)); PORTC |= (N << PC5); } while(0)
#define B1(N)	do { PORTC &= ~(_BV(PC4)); PORTC |= (N << PC4); } while(0)
#define C1(N)	do { PORTC &= ~(_BV(PC3)); PORTC |= (N << PC3); } while(0)
#define D1(N)	do { PORTC &= ~(_BV(PC2)); PORTC |= (N << PC2); } while(0)

#define A2(N)	do { PORTC &= ~(_BV(PC1)); PORTC |= (N << PC1); } while(0)
#define B2(N)	do { PORTC &= ~(_BV(PC0)); PORTC |= (N << PC0); } while(0)
#define C2(N)	do { PORTB &= ~(_BV(PB4)); PORTB |= (N << PB4); } while(0)
#define D2(N)	do { PORTB &= ~(_BV(PB2)); PORTB |= (N << PB2); } while(0)

#define A3(N)	do { PORTB &= ~(_BV(PB1)); PORTB |= (N << PB1); } while(0)
#define B3(N)	do { PORTD &= ~(_BV(PD3)); PORTD |= (N << PD3); } while(0)
#define C3(N)	do { PORTD &= ~(_BV(PD4)); PORTD |= (N << PD4); } while(0)
#define D3(N)	do { PORTB &= ~(_BV(PB0)); PORTB |= (N << PB0); } while(0)


void changeStepX(int dir){
	if(dir)
		stepX = (++stepX) % 4;
	else
		stepX = (--stepX) % 4;
	
	if(stepX==0){
		A1(1);
		B1(1);
		C1(0);
		D1(0);
	}else if(stepX==1){
		A1(0);
		B1(1);
		C1(1);
		D1(0);
	}else if(stepX==2){
		A1(0);
		B1(0);
		C1(1);
		D1(1);
	}else if(stepX==3){
		A1(1);
		B1(0);
		C1(0);
		D1(1);
	}
}

void changeStepY(int dir){
	if(dir)
		stepY = (++stepY) % 4;
	else
		stepY = (--stepY) % 4;
	
	if(stepY==0){
		A2(1);
		B2(1);
		C2(0);
		D2(0);
	}else if(stepY==1){
		A2(0);
		B2(1);
		C2(1);
		D2(0);
	}else if(stepY==2){
		A2(0);
		B2(0);
		C2(1);
		D2(1);
	}else if(stepY==3){
		A2(1);
		B2(0);
		C2(0);
		D2(1);
	}
}

void changeStepZ(int dir){
	if(dir)
		stepZ = (++stepZ) % 4;
	else
		stepZ = (--stepZ) % 4;
	
	if(stepZ==0){
		A3(1);
		B3(1);
		C3(0);
		D3(0);
	}else if(stepZ==1){
		A3(0);
		B3(1);
		C3(1);
		D3(0);
	}else if(stepZ==2){
		A3(0);
		B3(0);
		C3(1);
		D3(1);
	}else if(stepZ==3){
		A3(1);
		B3(0);
		C3(0);
		D3(1);
	}
}

static void serial_init(unsigned int bittimer)
{
	/* Set the baud rate */
	UBRR0H = (unsigned char) (bittimer >> 8);
	UBRR0L = (unsigned char) bittimer;
	/* set the framing to 8N1 */
	UCSR0C = (3 << UCSZ00);
	/* Engage! */
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	return;
}

static void serial_write(unsigned char c)
{
	while ( !(UCSR0A & (1 << UDRE0)) )
		;
	UDR0 = c;
}

int main (void)
{

	int i = 0;
	char *str
          = "XS = X XD = X YS = Y YD = Y ZS = Z ZD = Z\r\n";

	/* set PORTB for output*/
	DDRB = 0xFF;

	/* set PORTC for output*/
	DDRC = 0xFF;

	/* set PORTD[0,1,2,5,6,7] for input and PORTD[3,4] output*/
	DDRD = 0x18;

        /* set pull-up for all input pins */
	PORTD = ~(0x18);

	/* let the preprocessor calculate this */
	serial_init( ( F_CPU / SPEED / 16 ) - 1);

	if(PIND & X_STEP){
		modeX = 1;
	}else{
		modeX = 0;
	}
	
	if(PIND & Y_STEP){
		modeY = 1;
	}else{
		modeY = 0;
	}	
	
	if(PIND & Z_STEP){
		modeZ = 1;
	}else{
		modeZ = 0;
	}	

	modeZ = 0;
	
	while(1){
		if(modeX == 0){
			if(PIND & X_STEP){
				modeX = 1;
				if(PIND & X_DIR){
					changeStepX(1);
				}else{
					changeStepX(0);
				}
			}
					
		}else{
			if(!(PIND & X_STEP)){
				modeX = 0;
			}
		}
	
		if(modeY == 0){
			if(PIND & Y_STEP){
				modeY = 1;
				if(PIND & Y_DIR){
					changeStepY(1);
				}else{
					changeStepY(0);
				}
			}
					
		}else{
			if(!(PIND & Y_STEP)){
				modeY = 0;
			}
		}		
		
		if(modeZ == 0){
			if(PIND & Z_STEP){
				modeZ = 1;
				if(PIND & Z_DIR){
					changeStepZ(1);
				}else{
					changeStepZ(0);
				}
			}
					
		}else{
			if(!(PIND & Z_STEP)){
				modeZ = 0;
			}
		}		
		
	}

	/* while (1) {

		if (PIND & X_STEP)
			str[5]='1';
		else
			str[5]='0';

		if (PIND & X_DIR)
			str[12]='1';
		else
			str[12]='0';


		if (PIND & Y_STEP)
			str[19]='1';
		else
			str[19]='0';

		if (PIND & Y_DIR)
			str[26]='1';
		else
			str[26]='0';


		if (PIND & Z_STEP)
			str[33]='1';
		else
			str[33]='0';

		if (PIND & Z_DIR)
			str[40]='1';
		else
			str[40]='0';


		//USART_putstring(str);
		serial_write(str[i++]);
		if (str[i] == '\0') {
			i = 0;
			_delay_ms(500);
		}

	}*/
	return 0;
}

