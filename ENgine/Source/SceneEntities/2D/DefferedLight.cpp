
#include "DefferedLight.h"
#include "PointLight2D.h"
#include "Root/Root.h"
#include "Editor/Editor.h"

namespace Orin
{
	class GBufferTechnique : public RenderTechnique
	{
	public:
		virtual const char* GetVsName() { return "sprite_vs.shd"; };
		virtual const char* GetPsName() { return "gbuffer_ps.shd"; };

		virtual void ApplyStates()
		{
			//root.render.GetDevice()->SetAlphaBlend(true);
			root.render.GetDevice()->SetCulling(CullMode::CullNone);
		};
	};

	class DefferdLightTechnique : public RenderTechnique
	{
	public:
		virtual const char* GetVsName() { return "deffered_light_vs.shd"; };
		virtual const char* GetPsName() { return "deffered_light_ps.shd"; };

		virtual void ApplyStates()
		{
			//root.render.GetDevice()->SetAlphaBlend(true);
			root.render.GetDevice()->SetCulling(CullMode::CullNone);
		};
	};

	class BlurTechnique : public RenderTechnique
	{
	public:
		virtual const char* GetVsName() { return "Shaders\\blur_vs.shd"; };
		virtual const char* GetPsName() { return "Shaders\\blur_ps.shd"; };

		virtual void ApplyStates()
		{
			GetRoot()->GetRender()->GetDevice()->SetDepthWriting(false);
			GetRoot()->GetRender()->GetDevice()->SetDepthTest(false);
		};
	};

	ENTITYREG(SceneEntity, DefferedLight, "2D/Lights", "DefferedLight")

	META_DATA_DESC(DefferedLight)
		BASE_SCENE_ENTITY_PROP(DefferedLight)
		COLOR_PROP(DefferedLight, ambientColor, COLOR_WHITE, "Visual", "Ambient")
	META_DATA_DESC_END()

	bool DefferedLight::hackStateEnabled = false;
	RenderTechniqueRef DefferedLight::gbufferTech;

	void DefferedLight::Init()
	{
		transform.objectType = ObjectType::Object2D;
		transform.transformFlag = TransformFlag::MoveXYZ | TransformFlag::RotateZ | TransformFlag::ScaleX | TransformFlag::ScaleY;

		Tasks(true)->AddTask(-10, this, (Object::Delegate)&DefferedLight::SetRT);
		Tasks(true)->AddTask(8, this, (Object::Delegate)&DefferedLight::Draw);

		defferdLightTech = root.render.GetRenderTechnique<DefferdLightTechnique>(_FL_);
		blurRTech = GetRoot()->GetRender()->GetRenderTechnique<BlurTechnique>(_FL_);

		VertexDecl::ElemDesc desc[] = { { ElementType::Float2, ElementSemantic::Position, 0 } };
		vdecl = GetRoot()->GetRender()->GetDevice()->CreateVertexDecl(1, desc, _FL_);

		buffer = GetRoot()->GetRender()->GetDevice()->CreateBuffer(4, sizeof(Math::Vector2), _FL_);

		Math::Vector2* v = (Math::Vector2*)buffer->Lock();

		v[0] = Math::Vector2(-1, 1);
		v[1] = Math::Vector2(1, 1);
		v[2] = Math::Vector2(-1, -1);
		v[3] = Math::Vector2(1, -1);

		buffer->Unlock();

	}

	float DefferedLight::ComputeGaussian(float n)
	{
		float theta = 4.0f;

		return (float)((1.0 / sqrtf(2 * Math::PI * theta)) * expf(-(n * n) / (2 * theta * theta)));
	}

