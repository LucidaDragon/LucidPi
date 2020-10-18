#pragma once

namespace System::Environment::RawMemory
{
	ADDR MemoryStart;
	ADDR MemoryLength;
	ADDR MemoryPointer;

	U0 InitializeRaw(ADDR start, ADDR length)
	{
		MemoryStart = start;
		MemoryLength = length;
		MemoryPointer = start;
	}

	U0* AllocateRaw(ADDR size)
	{
		if (MemoryLength >= (MemoryPointer + size))
		{
			U0* result = (U0*)MemoryPointer;

			MemoryPointer += size;

			return result;
		}
		else
		{
			return NULL;
		}
	}

	U0 FreeRaw()
	{
		MemoryPointer = MemoryStart;
	}
}