#pragma once

namespace System
{
	U0 memcpy(U0* dest, U0* src, I32 bytes)
	{
		U8* d = (U8*)dest, *s = (U8*)src;

		while (bytes--)
		{
			*d++ = *s++;
		}
	}

	U0 bzero(U0* dest, I32 bytes)
	{
		U8* d = (U8*)dest;

		while (bytes--)
		{
			*d++ = 0;
		}
	}

	CString itoa(I32 i)
	{
		static char intbuf[12];
		I32 j = 0, isneg = 0;

		if (i == 0)
		{
			intbuf[0] = '0';
			intbuf[1] = '\0';
			return intbuf;
		}

		if (i < 0)
		{
			isneg = 1;
			i = -i;
		}

		while (i != 0)
		{
			intbuf[j++] = '0' + (i % 10); 
			i /= 10;
		}

		if (isneg)
		{
			intbuf[j++] = '-';
		}

		intbuf[j] = '\0';
		j--;
		i = 0;

		while (i < j)
		{
			isneg = intbuf[i];
			intbuf[i] = intbuf[j];
			intbuf[j] = isneg;
			i++;
			j--;
		}

		return intbuf;
	}
}