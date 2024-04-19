//***********************************************************************************************
// Universidad del Valle de Guatemala
// IE2023: Programación de Microcontroladores
// Autor: Héctor Martínez - 22369
// Proyecto: prelab 6
// Hardware: atmega328p
//***********************************************************************************************
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define F_CPU 16000000 
#define BAUD 9600        
#define UBRR_VAL (F_CPU/16/BAUD-1) // Calcular el valor UBRR

void USART_Init(unsigned int ubrr) {

	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)ubrr;
// Habilitar receptor y transmisor
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
// Establecer formato de trama: 8 bits de datos, 1 bit de parada, sin paridad
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void USART_Transmit(unsigned char data) {
// Esperar a que el buffer de transmisión esté vacío
	while (!(UCSR0A & (1 << UDRE0)));
// Coloca los datos en el buffer, envía los datos.
	UDR0 = data;
}

uint16_t readADC(uint8_t channel) {
//Selecciona el canal ADC
	ADMUX = (1 << REFS0) | (channel & 0x07);
// Iniciar conversión 
	ADCSRA |= (1 << ADSC);
// Espera a que se complete la conversión
	while (ADCSRA & (1 << ADSC));
// Devuelve el resultado del ADC
	return ADC;
}

int main(void) {
// Inicializa USART
	USART_Init(UBRR_VAL);
// Inicializar ADC
	ADMUX |= (1 << REFS0); // Voltaje de referencia en AVCC
	ADCSRA |= (1 << ADEN); // Habilitar ADC
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Establece el prescaler ADC en 128
	
	while (1) {
// Leer el valor ADC del potenciómetro 
		uint16_t adc_value = readADC(0);
// Escalar el valor de ADC al rango 0-1023
        adc_value = (adc_value * 1023) / 1023;
// Transmite el valor ADC a través de USART
		USART_Transmit(adc_value & 0xFF); // Enviar byte inferior
		USART_Transmit((adc_value >> 8) & 0xFF); // Enviar byte superior

		_delay_ms(1000); 
	}
}


