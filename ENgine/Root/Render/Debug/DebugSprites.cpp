#include "DebugSprites.h"
#include "DebugPrograms.h"

namespace Oak
{
	void DebugSprites::Init(TaskExecutor::SingleTaskPool* debugTaskPool)
	{
		VertexDecl::ElemDesc desc[] = { { ElementType::Float3, ElementSemantic::Position, 0 },{ ElementType::Float2, ElementSemantic::Texcoord, 0 } };
		vdecl = root.render.GetDevice()->CreateVertexDecl(2, desc);

		debugTaskPool->AddTask(1000, this, (Object::Delegate)&DebugSprites::Draw);

		vbuffer = root.render.GetDevice()->CreateBuffer(sizeof(SpriteVertex) * 4, sizeof(SpriteVertex));

		SpriteVertex* Data = (SpriteVertex*)vbuffer->Lock();

		Data[0].Position.x = 0.0f;
		Data[1].Position.x = 0.0f;
		Data[2].Position.x = 1.0f;
		Data[3].Position.x = 1.0f;

		Data[0].Position.y = 1.0f;
		Data[1].Position.y = 0.0f;
		Data[2].Position.y = 1.0f;
		Data[3].Position.y = 0.0f;

		Data[0].Position.z = 0.0f;
		Data[1].Position.z = 0.0f;
		Data[2].Position.z = 0.0f;
		Data[3].Position.z = 0.0f;

		Data[0].Texcoords.x = 0.0f;
		Data[1].Texcoords.x = 0.0f;
		Data[2].Texcoords.x = 1.0f;
		Data[3].Texcoords.x = 1.0f;

		Data[0].Texcoords.y = 1.0f;
		Data[1].Texcoords.y = 0.0f;
		Data[2].Texcoords.y = 1.0f;
		Data[3].Texcoords.y = 0.0f;

		vbuffer->Unlock();

		prg = root.render.GetProgram("DbgSprite");
	}

	void DebugSprites::AddSprite(Texture* texture, Math::Vector2 pos, Math::Vector2 size, Math::Vector2 offset, float angle, Color color)
	{
		if (sprites.size() > 2000)
		{
			return;
		}

		sprites.push_back(Sprite());
		Sprite* spr = &sprites[sprites.size()-1];

		spr->texture = texture;
		spr->color = color;
		spr->pos = pos;
		spr->size = size;
		spr->offset = offset;
		spr->angle = angle;
	}

	void DebugSprites::Draw(float dt)
	{
		if (sprites.size() == 0)
		{
			return;
		}

		root.render.GetDevice()->SetVertexDecl(vdecl);
		root.render.GetDevice()->SetVertexBuffer( 0, vbuffer);

		root.render.GetDevice()->SetProgram(prg);
		Math::Vector4 params[3];

		params[0] = Math::Vector4((float)root.render.GetDevice()->GetWidth(), (float)root.render.GetDevice()->GetHeight(), 0, 0);

		for (auto& sprite : sprites)
		{
			prg->SetTexture(ShaderType::Pixel, "diffuseMap", sprite.texture ? sprite.texture : root.render.GetWhiteTexture());
			prg->SetVector(ShaderType::Pixel, "color", (Math::Vector4*)&sprite.color.r, 1);

			params[1] = Math::Vector4(sprite.pos.x, sprite.pos.y, sprite.size.x, sprite.size.y);
			params[2] = Math::Vector4(sprite.offset.x, sprite.offset.y, sprite.angle, 0.0f);

			prg->SetVector(ShaderType::Vertex, "desc", params, 3);

			root.render.GetDevice()->Draw(PrimitiveTopology::TriangleStrip, 0, 2);
		}

		sprites.clear();
	}

	void DebugSprites::Release()
	{
		RELEASE(vbuffer)
	}
}
