#pragma once

namespace System::Environment::Atag
{
	enum AtagTag
	{
		NONE = 0x00000000,
		CORE = 0x54410001,
		MEM = 0x54410002,
		INITRD2 = 0x54420005,
		CMDLINE = 0x54410009
	};

	struct Region
	{
		U32 Size;
		U32 Start;
	};

	struct InitRegion
	{
		U32 Start;
		U32 Size;
	};

	struct CommandLine
	{
		U8 Line[1];
	};

	struct Atag
	{
		U32 Size;
		AtagTag Data;
		union
		{
			Region TagRegion;
			InitRegion TagInitRegion;
			CommandLine TagCommandLine;
		};
	};
}