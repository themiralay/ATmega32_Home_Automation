/*
 * myHomeAutomation.c
 *
 * Created: 30.05.2019 23:58:02
 * Author : CryptoTyler
 */ 
#include <util/delay.h>
#include <string.h>
#include <avr/sfr_defs.h>
#include <stdio.h>
#include <avr/io.h>
#include "ser_il328.h"
#define POT 500000
void ADC_Load()
{
	DDRC = 0x00;	        /* Make ADC port as input */
	//ADCSRA = 0x87;          /* Enable ADC, with freq/128  */
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADEN); 
}
int ADC_Read_ch(char channel)
{
	switch(channel)
	{
		 case 0:
			 ADMUX=0x40;
			 break;
		 case 1:
			 ADMUX=0x41;
			 break;
		 case 2:
			 ADMUX=0x42;
			 break;
		 case 3:
			 ADMUX=0x43;
			 break;
		 default:
			 ADMUX=0x44;
			 break;
	}
	float myValue=0;
	for(int i=0;i<1000;i++)
	{
		ADCSRA |= (1<<ADSC);               /* Start ADC conversion */
		while (!(ADCSRA & (1<<ADIF)));     /* Wait until end of conversion by polling ADC interrupt flag */
		ADCSRA |= (1<<ADIF);               /* Clear interrupt flag */
		_delay_us(100);                      /* Wait a little bit */
		myValue=myValue+ADCW;                       /* Return ADC word */
	}
	myValue=myValue/1000;
	return myValue;
}

int main(void)
{
	seri_yukle(16,9600);
	ADC_Load();
	DDRD=0xFF;
	DDRB=0xFC;
	int a,basla=0,tempperde=0;
	float degree=0,potval1=0,potval2=0,light=0,tiktok=0;
	char rec;
    while (1) 
    {
		if(basla==1)
		{
			tempperde=tempperde+1;
				if(tiktok<10)
				{
					if(tempperde!=2)
						continue;
					else
					{
						tempperde=0;
						if (bit_is_clear(PIND,4))
						{
							tiktok++;
							PORTD |=(1<<PIND4);
						}
						else
							PORTD &= ~(1<<PIND4);
					}
				}
				else
				{
					basla=0;
					tiktok=0;
				}
		}
		rec=al(a);
		if(rec!=1)
		{
			if(rec=='d')
			{
				if (bit_is_clear(PINB,2))
					PORTB |=(1<<PINB2);
				else
					PORTB &= ~(1<<PINB2);
			}
			if(rec=='e')
			{
				if (bit_is_clear(PINB,3))
					PORTB |=(1<<PINB3);
				else
					PORTB &= ~(1<<PINB3);
			}
			if(rec=='f')
			{
				if (bit_is_clear(PINB,4))
					PORTB |=(1<<PINB4);
				else
					PORTB &= ~(1<<PINB4);
			}
			if(rec=='q')
			{
				if(basla==0)
					basla=1;
				else
					basla=0;
			}
		}
		degree=((ADC_Read_ch(0))*3.68)/10;
		DegreeState(degree);
		convert2string(degree);
		gonder('d');
		//_delay_ms(600);
		potval1=(double)POT/1024*ADC_Read_ch(1);
		convert2string(potval1);
		gonder('p');
		if(potval1>300000 && degree>60)
		{
			PORTB |=(1<<PINB5);
			gonder('y');
		}
		else
			PORTB &= ~(1<<PINB5);	
		if(potval1>400000)
		{
			gonder('w');
		}
		else
		PORTB &= ~(1<<PINB5);
		//_delay_ms(600);
		potval2=(double)POT/1024*ADC_Read_ch(2);
		if(potval2>300000)
		{
			PORTB |=(1<<PINB5);
			gonder('a');
		}
		else
			PORTB &= ~(1<<PINB5);
		convert2string(potval2);
		gonder('p');
		gonder('l');
		//_delay_ms(600);
		light=ADC_Read_ch(3);
		convert2string(light);
		//_delay_ms(600);
		//_delay_ms(50);
		//celsius = (ADC_Read(0)*4.88);
		//celsius = (celsius/10.00);
		//_delay_ms(800);
    }
}
int temp1,temp2,temp3;
void DegreeState(float myState)
{
	if (myState<30)
	{
		temp1=1;
		PORTD |=(1<<PIND5);
		gonder('g');//good
	}
	else if(temp1==1)
	{
		temp1=0;
		PORTD &= ~(1<<PIND5);
	}
	if ((myState>30) && (myState<40))
	{
		temp2=1;
		PORTD |=(1<<PIND5);
		PORTD |=(1<<PIND6);
		gonder('s');
		gonder('h');//hot
	}
	else if(temp2==1)
	{
		temp2=0;
		PORTD &= ~(1<<PIND5);
		PORTD &= ~(1<<PIND6);
	}
	if((myState>40) && (myState<60))
	{
		temp3=1;
		PORTD |=(1<<PIND5);
		PORTD |=(1<<PIND6);
		PORTD |=(1<<PIND7);
		gonder('v');
		gonder('h'); //veryhot
	}
	else if(temp3==1)
	{
		temp3=0;
		PORTD &= ~(1<<PIND5);
		PORTD &= ~(1<<PIND6);
		PORTD &= ~(1<<PIND7);
	}
	if(60<myState)
	{
		gonder('f');
		//if(myDegree<=25)
	}
	return;
}
void convert2string(float convert)
{
	char buffer[12]={'0','0','0','0','0','0','0','0','0','0','0','0'};
	char t[12]={'0','0','0','0','0','0','0','0','0','0','0','0'};
	dtostrf(convert, 12, 2, t);
	for (int i=0;i<sizeof(t);i++)
	{
		gonder(t[i]);
	}
	return;
}



