
#pragma once

#include "Root/Render/Render.h"

namespace Oak
{
	class DebugSprites : public Object
	{
		struct SpriteVertex
		{
			Math::Vector3 Position;
			Math::Vector2 Texcoords;
		};

		Program* prg;
		VertexDecl* vdecl;
		DataBuffer* vbuffer;

		struct Sprite
		{
			Texture* texture;
			Color    color;
			Math::Vector2  pos;
			Math::Vector2  size;
			Math::Vector2  offset = 0.0f;
			float    angle = 0.0f;
		};

		std::vector<Sprite> sprites; 

	public:

		void Init(TaskExecutor::SingleTaskPool* debugTaskPool);
		void AddSprite(Texture* texture, Math::Vector2 pos, Math::Vector2 size, Math::Vector2 offset, float angle, Color color);
		void Draw(float dt);
		void Release();
	};
}