	void DefferedLight::SetRT(float dt)
	{
		if (!IsVisible())
		{
			return;
		}

		hackStateEnabled = true;
		if (!gbufferTech)
		{
			gbufferTech = root.render.GetRenderTechnique<GBufferTechnique>(_FL_);
		}

		int screenWidth = root.render.GetDevice()->GetWidth();
		int screenHeight = root.render.GetDevice()->GetHeight();;

		if (!albedoRT || albedoRT->GetWidth() != screenWidth || albedoRT->GetHeight() != screenHeight)
		{
			albedoRT = root.render.GetDevice()->CreateTexture(screenWidth, screenHeight, TextureFormat::FMT_A8R8G8B8, 1, true, TextureType::Tex2D, _FL_);
			materialRT = root.render.GetDevice()->CreateTexture(screenWidth, screenHeight, TextureFormat::FMT_A8R8G8B8, 1, true, TextureType::Tex2D, _FL_);
			normalRT = root.render.GetDevice()->CreateTexture(screenWidth, screenHeight, TextureFormat::FMT_A8R8G8B8, 1, true, TextureType::Tex2D, _FL_);
			selfilumRT = root.render.GetDevice()->CreateTexture(screenWidth, screenHeight, TextureFormat::FMT_A8R8G8B8, 1, true, TextureType::Tex2D, _FL_);
			tempRT = root.render.GetDevice()->CreateTexture(screenWidth, screenHeight, TextureFormat::FMT_A8R8G8B8, 1, true, TextureType::Tex2D, _FL_);

			sceneDepth = root.render.GetDevice()->CreateTexture(screenWidth, screenHeight, TextureFormat::FMT_D24, 1, true, TextureType::Tex2D, _FL_);
		}

		root.render.GetDevice()->SetRenderTarget(0, albedoRT);
		root.render.GetDevice()->SetRenderTarget(1, materialRT);
		root.render.GetDevice()->SetRenderTarget(2, normalRT);
		root.render.GetDevice()->SetRenderTarget(3, selfilumRT);

		root.render.GetDevice()->SetDepth(sceneDepth);

		root.render.GetDevice()->Clear(true, COLOR_BLACK, true, 1.0f);
	}

	void DefferedLight::BlurTexture(TextureRef src, TextureRef dest, float blurStrength)
	{
		GetRoot()->GetRender()->GetDevice()->SetVertexDecl(vdecl);
		GetRoot()->GetRender()->GetDevice()->SetVertexBuffer(0, buffer);

		GetRoot()->GetRender()->GetDevice()->SetRenderTarget(0, tempRT);
		GetRoot()->GetRender()->GetDevice()->SetDepth(nullptr);

		GetRoot()->GetRender()->GetDevice()->SetRenderTechnique(blurRTech);
		blurRTech->SetTexture(ShaderType::Pixel, "rt", src);

		Math::Vector4 samples[15];

		samples[0].z = ComputeGaussian(0);
		samples[0].x = 0;
		samples[0].y = 0;

		float totalWeights = samples[0].z;

		for (int i = 0; i < 7; i++)
		{
			float weight = ComputeGaussian((float)i + 1.0f);

			samples[i * 2 + 1].z = weight;
			samples[i * 2 + 2].z = weight;

			totalWeights += weight * 2;

			float sampleOffset = i * 2 + 1.5f;

			Math::Vector2 delta = Math::Vector2(1.0f / (float)tempRT->GetWidth(), 1.0f / (float)tempRT->GetHeight()) * sampleOffset * blurStrength;

			samples[i * 2 + 1].x = delta.x;
			samples[i * 2 + 1].y = 0.0f;
			samples[i * 2 + 1].w = delta.y;

			samples[i * 2 + 2].x = -delta.x;
			samples[i * 2 + 2].y = 0.0f;
			samples[i * 2 + 2].w = -delta.y;
		}

		for (int i = 0; i < 15; i++)
		{
			samples[i].z /= totalWeights;
		}

		blurRTech->SetVector(ShaderType::Pixel, "samples", samples, 15);

		GetRoot()->GetRender()->GetDevice()->Draw(PrimitiveTopology::TriangleStrip, 0, 2);

		GetRoot()->GetRender()->GetDevice()->SetRenderTarget(0, dest);

		for (int i = 0; i < 15; i++)
		{
			samples[i].x = 0;
			samples[i].y = samples[i].w;
		}

		blurRTech->SetTexture(ShaderType::Pixel, "rt", tempRT);
		blurRTech->SetVector(ShaderType::Pixel, "samples", samples, 15);

		GetRoot()->GetRender()->GetDevice()->Draw(PrimitiveTopology::TriangleStrip, 0, 2);
	}

