#include <stdio.h> //standart Giriþ- Çýkýþ

 #define C_DefaultDigitsToTransmit_U8          0xffu    // Will transmit the exact digits in the number
 #define C_MaxDigitsToTransmit_U8              10u      // Max decimal/hexadecimal digits to be transmitted
 #define C_NumOfBinDigitsToTransmit_U8         16u      // Max bits of a binary number to be transmitted
 #define C_MaxDigitsToTransmitUsingPrintf_U8   C_DefaultDigitsToTransmit_U8 /* Max dec/hexadecimal digits to be displayed using printf */
 /**************************************************************************************************/
void gonder(char kar); //Veri Gönderme
int al(char kar);   //Veri Alma
FILE data= FDEV_SETUP_STREAM(gonder, al, _FDEV_SETUP_RW);//(gönderilen, alýnan, Etkilenen Bayrak)) / _FDEV_SETUP_RW= okuma - yazma



void seri_yukle(uint16_t CPU_MHZ,uint16_t BAUD )
{
	int ubrr=(CPU_MHZ*1000000)/16/BAUD-1;
	stdout=stdin=&data; //standart girdi ve ciktiyi veriler degiskenine yonlendir
	UCSR0A=0x00; // Senkron için kullanýlýr. asenkron yaptýðým için kapalý
	UCSR0B=0x18; // 00011000 - TXEN - PD0 pini Alýcý olur / RXEN - PD1 pini Gönderir
	UCSR0C=0x86; // 10000110 - UCSZ0 / UCSZ1 / ursel akitf.
	UBRR0H=0x00;
	UBRR0L=ubrr; // Baðlantý hýzý
}

void gonder(char kar)
{
	//loop_until_bit_is_set(UCSRA, UDRE); // ucsra nýn 5. yazmacý olan udre
	// 1 olduðunda, yeni veri için boþ olduðunu gösterir. eðer boþsa gönder iþlemi yapýlýr
	while (!(UCSR0A&(1<<UDRE0)));			// 2. yol
	
	UDR0 = kar;							// Gönderme ve alma UDR den geçer.
}

int al(char kar)
{
	//loop_until_bit_is_set(UCSRA,RXC); // UCSRA içindeki 7. bitinde bulunan RXC yazmacý akitif olduðunda kullan
	
	while (!(UCSR0A & (1<<RXC0)));
	return kar= UDR0;
}
#if ((Enable_UART_TxNumber==1) || (Enable_UART_TxFloatNumber==1) || (Enable_UART_Printf == 1))
void UART_TxNumber(uint8_t v_numericSystem_u8, uint32_t v_number_u32, uint8_t v_numOfDigitsToTransmit_u8)
{
    uint8_t i=0,a[10];

    if(C_BINARY_U8 == v_numericSystem_u8)
    {
        while(v_numOfDigitsToTransmit_u8!=0)
        {
            /* Start Extracting the bits from the specified bit positions.
             Get the Acsii values of the bits and transmit */
            i = util_GetBitStatus(v_number_u32,(v_numOfDigitsToTransmit_u8-1));
            gonder(util_Dec2Ascii(i));
            v_numOfDigitsToTransmit_u8--;
        }    
    }     
    else if(v_number_u32==0)
    {
        /* If the number is zero then update the array with the same for transmitting */
        for(i=0;((i<v_numOfDigitsToTransmit_u8) && (i<C_MaxDigitsToTransmit_U8)) ;i++)
            gonder('0');
    }
    else
    {
        for(i=0;i<v_numOfDigitsToTransmit_u8;i++)
        {
            /* Continue extracting the digits from right side
               till the Specified v_numOfDigitsToTransmit_u8 */
            if(v_number_u32!=0)
            {
                /* Extract the digits from the number till it becomes zero.
                First get the remainder and divide the number by 10 each time.

                example for Decimal number:
                If v_number_u32 = 123 then extracted remainder will be 3 and number will be 12.
                The process continues till it becomes zero or max digits reached*/
                a[i]=util_GetMod32(v_number_u32,v_numericSystem_u8);
                v_number_u32=v_number_u32/v_numericSystem_u8;
            }
            else if( (v_numOfDigitsToTransmit_u8 == C_DefaultDigitsToTransmit_U8) ||
                    (v_numOfDigitsToTransmit_u8 > C_MaxDigitsToTransmit_U8))
            {
                /* Stop the iteration if the Max number of digits are reached or
                 the user expects exact(Default) digits in the number to be transmitted */ 
                break;
            }
            else
            {
                /*In case user expects more digits to be transmitted than the actual digits in number,
                  then update the remaining digits with zero.
                Ex: v_number_u32 is 123 and user wants five digits then 00123 has to be transmitted */
                a[i]=0;
            }
        }

        while(i)
        { 
            /* Finally get the ascii values of the digits and transmit*/
            gonder(util_Hex2Ascii(a[i-1]));
            i--;
        }
    }


}
#endif
