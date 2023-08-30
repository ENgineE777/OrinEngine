
#include "DefferedLight.h"
#include "PointLight2D.h"
#include "DirectionLight2D.h"
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
			root.render.GetDevice()->SetDepthTest(false);
			root.render.GetDevice()->SetDepthWriting(false);
			root.render.GetDevice()->SetCulling(CullMode::CullNone);
		};
	};

	class ShadowCastTechnique : public RenderTechnique
	{
	public:
		virtual const char* GetVsName() { return "cast_shadow_vs.shd"; };
		virtual const char* GetPsName() { return "cast_shadow_ps.shd"; };

		virtual void ApplyStates()
		{
			root.render.GetDevice()->SetDepthTest(false);
			root.render.GetDevice()->SetDepthWriting(false);
			root.render.GetDevice()->SetCulling(CullMode::CullNone);
		};
	};

	class ShadowRenderTechnique : public RenderTechnique
	{
	public:
		virtual const char* GetVsName() { return "deffered_light_vs.shd"; };
		virtual const char* GetPsName() { return "render_shadow_ps.shd"; };

		virtual void ApplyStates()
		{
			root.render.GetDevice()->SetDepthTest(false);
			root.render.GetDevice()->SetDepthWriting(false);
			root.render.GetDevice()->SetAlphaBlend(true);
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
		
		FLOAT_PROP(DefferedLight, metallic, 0.25f, "Visual", "metallic", "metallic")
		BOOL_PROP(DefferedLight, useFilter, false, "Filter", "useFilter", "")

		COLOR_PROP(DefferedLight, globalLights[0].ambientColor, COLOR_WHITE, "Main", "AmbientColor")
		FLOAT_PROP(DefferedLight, globalLights[0].ambientIntensity, 1.0f, "Main", "AmbientIntensity", "")
		
		INT_PROP(DefferedLight, globalLights[0].numBlurSelfIlum, 4, "Main", "numBlurSelfIlum", "")
		FLOAT_PROP(DefferedLight, globalLights[0].streangthBlurSelfIlum, 0.75f, "Main", "streangthBlurSelfIlum", "")
		FLOAT_PROP(DefferedLight, globalLights[0].powerSelfIlum, 4.0f, "Main", "powerSelfIlum", "")

		COLOR_PROP(DefferedLight, globalLights[1].ambientColor, COLOR_WHITE, "Secondary", "AmbientColorSec")
		FLOAT_PROP(DefferedLight, globalLights[1].ambientIntensity, 1.0f, "Secondary", "AmbientIntensitySec", "")

		INT_PROP(DefferedLight, globalLights[1].numBlurSelfIlum, 4, "Secondary", "numBlurSelfIlumSec", "")
		FLOAT_PROP(DefferedLight, globalLights[1].streangthBlurSelfIlum, 0.75f, "Secondary", "streangthBlurSelfIlumSec", "")
		FLOAT_PROP(DefferedLight, globalLights[1].powerSelfIlum, 4.0f, "Secondary", "powerSelfIlumSec", "")

	META_DATA_DESC_END()

	bool DefferedLight::hackStateEnabled = false;
	RenderTechniqueRef DefferedLight::gbufferTech;

	void DefferedLight::Init()
	{
		transform.objectType = ObjectType::Object2D;
		transform.transformFlag = TransformFlag::MoveXYZ | TransformFlag::RotateZ | TransformFlag::ScaleX | TransformFlag::ScaleY;

		Tasks(true)->AddTask(-10, this, (Object::Delegate)&DefferedLight::SetRT);
		Tasks(true)->AddTask(6, this, (Object::Delegate)&DefferedLight::Draw);

		defferdLightTech = root.render.GetRenderTechnique<DefferdLightTechnique>(_FL_);
		blurRTech = GetRoot()->GetRender()->GetRenderTechnique<BlurTechnique>(_FL_);

		shadowCastTech = root.render.GetRenderTechnique<ShadowCastTechnique>(_FL_);
		shadowRenderTech = root.render.GetRenderTechnique<ShadowRenderTechnique>(_FL_);

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
			albedoRT->SetAdress(TextureAddress::Clamp);

			materialRT = root.render.GetDevice()->CreateTexture(screenWidth, screenHeight, TextureFormat::FMT_A8R8G8B8, 1, true, TextureType::Tex2D, _FL_);
			materialRT->SetAdress(TextureAddress::Clamp);

			normalRT = root.render.GetDevice()->CreateTexture(screenWidth, screenHeight, TextureFormat::FMT_A8R8G8B8, 1, true, TextureType::Tex2D, _FL_);
			normalRT->SetAdress(TextureAddress::Clamp);

			selfilumRT = root.render.GetDevice()->CreateTexture(screenWidth, screenHeight, TextureFormat::FMT_A8R8G8B8, 1, true, TextureType::Tex2D, _FL_);
			selfilumRT->SetAdress(TextureAddress::Clamp);

			tempRT = root.render.GetDevice()->CreateTexture(screenWidth, screenHeight, TextureFormat::FMT_A8R8G8B8, 1, true, TextureType::Tex2D, _FL_);
			tempRT->SetAdress(TextureAddress::Clamp);

			occluderRT = root.render.GetDevice()->CreateTexture(512, 512, TextureFormat::FMT_A8R8G8B8, 1, true, TextureType::Tex2D, _FL_);
			occluderRT->SetAdress(TextureAddress::Clamp);
			occluderRT->SetFilters(TextureFilter::Point, TextureFilter::Point);

			shadowRT = root.render.GetDevice()->CreateTexture(360, MAX_LIGHTS, TextureFormat::FMT_R32_FLOAT, 1, true, TextureType::Tex2D, _FL_);
		}

		root.render.GetDevice()->SetRenderTarget(0, albedoRT);
		root.render.GetDevice()->SetRenderTarget(1, materialRT);
		root.render.GetDevice()->SetRenderTarget(2, normalRT);
		root.render.GetDevice()->SetRenderTarget(3, selfilumRT);

		root.render.GetDevice()->Clear(true, GetScene()->IsPlaying() ? COLOR_BLACK_A(0.0f) : Color(0.095f, 0.095f, 0.095f, 0.0f), true, 1.0f);
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

	void DefferedLight::GatherLights()
	{		
		eastl::vector<Scene::Group*> groups;

		dirLights.clear();
		GetScene()->GetGroup(groups, "DirectionLight2D");

		for (auto* group : groups)
		{
			for (auto* entity : group->entities)
			{
				if (!entity->IsVisible())
				{
					continue;
				}

				dirLights.push_back(dynamic_cast<DirectionLight2D*>(entity));

				if (dirLights.size() == MAX_LIGHTS)
				{
					break;
				}
			}
		}
		
		lights.clear();
		GetScene()->GetGroup(groups, "PointLight2D");

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
				auto size = trans.size * 0.5f;

				if (!Sprite::IsRectVisibile(Math::Vector2(pos.x, pos.y) - Math::Vector2(size.x, size.y), Math::Vector2(pos.x, pos.y) + Math::Vector2(size.x, size.y)))
				{
					continue;
				}

				lights.push_back(light);

				if (lights.size() == MAX_LIGHTS)
				{
					break;
				}
			}
		}
	}

	void DefferedLight::GenerateShadows()
	{
		Math::Matrix curView;
		Math::Matrix curProj;
		auto camPos = Sprite::GetCamPos();
		auto zoom = Sprite::GetCamZoom();

		root.render.GetTransform(TransformStage::View, curView);
		root.render.GetTransform(TransformStage::Projection, curProj);

		Math::Matrix view;
		Math::Matrix proj;

		for (int i = 0; i < lights.size(); i++)
		{
			auto* light = lights[i];

			if (!light->castShadow || light->lineWidth > 0.001f)
			{
				continue;
			}

			auto trans = light->GetTransform();
			auto pos = trans.GetGlobal().Pos();

			Sprite::SetCamPos(Sprite::ToPixels(Math::Vector2(pos.x, pos.y)));

			const Math::Vector3 upVector{ 0.0f, 1.0f, 0.f };

			float dist = Sprite::ToUnits(trans.size.x * 0.5f) / (tanf(22.5f * Math::Radian));
			view.BuildView(Math::Vector3(pos.x, pos.y, -dist), Math::Vector3(pos.x, pos.y, -dist + 1.0f), upVector);

			root.render.SetTransform(TransformStage::View, view);

			Math::Matrix proj;
			proj.BuildProjection(45.0f * Math::Radian, 1.0f, 1.0f, 1000.0f);
			root.render.SetTransform(TransformStage::Projection, proj);
			
			root.render.GetDevice()->SetRenderTarget(0, occluderRT);
			root.render.GetDevice()->Clear(true, COLOR_WHITE, false, 1.0f);
			root.render.ExecutePool(500, 0.0f);
			root.render.ExecutePool(501, 0.0f);

			Math::Vector4 params;

			params.x = (float)i;
			params.y =  1.0f / (float)shadowRT->GetHeight();

			shadowCastTech->SetVector(ShaderType::Vertex, "shadowParams", &params, 1);

			params.x = 1.0f / (float)occluderRT->GetWidth();
			params.y = 1.0f / (float)occluderRT->GetHeight();

			shadowCastTech->SetVector(ShaderType::Pixel, "params", &params, 1);

			root.render.GetDevice()->SetRenderTarget(0, shadowRT);
			root.render.GetDevice()->SetDepth(nullptr);

			Sprite::Draw(occluderRT, COLOR_WHITE, Math::Matrix(), 0.0f, 100.0f, 0.0f, 1.0f, shadowCastTech);
		}

		root.render.SetTransform(TransformStage::View, curView);
		root.render.SetTransform(TransformStage::Projection, curProj);

		Sprite::SetCamPos(camPos);
		Sprite::SetCamZoom(zoom);
	}

	void DefferedLight::BlurSelfIlum()
	{
		for (int i = 0; i < globalLights[0].numBlurSelfIlum; i++)
		{
			BlurTexture(selfilumRT, selfilumRT, globalLights[0].streangthBlurSelfIlum);
		}
	}

	void DefferedLight::RenderLights()
	{
		Math::Vector3 camPos = Sprite::GetCamPos();

		Math::Vector4 u_lights[5 + 4 * MAX_LIGHTS];

		u_lights[0] = { Sprite::GetPixelsHeight() / root.render.GetDevice()->GetAspect() * 0.5f, Sprite::GetPixelsHeight() * 0.5f, camPos.x, camPos.y };

		defferdLightTech->SetVector(ShaderType::Vertex, "desc", u_lights, 1);

		u_lights[0] = { timer, useFilter ? 1.0f : 0.0f, globalLights[0].powerSelfIlum, 0.1f };
		u_lights[1] = { 0.6f, 1.0f, 0.0f, 1.0f };

		auto halfScreenSize = Sprite::GetHalfScreenSize();

		u_lights[2] = { halfScreenSize.x, halfScreenSize.y, camPos.x, camPos.y };
		u_lights[3] = { globalLights[0].ambientColor.r, globalLights[0].ambientColor.g, globalLights[0].ambientColor.b, globalLights[0].ambientIntensity };
		u_lights[4] = { globalLights[1].ambientColor.r, globalLights[1].ambientColor.g, globalLights[1].ambientColor.b, globalLights[1].ambientIntensity };

		int index = 5;

		for (int i = 0; i < dirLights.size(); i++)
		{
			auto* light = dirLights[i];
			
			float rot = light->GetTransform().rotation.z;

			while (rot > 360.0f)
			{
				rot -= 360.0f;
			}

			while (rot < 0.0f)
			{
				rot += 360.0f;
			}

			rot = Math::PI + Math::Radian * rot;

			u_lights[index + 0] = { cosf(rot), sinf(rot), 0.75f, -1.0f }; //pos, dir
			u_lights[index + 1] = { light->color.r, light->color.g, light->color.b, light->intesity };
			u_lights[index + 3] = { 0.0f, 0.0f, light->isSecondaryLight ? 1.0f : 0.0f, 0.0f };

			index += 4;
		}

		for (int i = 0; i < lights.size(); i++)
		{
			auto* light = lights[i];
			auto trans = light->GetTransform();
			auto mat = trans.GetGlobal();
			auto rot = mat.GetRotation();
			auto pos = Sprite::ToPixels(mat.Pos());
			auto size = trans.size * 0.5f;

			u_lights[index + 0] = { pos.x, pos.y, 30.0f, 1.0f }; //pos, dir
			u_lights[index + 1] = { light->color.r, light->color.g, light->color.b, light->intesity }; //color, intesity		
			u_lights[index + 2] = { light->castShadow && light->lineWidth < 0.001f ? (float)i : -1.0f, light->falloff, rot.z, trans.size.x * 0.5f }; //light_depth, falloff, angle, radius
			u_lights[index + 3] = { light->viewAngle * Math::Radian, light->lineWidth , light->isSecondaryLight ? 1.0f : 0.0f, 0.0f }; //arc, width

			index += 4;
		}

		int lightCount = (int)lights.size() + (int)dirLights.size();
		u_lights[1].w = (float)lightCount;

		defferdLightTech->SetVector(ShaderType::Pixel, "u_lights", u_lights, 5 + 4 * lightCount);

		u_lights[0].x = metallic;

		defferdLightTech->SetVector(ShaderType::Pixel, "params", u_lights, 1);
		defferdLightTech->SetVector(ShaderType::Pixel, "g_rougness", u_lights, 5 + 4 * lightCount);
		
		defferdLightTech->SetTexture(ShaderType::Pixel, "materialMap", materialRT);
		defferdLightTech->SetTexture(ShaderType::Pixel, "normalsMap", normalRT);
		defferdLightTech->SetTexture(ShaderType::Pixel, "selfilumMap", selfilumRT);
		defferdLightTech->SetTexture(ShaderType::Pixel, "shadowMap", shadowRT);

		Sprite::Draw(albedoRT, COLOR_WHITE, Math::Matrix(), 0.0f, 100.0f, 0.0f, 1.0f, defferdLightTech);

		//root.render.DebugPrintText(10.0f, ScreenCorner::LeftTop, COLOR_WHITE, "Num lights %i", lightCount);
	}

	void DefferedLight::Draw(float dt)
	{
		if (!IsVisible())
		{
			return;
		}

		timer += dt;

		GatherLights();

		root.render.GetDevice()->RestoreRenderTarget();

		GenerateShadows();

		BlurSelfIlum();

		root.render.GetDevice()->RestoreRenderTarget();

		RenderLights();

		//root.render.DebugSprite(occluderRT, 10.0f, 100.0f);
		//root.render.DebugSprite(shadowRT, {120.0f, 10.0f}, 100.0f);

		//root.render.DebugSprite(albedoRT, 10.0f, 100.0f);
		//root.render.DebugSprite(materialRT, {120.0f, 10.0f}, 100.0f);
		//root.render.DebugSprite(normalRT, { 230.0f, 10.0f }, 100.0f);
		//root.render.DebugSprite(selfilumRT, { 340.0f, 10.0f }, 100.0f);

		//Sprite::Draw(shadowRT, COLOR_WHITE_A(0.5f), Math::Matrix(), 0.0f, 100.0f, 0.0f, 1.0f, shadowRenderTech);

		hackStateEnabled = false;
	}

	void DefferedLight::Release()
	{
		gbufferTech.ReleaseRef();
		SceneEntity::Release();
	}
}