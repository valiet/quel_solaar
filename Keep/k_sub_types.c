#include <math.h>
#include <stdio.h>
#include "forge.h"

#define K_SUB_TYPE_CORE_ALLOCATION (1024) /* Core units */
#define K_SUB_TYPE_SI_ALLOCATION (9 * 9 * 9 * 9 * 9 * 9 * 9) /* & exponents */
#define K_SUB_TYPE_CURRANCY_ALLOCATION (26 * 26 * 26) /* Three englich letters*/

typedef enum{
	K_TC_CORE,
	K_TC_EXTENDED_SI,
	K_TC_EXTENDED_CURRANCY,
	K_TC_EXTENDED_EXPLICIT
}KTypeCategory;

uint t_convert_type(uint extended_type)
{
	if(extended_type < K_SUB_TYPE_CORE_ALLOCATION)
		return extended_type;
	if(extended_type < K_SUB_TYPE_CORE_ALLOCATION + K_SUB_TYPE_CURRANCY_ALLOCATION)
		return K_TC_EXTENDED_SI;
}





typedef enum{
	K_SIE_NEGATIVE_FOUR,
	K_SIE_NEGATIVE_THREE,
	K_SIE_NEGATIVE_TWO,
	K_SIE_NEGATIVE_ONE,
	K_SIE_NOT_USED,
	K_SIE_POSITIVE_ONE,
	K_SIE_POSITIVE_TWO,
	K_SIE_POSITIVE_THREE,
	K_SIE_POSITIVE_FOUR,
	K_SIE_COUNT
}KSIExponent;

typedef enum{
	IS_BASE_METER,
	IS_BASE_KILOGRAM,
	IS_BASE_SECOND,
	IS_BASE_AMPERE,
	IS_BASE_KELVIN,
	IS_BASE_MOLE,
	IS_BASE_CANDELA,
	IS_BASE_COUNT
}KSIBaseUnits;

uint32 k_type_enum_si_derived_units(KSIExponent meter, KSIExponent kilogram, KSIExponent second, KSIExponent ampere, KSIExponent kelvin, KSIExponent mole, KSIExponent candela)
{
	uint i, code = 0;
	code = candela;
	code = mole + code * K_SIE_COUNT;
	code = kelvin + code * K_SIE_COUNT;
	code = ampere + code * K_SIE_COUNT;
	code = second + code * K_SIE_COUNT;
	code = kilogram + code * K_SIE_COUNT;
	code = meter + code * K_SIE_COUNT;
}



void k_type_enum_si_unit_name_print(char *buffer, uint type)
{
	KSIExponent base[IS_BASE_COUNT];
	char *names[IS_BASE_COUNT] = {"m", "kg", "s", "a", "k", "mol", "cd"};
	uint i, j, pos = 0, dividors = 0, multiplier = 0;
	for(i = 0; i < IS_BASE_COUNT; i++)
	{
		base[i] = type % K_SIE_COUNT;
		if(base[i] < K_SIE_NOT_USED)
			dividors++;
		if(base[i] > K_SIE_NOT_USED)
			multiplier++;
		type /= K_SIE_COUNT;
	}
	if(multiplier == 0)
		buffer[pos++] = '1';
	for(i = 0; i < IS_BASE_COUNT; i++)
	{
		if(base[i] > K_SIE_NOT_USED)
		{
			for(j = 0; names[i][j] != 0; j++)
				buffer[pos++] = names[i][j];
			if(base[i] == K_SIE_POSITIVE_TWO)
			{
				buffer[pos++] = 0xC2;
				buffer[pos++] = 0xB2;
			}
			if(base[i] == K_SIE_POSITIVE_THREE)
			{
				buffer[pos++] = 0xC2;
				buffer[pos++] = 0xB3;
			}
			if(base[i] == K_SIE_POSITIVE_FOUR)
			{
				buffer[pos++] = 0xE2;
				buffer[pos++] = 0x81;
				buffer[pos++] = 0xB4;
			}
		}
	}
	if(dividors != 0)
	{
		buffer[pos++] = 47; /* divider*/
		if(dividors > 1)
			buffer[pos++] = 40; /* round brackets */
		for(i = 0; i < IS_BASE_COUNT; i++)
		{
			if(base[i] < K_SIE_NOT_USED)
			{
				for(j = 0; names[i][j] != 0; j++)
					buffer[pos++] = names[i][j];
				if(base[i] == K_SIE_NEGATIVE_TWO)
				{
					buffer[pos++] = 0xC2;
					buffer[pos++] = 0xB2;
				}
				if(base[i] == K_SIE_NEGATIVE_THREE)
				{
					buffer[pos++] = 0xC2;
					buffer[pos++] = 0xB3;
				}
				if(base[i] == K_SIE_NEGATIVE_FOUR)
				{
					buffer[pos++] = 0xE2;
					buffer[pos++] = 0x81;
					buffer[pos++] = 0xB4;
				}
			}
		}

		if(dividors > 1)
			buffer[pos++] = 41; /* round brackets */
	}
	buffer[pos] = 0;
}
