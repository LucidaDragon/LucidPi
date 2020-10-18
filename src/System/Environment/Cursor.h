#include "Screen.h"

namespace System::Environment::Cursor
{
	using namespace System::Environment;

	const U32 Width = 12;
	const U32 Height = 19;
	U32 X = 0;
	U32 Y = 0;

	U8 CursorBitmap(U32 x, U32 y)
	{
		static const U8 cursor[19][3] = {
			0b01000000, 0b00000000, 0b00000000,
			0b01010000, 0b00000000, 0b00000000,
			0b01100100, 0b00000000, 0b00000000,
			0b01101001, 0b00000000, 0b00000000,
			0b01101010, 0b01000000, 0b00000000,
			0b01101010, 0b10010000, 0b00000000,
			0b01101010, 0b10100100, 0b00000000,
			0b01101010, 0b10101001, 0b00000000,
			0b01101010, 0b10101010, 0b01000000,
			0b01101010, 0b10101010, 0b10010000,
			0b01101010, 0b10101010, 0b10100100,
			0b01101010, 0b10101010, 0b10101001,
			0b01101010, 0b10101001, 0b01010101,
			0b01101010, 0b01101001, 0b00000000,
			0b01101001, 0b00011010, 0b01000000,
			0b01100100, 0b00011010, 0b01000000,
			0b01010000, 0b00000110, 0b10010000,
			0b00000000, 0b00000110, 0b10010000,
			0b00000000, 0b00000001, 0b01000000
		};

		return (cursor[y][x / 4] >> ((3 - (x % 4)) * 2)) & 0b11;
	}

	U0 SetPosition(U32 x, U32 y)
	{
		for (U32 i = 0; i < Width; i++)
		{
			for (U32 j = 0; j < Height; j++)
			{
				Screen::SetPixel(X + i, Y + j, Screen::GetPixel(X + i, Y + j));
			}
		}

		X = x;
		Y = y;
	}

	U0 AddOffset(I32 x, I32 y)
	{
		SetPosition(X + x, Y + y);
	}

	U0 Redraw()
	{
		for (U32 i = 0; i < Width; i++)
		{
			for (U32 j = 0; j < Height; j++)
			{
				U8 value = CursorBitmap(i, j);

				switch (value)
				{
					case 1:
						Screen::SetPixel(X + i, Y + j, Screen::Colors::BLACK, 1);
						break;
					case 2:
						Screen::SetPixel(X + i, Y + j, Screen::Colors::WHITE, 1);
						break;
				}
			}
		}
	}
}