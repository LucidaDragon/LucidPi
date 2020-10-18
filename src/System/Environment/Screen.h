#pragma once
#include "RawMemory.h"

namespace System::Environment::Screen::Internal
{
	const U32 PropertyChannel = 8;
	
	enum
	{
		PERIPHERAL_BASE = 0x3F000000,
		MAILBOX_OFFSET = 0xB880,
		MAILBOX_BASE = PERIPHERAL_BASE + MAILBOX_OFFSET
	};

	struct FrameInfo
	{
		U32 Width;
		U32 Height;
		U32 Pitch;
		U0* Buffer;
		U32 BufferSize;
		U0* BackBuffer;
	};

	FrameInfo Frame;

	enum BufferCode
	{
		REQUEST = 0x00000000,
		RESPONSE_SUCCESS = 0x80000000,
		RESPONSE_ERROR = 0x80000001
	};

	enum FramePropertyTag
	{
		NULL_TAG = 0,
		ALLOCATE_BUFFER = 0x00040001,
		RELEASE_BUFFER = 0x00048001,
		GET_PHYSICAL_DIMENSIONS = 0x00040003,
		SET_PHYSICAL_DIMENSIONS = 0x00048003,
		GET_VIRTUAL_DIMENSIONS = 0x00040004,
		SET_VIRTUAL_DIMENSIONS = 0x00048004,
		GET_BITS_PER_PIXEL = 0x00040005,
		SET_BITS_PER_PIXEL = 0x00048005,
		GET_BYTES_PER_ROW = 0x00040008
	};

	struct FrameAllocation
	{
		U0* Address;
		U32 Size;
	};

	struct ScreenSize
	{
		U32 Width;
		U32 Height;
	};

	union ValueBuffer
	{
		U32 Alignment;
		FrameAllocation Allocation;
		ScreenSize Size;
		U32 BitsPerPixel;
		U32 BytesPerRow;
	};

	struct MessageBuffer
	{
		U32 Size;
		BufferCode Code;
		U32 Tags[1];
	};

	struct FrameProperty
	{
		FramePropertyTag Tag;
		ValueBuffer Value;	
	};

	struct MailMessage
	{
		U8 Channel: 4;
		U32 Data: 28;
	};

	struct MailStatus
	{
		U32 Reserved: 30;
		U8 Empty: 1;
		U8 Full: 1;
	};

	MailMessage* MAIL0_READ = (MailMessage*)MAILBOX_BASE;
	MailMessage* MAIL0_WRITE = (MailMessage*)(MAILBOX_BASE + 0x20);
	MailStatus* MAIL0_STATUS = (MailStatus*)(MAILBOX_BASE + 0x18);

	static U32 GetBufferLength(FrameProperty* property)
	{
		switch (property->Tag)
		{
			case ALLOCATE_BUFFER:
			case GET_PHYSICAL_DIMENSIONS:
			case SET_PHYSICAL_DIMENSIONS:
			case GET_VIRTUAL_DIMENSIONS:
			case SET_VIRTUAL_DIMENSIONS:
				return 8;

			case GET_BITS_PER_PIXEL:
			case SET_BITS_PER_PIXEL:
			case GET_BYTES_PER_ROW:
				return 4;
			
			case RELEASE_BUFFER:
			default:
				return 0;
		}
	}

	MailMessage ReadFromMailbox(I32 channel)
	{
		MailStatus status;
		MailMessage message;

		do
		{
			do
			{
				status = *MAIL0_STATUS;
			} while (status.Empty);

			message = *MAIL0_READ;
		} while (message.Channel != channel);

		return message;
	}

	U0 SendToMailbox(MailMessage message, I32 channel)
	{
		MailStatus status;
		message.Channel = channel;

		do
		{
			status = *MAIL0_STATUS;
		} while (status.Full);

		*MAIL0_WRITE = message;
	}

