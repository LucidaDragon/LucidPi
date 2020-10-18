//#define _compile_emulatedsys

#include "Primitives.h"
#include "System.h"
#include "System/Console.h"
#include "System/Environment.h"
#include "System/Environment/RawMemory.h"
#include "System/Environment/UART.h"
#include "System/Environment/Screen.h"
#include "System/Environment/Cursor.h"

extern U8 __end;

using namespace System;
using namespace System::Environment;

extern "C" void kernel_main(U32 r0, U32 r1, U32 atags)
{
	(void) r0;
	(void) r1;
	(void) atags;

	RawMemory::InitializeRaw((ADDR)&__end, 1048576);

	UART::Initialize();
	UART::Write("Initialized UART\r\n");

	UART::Write(itoa(GetMemorySize((U0*)atags)));
	UART::Write(" bytes of memory available.\r\n");
	
	I32 error = Screen::Initialize();

	if (error != 0)
	{
		UART::Write("Failed to initialize video. Code: ");
		UART::Write(itoa(error));
		return;
	}

	Screen::Color c;
	c.R = 0x33;
	c.G = 0x33;
	c.B = 0x33;

	Screen::Clear(c);

	Cursor::SetPosition(Screen::GetWidth() / 2, Screen::GetHeight() / 2);
	Cursor::Redraw();

	while (1)
	{
		U8 chr = UART::ReadChar();

		UART::WriteChar(chr);

		if (chr == '\r') chr = '\n';

		Console::WriteChar(chr);
	}
}