	void DefferedLight::Draw(float dt)
	{
		if (!IsVisible())
		{
			return;
		}

		root.render.GetDevice()->RestoreRenderTarget();

		BlurTexture(selfilumRT, selfilumRT, 0.75f);
		BlurTexture(selfilumRT, selfilumRT, 0.75f);
		BlurTexture(selfilumRT, selfilumRT, 0.75f);
		BlurTexture(selfilumRT, selfilumRT, 0.75f);

		root.render.GetDevice()->RestoreRenderTarget();

		//root.render.DebugSprite(albedoRT, 10.0f, 100.0f);
		//root.render.DebugSprite(materialRT, {120.0f, 10.0f}, 100.0f);
		//root.render.DebugSprite(normalRT, { 230.0f, 10.0f }, 100.0f);
		//root.render.DebugSprite(selfilumRT, { 340.0f, 10.0f }, 100.0f);

		Math::Vector3 camPos = Sprite::GetCamPos();

		Math::Vector4 u_lights[3 + 4 * 16];

		defferdLightTech->SetTexture(ShaderType::Pixel, "materialMap", materialRT);
		defferdLightTech->SetTexture(ShaderType::Pixel, "normalsMap", normalRT);
		defferdLightTech->SetTexture(ShaderType::Pixel, "selfilumMap", selfilumRT);

		u_lights[0] = { Sprite::GetPixelsHeight() / root.render.GetDevice()->GetAspect() * 0.5f, Sprite::GetPixelsHeight() * 0.5f, camPos.x, camPos.y};

		defferdLightTech->SetVector(ShaderType::Vertex, "desc", u_lights, 1);

		float scale = editor.freeCamera.zoom2D;

		u_lights[0] = { 0.72f, 1.0f, 1.0f, 0.1f };
		u_lights[1] = { 0.6f, 1.0f, 0.0f, 1.0f };
		u_lights[2] = { Sprite::GetPixelsHeight() / root.render.GetDevice()->GetAspect() * 0.5f / scale, Sprite::GetPixelsHeight() * 0.5f / scale, camPos.x, camPos.y };

		int index = 3;

		eastl::vector<Scene::Group*> groups;
		GetScene()->GetGroup(groups, "PointLight2D");

		int lightCount = 0;

		for (auto* group : groups)
		{
			for (auto* entity : group->entities)
			{
				if (!entity->IsVisible())
				{
					continue;
				}

				PointLight2D* light = dynamic_cast<PointLight2D*>(entity);

				auto trans = light->GetTransform();
				auto pos = Sprite::ToPixels(trans.GetGlobal().Pos());

				u_lights[index + 0] = { pos.x, pos.y, 100.0f, 1.0f }; //pos, dir
				u_lights[index + 1] = { light->color.r, light->color.g, light->color.b, light->intesity }; //color, intesity		
				u_lights[index + 2] = { 1.0f, light->falloff, trans.rotation.z * Math::Radian, trans.size.x * 0.5f }; //light_depth, falloff, angle, radius
				u_lights[index + 3] = { light->viewAngle * Math::Radian, light->lineWidth , 0.0f, 0.0f }; //arc, width

				index += 4;
				lightCount++;
			}
		}
		
		u_lights[1].w = (float)lightCount;		

		defferdLightTech->SetVector(ShaderType::Pixel, "u_lights", u_lights, 3 + 4 * lightCount);

		Sprite::Draw(albedoRT, ambientColor, Math::Matrix(), 0.0f, 100.0f, 0.0f, 1.0f, defferdLightTech);

		hackStateEnabled = false;
	}

	void DefferedLight::Release()
	{
		gbufferTech.ReleaseRef();
		SceneEntity::Release();
	}
}