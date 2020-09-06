
#pragma once

#include "Root/Render/DataBuffer.h"
#include <eastl/vector.h>

struct ID3D11Buffer;

namespace Oak
{
	class DataBufferDX11 : public DataBuffer
	{
	public:

		ID3D11Buffer* buffer = nullptr;

		DataBufferDX11(int sz, int strd);

		virtual void* Lock();
		virtual void Unlock();
		virtual void Release();
	};
}