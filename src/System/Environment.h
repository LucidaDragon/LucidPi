#pragma once
#include "Environment/Atag.h"

namespace System::Environment
{
	using namespace System::Environment;

	U32 GetMemorySize(U0* tags)
	{
		Atag::Atag* tag = (Atag::Atag*)tags;

		while (tag->Data != Atag::NONE)
		{
			if (tag->Data == Atag::MEM)
			{
				return tag->TagRegion.Size;
			}

			tag = (Atag::Atag*)((U32*)tag) + tag->Size;
		}

		return 1024 * 1024 * 1024;
	}
}