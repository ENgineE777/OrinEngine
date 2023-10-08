
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
		virtual const char* GetVsName() { return "blur_vs.shd"; };
		virtual const char* GetPsName() { return "blur_ps.shd"; };

		virtual void ApplyStates()
		{
			GetRoot()->GetRender()->GetDevice()->SetDepthWriting(false);
			GetRoot()->GetRender()->GetDevice()->SetDepthTest(false);
		};
	};

	class BlurDownTechnique : public RenderTechnique
	{
	public:
		virtual const char* GetVsName() { return "blur_vs.shd"; };
		virtual const char* GetPsName() { return "blur_down_ps.shd"; };

		virtual void ApplyStates()
		{
			GetRoot()->GetRender()->GetDevice()->SetDepthWriting(false);
			GetRoot()->GetRender()->GetDevice()->SetDepthTest(false);
		};
	};

	class BlurUpTechnique : public RenderTechnique
	{
	public:
		virtual const char* GetVsName() { return "blur_vs.shd"; };
		virtual const char* GetPsName() { return "blur_up_ps.shd"; };

		virtual void ApplyStates()
		{
			GetRoot()->GetRender()->GetDevice()->SetDepthWriting(false);
			GetRoot()->GetRender()->GetDevice()->SetDepthTest(false);

			root.render.GetDevice()->SetAlphaBlend(true);
			root.render.GetDevice()->SetBlendFunc(BlendArg::ArgOne, BlendArg::ArgOne);
		};
	};


	META_DATA_DESC(DefferedLight::GlobalLight)
		COLOR_PROP(DefferedLight::GlobalLight, ambientColor, COLOR_WHITE, "Ambient", "AmbientColor")
		FLOAT_PROP(DefferedLight::GlobalLight, ambientIntensity, 1.0f, "Ambient", "AmbientIntensity", "")
	META_DATA_DESC_END()

	ENTITYREG(SceneEntity, DefferedLight, "2D/Lights", "DefferedLight")

	META_DATA_DESC(DefferedLight)
		BASE_SCENE_ENTITY_PROP(DefferedLight)
		
		FLOAT_PROP(DefferedLight, metallic, 0.25f, "Visual", "metallic", "metallic")
		BOOL_PROP(DefferedLight, useFilter, false, "Filter", "useFilter", "")

		ARRAY_PROP(DefferedLight, globalLights, GlobalLight, "Prop", "Ambinet")

	META_DATA_DESC_END()

	bool DefferedLight::hackStateEnabled = false;
	float DefferedLight::lightGroupDivider = 64.0f;
	RenderTechniqueRef DefferedLight::gbufferTech;

	void DefferedLight::Init()
	{
		transform.objectType = ObjectType::Object2D;
		transform.transformFlag = TransformFlag::MoveXYZ | TransformFlag::RotateZ | TransformFlag::ScaleX | TransformFlag::ScaleY;

		Tasks(true)->AddTask(-10, this, (Object::Delegate)&DefferedLight::SetRT);
		Tasks(true)->AddTask(6, this, (Object::Delegate)&DefferedLight::Draw);

		defferdLightTech = root.render.GetRenderTechnique<DefferdLightTechnique>(_FL_);

		blurRTech = GetRoot()->GetRender()->GetRenderTechnique<BlurTechnique>(_FL_);
		blurDownRTech = GetRoot()->GetRender()->GetRenderTechnique<BlurDownTechnique>(_FL_);
		blurUpRTech = GetRoot()->GetRender()->GetRenderTechnique<BlurUpTechnique>(_FL_);

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

			selfilumRTs.clear();

			int curWidth = screenWidth;
			int curHeight = screenHeight;

			while (curWidth > 4 && curHeight > 4)
			{
				auto selfilumRT = root.render.GetDevice()->CreateTexture(curWidth, curHeight, TextureFormat::FMT_R11G11B10_FLOAT, 1, true, TextureType::Tex2D, _FL_);
				selfilumRT->SetAdress(TextureAddress::Border);

				selfilumRTs.push_back(selfilumRT);

				curWidth = (int)(curWidth / 2);
				curHeight = (int)(curHeight / 2);
			}			
			
			occluderRT = root.render.GetDevice()->CreateTexture(512, 512, TextureFormat::FMT_A8R8G8B8, 1, true, TextureType::Tex2D, _FL_);
			occluderRT->SetAdress(TextureAddress::Clamp);
			occluderRT->SetFilters(TextureFilter::Point, TextureFilter::Point);

			shadowRT = root.render.GetDevice()->CreateTexture(360, MAX_LIGHTS, TextureFormat::FMT_R32_FLOAT, 1, true, TextureType::Tex2D, _FL_);
		}

		root.render.GetDevice()->SetRenderTarget(0, albedoRT);
		root.render.GetDevice()->SetRenderTarget(1, materialRT);
		root.render.GetDevice()->SetRenderTarget(2, normalRT);
		root.render.GetDevice()->SetRenderTarget(3, selfilumRTs[0]);

		root.render.GetDevice()->Clear(true, GetScene()->IsPlaying() ? COLOR_BLACK_A(0.0f) : Color(0.05f, 0.05f, 0.05f, 0.0f), true, 1.0f);
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

				if (lights.size() == MAX_LIGHTS - dirLights.size())
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
		for (int i = 0; i < selfilumRTs.size() - 1; i++)
		{			
			GetRoot()->GetRender()->GetDevice()->SetVertexDecl(vdecl);
			GetRoot()->GetRender()->GetDevice()->SetVertexBuffer(0, buffer);

			GetRoot()->GetRender()->GetDevice()->SetRenderTarget(0, selfilumRTs[i+1]);
			GetRoot()->GetRender()->GetDevice()->SetDepth(nullptr);
			
			GetRoot()->GetRender()->GetDevice()->SetRenderTechnique(blurDownRTech);

			blurDownRTech->SetTexture(ShaderType::Pixel, "rt", selfilumRTs[i]);

			Math::Vector4 texelSize;
			texelSize.x = 1.0f / (float)selfilumRTs[i]->GetWidth();
			texelSize.y = 1.0f / (float)selfilumRTs[i]->GetHeight();

			blurDownRTech->SetVector(ShaderType::Pixel, "texelSize", &texelSize, 1);

			GetRoot()->GetRender()->GetDevice()->Draw(PrimitiveTopology::TriangleStrip, 0, 2);
		}

		for (int i = (int)selfilumRTs.size() - 1; i > 0; i--)
		{
			GetRoot()->GetRender()->GetDevice()->SetVertexDecl(vdecl);
			GetRoot()->GetRender()->GetDevice()->SetVertexBuffer(0, buffer);

			GetRoot()->GetRender()->GetDevice()->SetRenderTarget(0, selfilumRTs[i - 1]);
			GetRoot()->GetRender()->GetDevice()->SetDepth(nullptr);

			GetRoot()->GetRender()->GetDevice()->SetRenderTechnique(blurUpRTech);

			blurUpRTech->SetTexture(ShaderType::Pixel, "rt", selfilumRTs[i]);

			Math::Vector4 texelSize;
			texelSize.x = 0.002f;

			blurUpRTech->SetVector(ShaderType::Pixel, "texelSize", &texelSize, 1);

			GetRoot()->GetRender()->GetDevice()->Draw(PrimitiveTopology::TriangleStrip, 0, 2);
		}
	}

	void DefferedLight::RenderLights()
	{
		Math::Vector3 camPos = Sprite::GetCamPos();
		int lightData = 9 + 4 * MAX_LIGHTS;
		eastl::vector<Math::Vector4> u_lights;
		u_lights.resize(lightData);

		u_lights[0] = { Sprite::GetPixelsHeight() / root.render.GetDevice()->GetAspect() * 0.5f, Sprite::GetPixelsHeight() * 0.5f, camPos.x, camPos.y };

		defferdLightTech->SetVector(ShaderType::Vertex, "desc", &u_lights[0], 1);

		u_lights[0] = { timer, useFilter ? 1.0f : 0.0f, 1.0f, 0.1f };
		u_lights[1] = { 0.6f, 1.0f, 0.0f, 1.0f };

		auto halfScreenSize = Sprite::GetHalfScreenSize();

		u_lights[2] = { halfScreenSize.x, halfScreenSize.y, camPos.x, camPos.y };

		for (int i = 0; i < 6; i++)
		{
			u_lights[3 + i] = (i < globalLights.size()) ? Math::Vector4(globalLights[i].ambientColor.r, globalLights[i].ambientColor.g, globalLights[i].ambientColor.b, globalLights[i].ambientIntensity) : 1.0f;
		}

		int index = 9;

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
			u_lights[index + 3] = { 0.0f, 0.0f, (float)light->lightGroup / lightGroupDivider, 0.0f };

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
			u_lights[index + 3] = { light->viewAngle * Math::Radian, light->lineWidth , (float)light->lightGroup / lightGroupDivider, 0.0f }; //arc, width

			index += 4;
		}

		int lightCount = (int)lights.size() + (int)dirLights.size();
		u_lights[1].w = (float)lightCount;

		defferdLightTech->SetVector(ShaderType::Pixel, "u_lights", &u_lights[0], lightData);

		u_lights[0].x = metallic;

		defferdLightTech->SetVector(ShaderType::Pixel, "params", &u_lights[0], 1);
		defferdLightTech->SetVector(ShaderType::Pixel, "g_rougness", &u_lights[0], lightData);
		
		defferdLightTech->SetTexture(ShaderType::Pixel, "materialMap", materialRT);
		defferdLightTech->SetTexture(ShaderType::Pixel, "normalsMap", normalRT);
		defferdLightTech->SetTexture(ShaderType::Pixel, "selfilumMap", selfilumRTs[0]);
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
		//root.render.DebugSprite(downSelfilumRT, { 230.0f, 10.0f }, 100.0f);
		//root.render.DebugSprite(selfilumRTs[0], { 340.0f, 10.0f }, 100.0f);		

		//Sprite::Draw(shadowRT, COLOR_WHITE_A(0.5f), Math::Matrix(), 0.0f, 100.0f, 0.0f, 1.0f, shadowRenderTech);

		hackStateEnabled = false;
	}

	void DefferedLight::Release()
	{
		gbufferTech.ReleaseRef();
		blurRTech.ReleaseRef();
		blurUpRTech.ReleaseRef();
		blurDownRTech.ReleaseRef();
		shadowCastTech.ReleaseRef();
		shadowRenderTech.ReleaseRef();

		SceneEntity::Release();
	}
}