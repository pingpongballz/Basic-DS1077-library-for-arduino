#include "Arduino.h"
#include "DS1077.h"
#include "Wire.h"
#include "math.h"

byte mux_word[2] = { 0x18,0x00 };
byte div_word[2] = { 0x00,0x00 };

DS1077::DS1077(unsigned long base_freq)
{
	Wire.begin();
	_base_freq = base_freq;
}

unsigned long DS1077::set_freq(word N_scale, word P1_scale)
{
	if (P1_scale == 1 || P1_scale == 2 || P1_scale == 4 || P1_scale == 8)
	{
		int P1_scale_place = log10(P1_scale) / log10(2);
		set_N_divider(N_scale);
		set_P1_divider(P1_scale_place);
	}
	return _base_freq / (N_scale * P1_scale);



}

void DS1077::reset(void)
{

	mux_word[0] = 0x18;
	mux_word[1] = 0x00;
	div_word[0] = 0x00;
	div_word[1] = 0x00;
	write_to(2, DIV, div_word);
	write_to(2, MUX, mux_word);
}

unsigned long DS1077::set_closest_freq(uint32_t freq)
{
	unsigned int ratio = _base_freq / freq;
	unsigned long closest_freq;
	unsigned long N;
	if (ratio > 8200)
	{
		set_freq(1025, 8);
		closest_freq = _base_freq/8200;
	}
	else if (ratio == 1 || ratio == 0)
	{
		set_freq(1, 1);
		closest_freq = _base_freq;
	}
	else if (ratio >= 2 && ratio <= 1025)
	{
		set_freq(ratio, 1);
		closest_freq = _base_freq / ratio;
	}
	else if (ratio >= 1026 && ratio <= 2050)
	{
		N = round(ratio / 2);
		set_freq(N, 2);
		closest_freq = _base_freq / N;
	}
	else if (ratio >= 2051 && ratio <= 4100)
	{
		N = round(ratio / 4);
		set_freq(N, 4);
		closest_freq = _base_freq / N;
	}
	else if (ratio >= 4101 && ratio <= 8200)
	{
		N = round(ratio / 8);
		set_freq(N, 8);
		closest_freq = _base_freq / N;
	}
	return closest_freq;

}



void DS1077::write_to(int data_bytes, byte reg_addr, byte data[])
{
	Wire.beginTransmission(address);
	Wire.write(reg_addr);
	for (int i = 0; i < data_bytes; i++)
	{
		Wire.write(data[i]);
	}
	Wire.endTransmission();
}

void DS1077::set_N_divider(word divider)
{
	if (divider >= 2 && divider <= 1025)
	{
		mux_word[1] = mux_word[1] & 0b10111111;
		write_to(2, MUX, mux_word);
		divider -= 2;
		divider = divider << 6;
		byte firstbyte = highByte(divider);
		byte secondbyte = lowByte(divider);
		div_word[0] = firstbyte;
		div_word[1] = secondbyte;
		write_to(2, DIV, div_word);
	}
	else if (divider == 1)
	{
		mux_word[1] = mux_word[1] | 0b01000000;
		write_to(2, MUX, mux_word);
	}

}

void DS1077::set_P1_divider(word divider)
{
	if (divider >= 1 && divider <= 4)
	{
		divider = divider << 7;
		mux_word[0] = (mux_word[0] & 0b11111110) | highByte(divider);
		mux_word[1] = (mux_word[0] & 0b01111111) | lowByte(divider);
		write_to(2, MUX, mux_word);
	}
	
}
