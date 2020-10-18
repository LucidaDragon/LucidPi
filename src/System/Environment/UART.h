#pragma once

namespace System::Environment::UART
{
	enum
	{
		// The GPIO registers base address.
		GPIO_BASE = 0x3F200000, // for raspi2 & 3, 0x20200000 for raspi1

		GPPUD = (GPIO_BASE + 0x94),
		GPPUDCLK0 = (GPIO_BASE + 0x98),

		// The base address for UART.
		UART0_BASE = 0x3F201000, // for raspi2 & 3, 0x20201000 for raspi1

		UART0_DR     = (UART0_BASE + 0x00),
		UART0_RSRECR = (UART0_BASE + 0x04),
		UART0_FR     = (UART0_BASE + 0x18),
		UART0_ILPR   = (UART0_BASE + 0x20),
		UART0_IBRD   = (UART0_BASE + 0x24),
		UART0_FBRD   = (UART0_BASE + 0x28),
		UART0_LCRH   = (UART0_BASE + 0x2C),
		UART0_CR     = (UART0_BASE + 0x30),
		UART0_IFLS   = (UART0_BASE + 0x34),
		UART0_IMSC   = (UART0_BASE + 0x38),
		UART0_RIS    = (UART0_BASE + 0x3C),
		UART0_MIS    = (UART0_BASE + 0x40),
		UART0_ICR    = (UART0_BASE + 0x44),
		UART0_DMACR  = (UART0_BASE + 0x48),
		UART0_ITCR   = (UART0_BASE + 0x80),
		UART0_ITIP   = (UART0_BASE + 0x84),
		UART0_ITOP   = (UART0_BASE + 0x88),
		UART0_TDR    = (UART0_BASE + 0x8C)
	};

	static inline U0 WriteMMIO(U32 reg, U32 data)
	{
		*(volatile U32*)reg = data;
	}

	static inline U32 ReadMMIO(U32 reg)
	{
		return *(volatile U32*)reg;
	}

	static inline U0 Delay(I32 count)
	{
		asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n" : "=r"(count): [count]"0"(count) : "cc");
	}

	U0 Initialize()
	{
		WriteMMIO(UART0_CR, 0x00000000);

		WriteMMIO(GPPUD, 0x00000000);
		Delay(150);

		WriteMMIO(GPPUDCLK0, (1 << 14) | (1 << 15));
		Delay(150);

		WriteMMIO(GPPUDCLK0, 0x00000000);

		WriteMMIO(UART0_ICR, 0x7FF);

		WriteMMIO(UART0_IBRD, 1);
		WriteMMIO(UART0_FBRD, 40);

		WriteMMIO(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));

		WriteMMIO(UART0_IMSC, (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10));

		WriteMMIO(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));
	}

	U8 ReadChar()
	{
		while (ReadMMIO(UART0_FR) & (1 << 4));

		return ReadMMIO(UART0_DR);
	}

	U0 WriteChar(U8 c)
	{
		while (ReadMMIO(UART0_FR) & (1 << 5));

		WriteMMIO(UART0_DR, c);
	}

	U0 Write(CString str)
	{
		for (ADDR i = 0; str[i] != '\0'; i++)
		{
			WriteChar((U8)str[i]);
		}
	}
}