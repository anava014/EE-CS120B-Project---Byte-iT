#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "\\psf\Dropbox\Others\Andrew&Kenneth\Kenneth\CS120Project\headerFiles\hd44780.c"
#include "\\psf\Dropbox\Others\Andrew&Kenneth\Kenneth\CS120Project\headerFiles\io.c"
#include "\\psf\Dropbox\Others\Andrew&Kenneth\Kenneth\CS120Project\headerFiles\customCharacters.c"

//7-Seg numbers, bits inverted for output
const unsigned char ZERO  = ~0x3F;
const unsigned char ONE   = ~0x06;
const unsigned char TWO   = ~0x5B;
const unsigned char THREE = ~0x4F;
const unsigned char FOUR  = ~0x66;
const unsigned char FIVE  = ~0x6D;
const unsigned char SIX   = ~0x7D;
const unsigned char SEVEN = ~0x07;
const unsigned char EIGHT = ~0x7F;
const unsigned char NINE  = ~0x67;
const unsigned char DASH  = ~0x80;
const unsigned char BLANK = ~0x00;

unsigned short score = 0;

void addScore(char pointsToAdd) {
	for (char i = 0; i < pointsToAdd; ++i) {
		if (score > 99)
		return;
		score = score + 1;
	}
}

void displayScore() {
	char tenthDigit = score / 10;
	char onethDigit = score % 10;
	switch (tenthDigit) {
		case 0:
		PORTB = BLANK;
		break;
		case 1:
		PORTB = ONE;
		break;
		case 2:
		PORTB = TWO;
		break;
		case 3:
		PORTB = THREE;
		break;
		case 4:
		PORTB = FOUR;
		break;
		case 5:
		PORTB = FIVE;
		break;
		case 6:
		PORTB = SIX;
		break;
		case 7:
		PORTB = SEVEN;
		break;
		case 8:
		PORTB = EIGHT;
		break;
		case 9:
		PORTB = NINE;
		break;
	}
	switch (onethDigit) {
		case 0:
		PORTA = ZERO;
		break;
		case 1:
		PORTA = ONE;
		break;
		case 2:
		PORTA = TWO;
		break;
		case 3:
		PORTA = THREE;
		break;
		case 4:
		PORTA = FOUR;
		break;
		case 5:
		PORTA = FIVE;
		break;
		case 6:
		PORTA = SIX;
		break;
		case 7:
		PORTA = SEVEN;
		break;
		case 8:
		PORTA = EIGHT;
		break;
		case 9:
		PORTA = NINE;
		break;
	}
}

void createCustomChar(unsigned char custom[], uint8_t pos){
	for(unsigned char x = 0; x < 8; ++x) {
		lcd_putc(custom[x]);
	}
}

void sendAck() {
	PORTD = PORTD | 0x10;
	_delay_ms(50);
	PORTD = 0x00;
	_delay_ms(50);
}

void displayGameRules() {
	char rules[300] = {"              Use the NES controller's arrow to move around the player. You must avoid all incoming obstacles. You can collect fruits that will randomly show up on the map and gain power-up. Pressing A will consume your power-up completely and make you temporarily immune to obstacles. =<Push Start>="};

	lcd_clrscr();
	lcd_command(0x40);
	createCustomChar(first,0);
	createCustomChar(second,1);
	createCustomChar(third,2);
	createCustomChar(fourth,3);
	createCustomChar(fifth,4);
	createCustomChar(sixth,5);
	
	lcd_goto(0);
	lcd_putc(0);
	lcd_goto(1);
	lcd_putc(1);
	lcd_goto(2);
	lcd_putc(2);

	lcd_puts(3, "Game Rules");
	
	lcd_goto(13);
	lcd_putc(3);
	lcd_goto(14);
	lcd_putc(4);
	lcd_goto(15);
	lcd_putc(5);
	
	char temp[15] = {""};
	
	for (unsigned long long i = 0; i < 300-14; ++i) {
		for (unsigned short j = 0; j < 16; ++j) {
			temp[j] = rules[i+j];
		}
		temp[16] = '\0';
		
		lcd_puts(17, temp);
		
		//checking to start
		if ( (PIND & 0x0F) == 0x0F) {
			sendAck();
			return;
		}
		_delay_ms(200);
	}
	
	//wait for start
	while (1) {
		if ( (PIND & 0x0F) == 0x0F) {
			sendAck();
			return;
		}
	}
}

void displayInvincible () {
	
	lcd_goto(0);
	lcd_command(0x40);
	
	createCustomChar(layerOne,0);
	createCustomChar(layerTwo,1);
	createCustomChar(layerThree,2);
	createCustomChar(layerFour,3);
	createCustomChar(layerFive,4);
	createCustomChar(layerSix,5);
	createCustomChar(layerSeven,6);
	createCustomChar(layerEight,7);
	
	for(short x = 0; x < 8; ++x){
		lcd_goto(x);
		lcd_putc(x);
	}
	
	for(short x = 8; x < 16; ++x){
		lcd_goto(x);
		lcd_putc(15-x);
	}
	
	
	lcd_puts(17, "   INVINCIBLE");
}

void displayNotInvincible () {
	lcd_clrscr();
	lcd_puts(1, "  Good luck!");
}

int main(void)
{
	DDRA = 0xFF; PORTA = 0x00; //7-seg oneth digit
	DDRB = 0xFF; PORTB = 0x00; //7-seg tenth digit
	DDRC = 0xFF; PORTC = 0x00; //LCD
	DDRD = 0xF0; PORTD = 0x0F; //Master

BEGIN:
	score = 0;
	lcd_init();
	lcd_clrscr();
	lcd_command(0x40);
	
	
	createCustomChar(layerOne,0);
	createCustomChar(layerThree,1);
	createCustomChar(layerFive,2);
	createCustomChar(layerSeven,3);
	
	lcd_goto(0);
	lcd_putc(0);
	lcd_goto(1);
	lcd_putc(1);
	lcd_goto(2);
	lcd_putc(2);
	lcd_goto(3);
	lcd_putc(3);
	lcd_puts(4, "Byte-iT");
	lcd_goto(11);
	lcd_putc(3);
	lcd_goto(12);
	lcd_putc(2);
	lcd_goto(13);
	lcd_putc(1);
	lcd_goto(14);
	lcd_putc(0);
	lcd_puts(17, "Andrew & Kenneth");
	_delay_ms(1500);
	
	displayGameRules();
	
	lcd_clrscr();
	lcd_puts(1, "  Good luck!");
	
	displayScore();
	
	while(1) {
		if ((PIND & 0x0F) == 0x01) { //PIND0 is 0000 0001
			sendAck();
			addScore(1);
			displayScore();
		}
		else if ((PIND & 0x0F) == 0x02) { //PIND1 is 0000 0010
			sendAck();
			addScore(3);
			displayScore();
		}
		else if ((PIND & 0x0F) == 0x03) {
			sendAck();
			lcd_puts(1, "  Game Over...");
			_delay_ms(1500);
			goto BEGIN;
		}
		else if ((PIND & 0x0F) == 0x04) {
			sendAck();
			displayInvincible();
		}
		else if ((PIND & 0x0F) == 0x05) {
			sendAck();
			displayNotInvincible();
		}
		else if ((PIND & 0x0F) == 0x0B) {
			sendAck();
			goto BEGIN;
		}
	}
}
