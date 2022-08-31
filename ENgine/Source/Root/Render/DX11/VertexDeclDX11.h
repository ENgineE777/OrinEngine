
#pragma once

#include "Root/Render/VertexDecl.h"
#include "d3d11.h"
#include "ShaderDX11.h"

namespace Oak
{
	class VertexDeclDX11 : public VertexDecl
	{
		virtual void Release();

	public:

		eastl::vector<D3D11_INPUT_ELEMENT_DESC> layoutDesc;

		eastl::map<ShaderDX11*, ID3D11InputLayout*> layouts;

		VertexDeclDX11(int count, VertexDecl::ElemDesc* elems);
		void Apply(ShaderDX11* shader);
	};
}