	I32 SendMessages(FrameProperty* properties)
	{
		MessageBuffer* message;
		MailMessage mail;
		U32 bufferSize = 0, i, length, bufferPosition;

		for (i = 0; properties[i].Tag != NULL_TAG; i++)
		{
			bufferSize += GetBufferLength(&properties[i]) + (sizeof(U32) * 3);
		}

		bufferSize += sizeof(U32) * 3;

		bufferSize += (bufferSize % 16) ? (16 - (bufferSize % 16)) : 0;

		message = (MessageBuffer*)System::Environment::RawMemory::AllocateRaw(bufferSize);

		if (!message) return -1;

		message->Size = bufferSize;
		message->Code = REQUEST;

		for (i = 0, bufferPosition = 0; properties[i].Tag != NULL_TAG; i++)
		{
			length = GetBufferLength(&properties[i]);
			message->Tags[bufferPosition++] = properties[i].Tag;
			message->Tags[bufferPosition++] = length;
			message->Tags[bufferPosition++] = 0;

			memcpy(message->Tags + bufferPosition, &properties[i].Value, length);

			bufferPosition += length / 4;
		}

		message->Tags[bufferPosition] = 0;

		mail.Data = ((U32)message) >> 4;

		SendToMailbox(mail, PropertyChannel);

		mail = ReadFromMailbox(PropertyChannel);

		if (message->Code == REQUEST)
		{
			System::Environment::RawMemory::FreeRaw();
			return 1;
		}
		else if (message->Code == RESPONSE_ERROR)
		{
			System::Environment::RawMemory::FreeRaw();
			return 2;
		}

		for (i = 0, bufferPosition = 0; properties[i].Tag != NULL_TAG; i++)
		{
			length = GetBufferLength(&properties[i]);
			bufferPosition += 3;

			memcpy(&properties[i].Value, message->Tags + bufferPosition, length);

			bufferPosition += length / 4;
		}

		System::Environment::RawMemory::FreeRaw();
		return 0;
	}

	I32 InitializeFrameBuffer()
	{
		FrameProperty properties[5];
		I32 error;

		properties[0].Tag = SET_PHYSICAL_DIMENSIONS;
		properties[0].Value.Size.Width = 640;
		properties[0].Value.Size.Height = 480;
		properties[1].Tag = SET_VIRTUAL_DIMENSIONS;
		properties[1].Value.Size.Width = 640;
		properties[1].Value.Size.Height = 480;
		properties[2].Tag = SET_BITS_PER_PIXEL;
		properties[2].Value.BitsPerPixel = 24;
		properties[3].Tag = NULL_TAG;

		error = SendMessages(properties);

		if (error != 0)
		{
			return error + 10;
		}

		Frame.Width = properties[0].Value.Size.Width;
		Frame.Height = properties[0].Value.Size.Height;
		Frame.Pitch = Frame.Width * 3;

		properties[0].Tag = ALLOCATE_BUFFER;
		properties[0].Value.Size.Width = 0;
		properties[0].Value.Size.Height = 0;
		properties[0].Value.Alignment = 16;
		properties[1].Tag = NULL_TAG;

		error = SendMessages(properties);

		if (error != 0)
		{
			return error + 20;
		}

		Frame.Buffer = properties[0].Value.Allocation.Address;
		Frame.BufferSize = properties[0].Value.Allocation.Size;
		Frame.BackBuffer = System::Environment::RawMemory::AllocateRaw(Frame.BufferSize);

		return 0;
	}
}

namespace System::Environment::Screen
{
	using namespace System::Environment::Screen::Internal;

	struct Color
	{
		U8 R;
		U8 G;
		U8 B;
	};

	namespace Colors
	{
		static const Color WHITE = {0xff, 0xff, 0xff};
    	static const Color BLACK = {0x00, 0x00, 0x00};
	}

	I32 Initialize()
	{
		return InitializeFrameBuffer();
	}

	U32 GetWidth()
	{
		return Frame.Width;
	}

	U32 GetHeight()
	{
		return Frame.Height;
	}

	Color GetPixel(U32 x, U32 y)
	{
		Color color;

		U8* addressBack = (U8*)Frame.BackBuffer + (y * Frame.Pitch) + (x * sizeof(Color));

		memcpy(&color, addressBack, sizeof(Color));

		return color;
	}

	U0 SetPixel(U32 x, U32 y, Color color, BOOL overlay = 0)
	{
		U8* address = (U8*)Frame.Buffer + (y * Frame.Pitch) + (x * sizeof(Color));
		memcpy(address, &color, sizeof(Color));

		if (!overlay)
		{
			U8* addressBack = (U8*)Frame.BackBuffer + (y * Frame.Pitch) + (x * sizeof(Color));
			memcpy(addressBack, &color, sizeof(Color));
		}
	}

	U0 Clear(Color color)
	{
		for	(U32 i = 0; i < Frame.Width; i++)
		{
			for (U32 j = 0; j < Frame.Height; j++)
			{
				SetPixel(i, j, color);
			}
		}
	}
}