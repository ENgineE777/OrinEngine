
#include "Render.h"
#include "Root/Files/FileInMemory.h"

#include "Debug/Debug.h"
#include <memory>

#ifdef PLATFORM_WIN
#include "DX11/DeviceDX11.h"
#endif

#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Oak
{
	bool Render::Init(const char* device_name, void* external_device)
	{
		#ifdef PLATFORM_WIN
		device = NEW DeviceDX11();
		#endif

		groupTaskPool = root.taskExecutor.CreateGroupTaskPool(_FL_);
		debugTaskPool = groupTaskPool->AddTaskPool(_FL_);

		if (!device->Init(external_device))
		{
			return false;
		}

		device->SetCulling(CullMode::CullCCW);
		device->SetBlendFunc(BlendArg::ArgSrcAlpha, BlendArg::ArgInvSrcAlpha);
		device->SetDepthTest(true);

		spheres = NEW DebugSpheres();
		spheres->Init(debugTaskPool);

		boxes = NEW DebugBoxes();
		boxes->Init(debugTaskPool);

		triangles = NEW DebugTriangles();
		triangles->Init(debugTaskPool);

		lines = NEW DebugLines();
		lines->Init(debugTaskPool);

		sprites = NEW DebugSprites();
		sprites->Init(debugTaskPool);

		triangles2D = NEW DebugTriangles2D();
		triangles2D->Init(debugTaskPool);

		font = NEW DebugFont();
		font->Init(debugTaskPool);

		whiteTex = device->CreateTexture(2, 2, TextureFormat::FMT_A8R8G8B8, 0, false, TextureType::Tex2D, _FL_);

		uint8_t white_tex_data[4 * 4];
		memset(white_tex_data, 255, 16);

		whiteTex->Update(0, 0, white_tex_data, 2 * 4);

		return true;
	}

	Device* Render::GetDevice()
	{
		return device;
	}

	void Render::SetTransform(TransformStage stage, Math::Matrix mat)
	{
		if (stage != TransformStage::WrldViewProj)
		{
			trans[(int)stage] = mat;
			needCalcTrans = true;
		}
	}

	void Render::GetTransform(TransformStage stage, Math::Matrix& mat)
	{
		if (stage == TransformStage::WrldViewProj && needCalcTrans)
		{
			CalcTrans();
		}

		mat = trans[(int)stage];
	}

	void Render::CalcTrans()
	{
		trans[3] = trans[0] * trans[1] * trans[2];
		needCalcTrans = false;
	}

	ProgramRef Render::GetProgram(const char* name, const char* file, int line)
	{
		Program* program = nullptr;

		if (programs.count(name) > 0)
		{
			program = programs[name];
		}
		else
		{
			auto decls = ClassFactoryProgram::Decls();
			program = ClassFactoryProgram::Create(name, file, line);
			program->Init();
			program->name = name;
			device->PrepareProgram(program);

			programs[name] = program;
		}

		return ProgramRef(program, file, line);
	}

	TextureRef Render::LoadTexture(const char* name, const char* file, int line)
	{
		Texture* texture = nullptr; 

		if (textures.count(name) > 0)
		{
			texture = textures[name];
		}
		else
		{
			FileInMemory buffer;

			if (!buffer.Load(name))
			{
				return TextureRef();
			}

			uint8_t* ptr = buffer.GetData();

			int bytes;
			int width;
			int height;
			uint8_t* data = stbi_load_from_memory(ptr, buffer.GetSize(), &width, &height, &bytes, STBI_rgb_alpha);

			texture = device->CreateTextureInner(width, height, TextureFormat::FMT_A8R8G8B8, 0, false, TextureType::Tex2D, _FL_);
			texture->name = name;

			texture->Update(0, 0, data, width * 4);

			free(data);

			texture->GenerateMips();

			textures[name] = texture;
		}

		return TextureRef(texture, file, line);
	}

	void Render::AddExecutedLevelPool(int level)
	{
		groupTaskPool->AddFilter(level);
	}

	void Render::ExecutePool(int level, float dt)
	{
		groupTaskPool->ExecutePool(level, dt);
	}

	TaskExecutor::SingleTaskPool* Render::AddTaskPool(const char* file, int line)
	{
		return groupTaskPool->AddTaskPool(file, line);
	}

	void Render::DelTaskPool(TaskExecutor::SingleTaskPool* pool)
	{
		groupTaskPool->DelTaskPool(pool);
	}

	void Render::Execute(float dt)
	{
		groupTaskPool->Execute(dt);
	}

	void Render::DebugLine(Math::Vector3 from, Color from_clr, Math::Vector3 to, Color to_clr, bool use_depth)
	{
		lines->AddLine(from, from_clr, to, to_clr, use_depth);
	}

	void Render::DebugLine2D(Math::Vector2 from, Color from_clr, Math::Vector2 to, Color to_clr)
	{
		lines->AddLine2D(from, from_clr, to, to_clr);
	}

	void Render::DebugRect2D(Math::Vector2 from, Math::Vector2 to, Color color)
	{
		lines->AddLine2D(Math::Vector2(from.x, from.y), color, Math::Vector2(to.x  , from.y), color);
		lines->AddLine2D(Math::Vector2(to.x  , from.y), color, Math::Vector2(to.x  , to.y  ), color);
		lines->AddLine2D(Math::Vector2(to.x  , to.y  ), color, Math::Vector2(from.x, to.y  ), color);
		lines->AddLine2D(Math::Vector2(from.x, to.y  ), color, Math::Vector2(from.x, from.y), color);
	}

	void Render::DebugSphere(Math::Vector3 pos, Color color, float radius, bool full_shade)
	{
		if (full_shade)
		{
			spheres->AddSphere(pos, color, radius);
		}
		else
		{
			lines->DrawCircle(0, pos, color, radius);
			lines->DrawCircle(1, pos, color, radius);
			lines->DrawCircle(2, pos, color, radius);
		}
	}

	void Render::DebugBox(Math::Matrix pos, Color color, Math::Vector3 scale)
	{
		boxes->AddBox(pos, color, scale);
	}

	void Render::DebugTriangle(Math::Vector3 p1, Math::Vector3 p2, Math::Vector3 p3, Color color)
	{
		triangles->AddTriangle(p1, p2, p3, color);
	}

	void Render::DebugTriangle2D(Math::Vector2 p1, Math::Vector2 p2, Math::Vector2 p3, Color color)
	{
		triangles2D->AddTriangle(p1, p2, p3, color);
	}

	void Render::DebugPrintText(Math::Vector2 pos, ScreenCorner corner, Color color, const char* text, ...)
	{
		char buffer[256];
		va_list args;
		va_start(args, text);
		vsnprintf(buffer, 256, text, args);
		va_end(args);

		font->AddText(pos, corner, color, buffer);
	}

	void Render::DebugPrintText(Math::Vector3 pos, float dist, Color color, const char* text, ...)
	{
		char buffer[256];
		va_list args;
		va_start(args, text);
		vsnprintf(buffer, 256, text, args);
		perror(buffer);
		va_end(args);

		font->AddText(pos, dist, color, buffer);
	}

	void Render::DebugSprite(Texture* texture, Math::Vector2 pos, Math::Vector2 size, Color color, Math::Vector2 offset, float angle)
	{
		sprites->AddSprite(texture, pos, size, offset, angle, color);
	}

	Math::Vector3 Render::TransformToScreen(Math::Vector3 pos, int type)
	{
		Math::Matrix view;
		GetTransform(TransformStage::View, view);

		Math::Matrix view_proj;
		GetTransform(TransformStage::WrldViewProj, view_proj);

		Math::Vector3 pre_ps = pos * view;
		Math::Vector4 ps2 = view_proj.MulVertex4(pos);
		Math::Vector3 ps;
		ps.Set(ps2.x / ps2.w, ps2.y / ps2.w, ps2.z);

		if (pre_ps.z < 0.0f)
		{
			ps = -ps;
		}

		if (type == 0)
		{
			ps.z = pre_ps.z;
		}
		else
		if (type == 1 || type == 2)
		{
			ps.x = 0.5f + ps.x*0.5f;
			ps.y = 0.5f - ps.y*0.5f;
			ps.z = pre_ps.z;

			if (type == 2)
			{
				ps.x *= GetDevice()->GetWidth();
				ps.y *= GetDevice()->GetHeight();
			}
		}

		return ps;
	}

	void Render::Release()
	{
		groupTaskPool->DelTaskPool(debugTaskPool);
		delete groupTaskPool;

		whiteTex.ReleaseRef();
		RELEASE(lines)
		RELEASE(spheres)
		RELEASE(boxes)
		RELEASE(triangles)
		RELEASE(font)
		RELEASE(sprites)
		RELEASE(triangles2D)

		device->Release();
	}
}
