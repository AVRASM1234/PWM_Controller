/*
 * PWM_Controller.c
 *
 * Created: 18.01.2018 22:08:13
 * Author : AVRASM1234
 */ 
#define F_CPU			9600000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#define  BUTTON			(PINB & (1<<PORTB3))

unsigned char __BUT_PrevState = 0;
unsigned char __PWM_Enabled = 0;

// ��������, � ������� ������� ����� ������������ ������� �� �������, � �� ����������,
// ����� ������� �������� ���������-�������...
const unsigned char PerToFill[256] PROGMEM = 
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
	0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 
	0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 
	0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 
	0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x03, 
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 
	0x03, 0x03, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x06, 
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x07, 
	0x07, 0x07, 0x07, 0x07, 0x07, 0x08, 0x08, 0x08, 
	0x08, 0x08, 0x08, 0x09, 0x09, 0x09, 0x09, 0x09, 
	0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0B, 0x0B, 0x0B, 
	0x0B, 0x0C, 0x0C, 0x0C, 0x0C, 0x0D, 0x0D, 0x0D, 
	0x0E, 0x0E, 0x0E, 0x0E, 0x0F, 0x0F, 0x0F, 0x10, 
	0x10, 0x10, 0x11, 0x11, 0x12, 0x12, 0x12, 0x13, 
	0x13, 0x14, 0x14, 0x14, 0x15, 0x15, 0x16, 0x16, 
	0x17, 0x17, 0x18, 0x18, 0x19, 0x19, 0x1A, 0x1B, 
	0x1B, 0x1C, 0x1C, 0x1D, 0x1E, 0x1E, 0x1F, 0x20, 
	0x20, 0x21, 0x22, 0x23, 0x23, 0x24, 0x25, 0x26, 
	0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 
	0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 
	0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3D, 0x3E, 0x3F, 
	0x41, 0x42, 0x44, 0x45, 0x47, 0x48, 0x4A, 0x4C, 
	0x4D, 0x4F, 0x51, 0x52, 0x54, 0x56, 0x58, 0x5A, 
	0x5C, 0x5E, 0x60, 0x62, 0x64, 0x66, 0x69, 0x6B, 
	0x6D, 0x70, 0x72, 0x75, 0x77, 0x7A, 0x7C, 0x7F, 
	0x82, 0x85, 0x88, 0x8B, 0x8E, 0x91, 0x94, 0x97, 
	0x9B, 0x9E, 0xA2, 0xA5, 0xA9, 0xAC, 0xB0, 0xB4, 
	0xB8, 0xBC, 0xC0, 0xC4, 0xC9, 0xCD, 0xD2, 0xD6, 
	0xDB, 0xE0, 0xE5, 0xEA, 0xEF, 0xF4, 0xF9, 0xFF
};

int main(void)
{
    DDRB = 0b00000111;
    PORTB = 0;
	
	// ��������� �������
	TCCR0A = 0b10100001;
	TCCR0B = 0b00000001;
	TCNT0 = 0;
	TIMSK0 = 0;
	
	OCR0A = 0;
	OCR0B = 0;
	
	// ��������� ���
	ADMUX = 0b00100010;
	ADCSRA = 0b10001111;
	
	sei();
    while (1) 
    {
		// ��������� ������� ������ ���������
		if (BUTTON == 0)
		{
			// ���� ������ ������ - ���������� ���
			__BUT_PrevState = 1;
		}
		else
		{
			// ���� ������ ��������...
			if (__BUT_PrevState == 1)
			{
				// ...�� �� ����� ������ - ����������� ��������� ������ ���-����������� � ���������� ��������� ������
				__BUT_PrevState = 0;
				__PWM_Enabled ^= (1<<0);
				PORTB ^= (1<<PORTB2);
			}
		}
		
		// ���� ���������� �������
		if (__PWM_Enabled)
			// ��������� �������������� ���
			ADCSRA |= (1<<ADSC);
		else
			// ����� ���� ����� ����
			OCR0A = 0;
		// ������ �������� � 50 ��
		_delay_ms(50);	
    }
}

ISR(ADC_vect)
{
	// ����� �������� �� �������������� - ����� � ������� ��� ��������������� �������� ������� �������� � ������������ ��� (��� �� ����� ��� �������...)
	OCR0A = pgm_read_byte(&PerToFill[ADCH]);
}
