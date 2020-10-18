#pragma once
#include "Environment/Screen.h"
#include "Environment/Font.h"

namespace System::Console
{
	using namespace System::Environment;

	U32 CharWidth = 8;
	U32 CharHeight = 8;
	U32 X = 0;
	U32 Y = 0;

	Screen::Color Foreground = Screen::Colors::WHITE;
	Screen::Color Background = Screen::Colors::BLACK;
	BOOL Transparent = 1;

	U32 GetWidth()
	{
		return Screen::GetWidth() / CharWidth;
	}

	U32 GetHeight()
	{
		return Screen::GetHeight() / CharHeight;
	}

	U0 WriteChar(U8 c)
	{
		if (c == '\r')
		{
			X = 0;

			return;
		}
		else if (c == '\n')
		{
			X = 0;
			Y++;

			return;
		}

		while (X >= Screen::GetWidth())
		{
			X -= Screen::GetWidth();
			Y++;
		}

		while (Y >= Screen::GetHeight())
		{
			Y -= Screen::GetHeight();
		}

		const U8* bitmap = Font::GetFont(c);

		for (U32 i = 0; i < 8; i++)
		{
			for (U32 j = 0; j < 8; j++)
			{
				U32 mask = 1 << i;

				if (bitmap[j] & mask)
				{
					Screen::SetPixel((X * CharWidth) + i, (Y * CharHeight) + j, Foreground);
				}
				else if (!Transparent)
				{
					Screen::SetPixel((X * CharWidth) + i, (Y * CharHeight) + j, Background);
				}
			}
		}

		X++;

		while (X >= Screen::GetWidth())
		{
			X -= Screen::GetWidth();
			Y++;
		}

		while (Y >= Screen::GetHeight())
		{
			Y -= Screen::GetHeight();
		}
	}

	U0 Write(CString str)
	{
		for (ADDR i = 0; str[i] != '\0'; i++)
		{
			WriteChar((U8)str[i]);
		}
	}
}