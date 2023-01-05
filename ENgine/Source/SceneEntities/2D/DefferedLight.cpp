
#include "DefferedLight.h"
#include "PointLight2D.h"
#include "Root/Root.h"

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


	ENTITYREG(SceneEntity, DefferedLight, "2D", "DefferedLight")

	META_DATA_DESC(DefferedLight)
		BASE_SCENE_ENTITY_PROP(DefferedLight)
	META_DATA_DESC_END()

	void DefferedLight::Init()
	{
		transform.objectType = ObjectType::Object2D;
		transform.transformFlag = TransformFlag::MoveXYZ | TransformFlag::RotateZ | TransformFlag::ScaleX | TransformFlag::ScaleY;

		Tasks(true)->AddTask(0, this, (Object::Delegate)&DefferedLight::Draw);

		ground.albedo = root.render.LoadTexture("PBR\\wall.png", _FL_);
		ground.material = root.render.LoadTexture("PBR\\concrete_m.png", _FL_);
		ground.normal = root.render.LoadTexture("PBR\\wall_n.png", _FL_);

		box.albedo = root.render.LoadTexture("PBR\\crate.png", _FL_);
		box.material = root.render.LoadTexture("PBR\\crate_m.png", _FL_);
		box.normal = root.render.LoadTexture("PBR\\crate_n.png", _FL_);

		gbufferTech = root.render.GetRenderTechnique<GBufferTechnique>(_FL_);
		defferdLightTech = root.render.GetRenderTechnique<DefferdLightTechnique>(_FL_);
	}

	void DefferedLight::SetRT(float dt)
	{
		int screenWidth = root.render.GetDevice()->GetWidth();
		int screenHeight = root.render.GetDevice()->GetHeight();;

		if (!albedoRT || albedoRT->GetWidth() != screenWidth || albedoRT->GetHeight() != screenHeight)
		{
			albedoRT = root.render.GetDevice()->CreateTexture(screenWidth, screenHeight, TextureFormat::FMT_A8R8G8B8, 1, true, TextureType::Tex2D, _FL_);
			materialRT = root.render.GetDevice()->CreateTexture(screenWidth, screenHeight, TextureFormat::FMT_A8R8G8B8, 1, true, TextureType::Tex2D, _FL_);
			normalRT = root.render.GetDevice()->CreateTexture(screenWidth, screenHeight, TextureFormat::FMT_A8R8G8B8, 1, true, TextureType::Tex2D, _FL_);

			sceneDepth = root.render.GetDevice()->CreateTexture(screenWidth, screenHeight, TextureFormat::FMT_D24, 1, true, TextureType::Tex2D, _FL_);
		}

		root.render.GetDevice()->SetRenderTarget(0, albedoRT);
		root.render.GetDevice()->SetRenderTarget(1, materialRT);
		root.render.GetDevice()->SetRenderTarget(2, normalRT);

		root.render.GetDevice()->SetDepth(sceneDepth);

		root.render.GetDevice()->Clear(true, COLOR_BLACK, true, 1.0f);
	}

	void DefferedLight::Draw(float dt)
	{
		SetRT(dt);

		gbufferTech->SetTexture(ShaderType::Pixel, "materialMap", ground.material);
		gbufferTech->SetTexture(ShaderType::Pixel, "normalsMap", ground.normal);

		Math::Matrix mat;
		Sprite::Draw(ground.albedo, COLOR_WHITE, mat, 0.0f, { 600.0f, 600.0f }, 0.0f, 4.0f, gbufferTech);

		gbufferTech->SetTexture(ShaderType::Pixel, "materialMap", box.material);
		gbufferTech->SetTexture(ShaderType::Pixel, "normalsMap", box.normal);

		mat.Pos().z -= 0.001f;
		Sprite::Draw(box.albedo, COLOR_WHITE, mat, { 220.0f, -220.f}, { 75.0f, 75.0f }, 0.0f, 1.0f, gbufferTech);

		root.render.GetDevice()->RestoreRenderTarget();

		root.render.DebugSprite(albedoRT, 10.0f, 100.0f);
		root.render.DebugSprite(materialRT, {120.0f, 10.0f}, 100.0f);
		root.render.DebugSprite(normalRT, { 230.0f, 10.0f }, 100.0f);

		// amb_intesity - 0.05f
		// amb_color - BAFFE6
		// 

		Math::Vector3 camPos = Sprite::GetCamPos();

		Math::Vector4 u_lights[3 + 4 * 16];

		defferdLightTech->SetTexture(ShaderType::Pixel, "materialMap", materialRT);
		defferdLightTech->SetTexture(ShaderType::Pixel, "normalsMap", normalRT);

		u_lights[0] = { Sprite::GetPixelsHeight() / root.render.GetDevice()->GetAspect() * 0.5f, Sprite::GetPixelsHeight() * 0.5f, camPos.x, camPos.y};

		defferdLightTech->SetVector(ShaderType::Vertex, "desc", u_lights, 1);

		u_lights[0] = { 0.72f, 1.0f, 1.0f, 0.1f };
		u_lights[1] = { 0.6f, 1.0f, 0.0f, 1.0f };
		u_lights[2] = { Sprite::GetPixelsHeight() / root.render.GetDevice()->GetAspect() * 0.5f, Sprite::GetPixelsHeight() * 0.5f, camPos.x, camPos.y };

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

				u_lights[index + 0] = { trans.position.x, trans.position.y, 100.0f, 1.0f }; //pos, dir
				u_lights[index + 1] = { light->color.r, light->color.g, light->color.b, light->intesity }; //color, intesity		
				u_lights[index + 2] = { 1.0f, light->falloff, trans.rotation.z * Math::Radian, trans.size.x * 0.5f }; //light_depth, falloff, angle, radius
				u_lights[index + 3] = { light->viewAngle * Math::Radian, light->lineWidth , 0.0f, 0.0f }; //arc, width

				index += 4;
				lightCount++;
			}
		}
		
		u_lights[1].w = lightCount;		

		defferdLightTech->SetVector(ShaderType::Pixel, "u_lights", u_lights, 3 + 4 * lightCount);

		Sprite::Draw(albedoRT, COLOR_WHITE, mat, 0.0f, 100.0f, 0.0f, 1.0f, defferdLightTech);
	}
}