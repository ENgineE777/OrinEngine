//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2011 - Julien Fryer - julienfryer@gmail.com				//
//																				//
// This software is provided 'as-is', without any express or implied			//
// warranty.  In no event will the authors be held liable for any damages		//
// arising from the use of this software.										//
//																				//
// Permission is granted to anyone to use this software for any purpose,		//
// including commercial applications, and to alter it and redistribute it		//
// freely, subject to the following restrictions:								//
//																				//
// 1. The origin of this software must not be misrepresented; you must not		//
//    claim that you wrote the original software. If you use this software		//
//    in a product, an acknowledgment in the product documentation would be		//
//    appreciated but is not required.											//
// 2. Altered source versions must be plainly marked as such, and must not be	//
//    misrepresented as being the original software.							//
// 3. This notice may not be removed or altered from any source distribution.	//
//////////////////////////////////////////////////////////////////////////////////

#include <SPARK_Core.h>
#include "SPK_Buffer.h"
#include "Root/Root.h"

using namespace Orin;

namespace SPK
{
	GLBuffer::GLBuffer(size_t nbVertices,size_t nbTexCoords) :
		nbVertices(nbVertices),
		nbTexCoords(nbTexCoords),
		texCoordBuffer(NULL),
		currentVertexIndex(0),
		currentColorIndex(0),
		currentTexCoordIndex(0)
	{
		SPK_ASSERT(nbVertices > 0,"GLBuffer::GLBuffer(size_t,size_t) - The number of vertices cannot be 0");

		vertexBuffer = SPK_NEW_ARRAY(Vector3D, nbVertices * 4);
		colorBuffer = SPK_NEW_ARRAY(Color, nbVertices * 4);

		if (nbTexCoords > 0)
			texCoordBuffer = SPK_NEW_ARRAY(float, nbVertices * nbTexCoords * 4);

		vbuffer = root.render.GetDevice()->CreateBuffer((int)nbVertices * 4, sizeof(Vertex), _FL_);

		indices = root.render.GetDevice()->CreateBuffer((int)nbVertices * 6, sizeof(uint32_t), _FL_);
		uint32_t* mesh_indices = (uint32_t*)indices->Lock();

		for (int i = 0; i < nbVertices; i++)
		{
			mesh_indices[i * 6 + 0] = i * 4 + 0;
			mesh_indices[i * 6 + 1] = i * 4 + 1;
			mesh_indices[i * 6 + 2] = i * 4 + 2;
			mesh_indices[i * 6 + 3] = i * 4 + 2;
			mesh_indices[i * 6 + 4] = i * 4 + 0;
			mesh_indices[i * 6 + 5] = i * 4 + 3;
		}

		indices->Unlock();
	}

	GLBuffer::~GLBuffer()
	{
		SPK_DELETE_ARRAY(vertexBuffer);
		SPK_DELETE_ARRAY(colorBuffer);
		SPK_DELETE_ARRAY(texCoordBuffer);
	}

	void GLBuffer::setNbTexCoords(size_t nb)
	{
		if (nbTexCoords != nb)
		{
			nbTexCoords = nb;
			SPK_DELETE_ARRAY(texCoordBuffer);
			if (nbTexCoords > 0)
				texCoordBuffer = SPK_NEW_ARRAY(float,nbVertices * nbTexCoords);
			currentTexCoordIndex = 0;
		}
	}

	void GLBuffer::render(int primitive,size_t nbVertices)
	{
		Vertex* vertices = (Vertex*)vbuffer->Lock();

		for (int i = 0; i < nbVertices * 4; i++)
		{
			vertices[i].pos = {vertexBuffer[i].x, vertexBuffer[i].y, vertexBuffer[i].z};
			vertices[i].color = colorBuffer[i].getABGR();

			if (nbTexCoords > 0)
			{
				vertices[i].uv = {texCoordBuffer[i * 2], texCoordBuffer[i * 2 + 1]};
			}
		}

		vbuffer->Unlock();

		root.render.GetDevice()->SetVertexBuffer(0, vbuffer);
		root.render.GetDevice()->SetIndexBuffer(indices);

		root.render.GetDevice()->DrawIndexed(PrimitiveTopology::TrianglesList, 0, 0, (int)nbVertices * 2);
	}
}
