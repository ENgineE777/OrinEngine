#include "Gameplay.h"

void* operator new (size_t size, const char* file, int line)
{
	return malloc(size);
}

void operator delete(void* ptr, const char* file, int line)
{
	free(ptr);
}

void register_code(eastl::vector<Oak::ClassFactorySceneEntity*>& Decls)
{
	auto& localDecls = Oak::ClassFactorySceneEntity::Decls();

	for(auto decl : localDecls)
	{
		Decls.push_back(decl);
	}
}

void recreate_entites(eastl::vector<Oak::SceneEntity*>& entities)
{
	auto& localDecls = Oak::ClassFactorySceneEntity::Decls();

	for (int i = 0; i < entities.size(); i++)
	{
		Oak::SceneEntity* entity = entities[i];

		bool needRecreate = false;

		for (auto decl : localDecls)
		{
			if (_stricmp(entity->className, decl->GetShortName()) == 0)
			{
				needRecreate = true;
				break;
			}
		}

		if (needRecreate)
		{
			Oak::SceneEntity* copy = entity->GetScene()->CreateEntity(entity->className);

			auto* parent = entity->GetParent();

			if (parent)
			{
				copy->SetParent(parent, nullptr);
			}
			else
			{
				entities[i] = copy;
			}

			copy->Copy(entity);
			copy->SetName(entity->GetName());
			copy->SetUID(entity->GetUID());

			entity->Release();
		}
	}
}

void unregister_code(eastl::vector<Oak::ClassFactorySceneEntity*>& Decls)
{
	auto& localDecls = Oak::ClassFactorySceneEntity::Decls();

	for (int i = 0; i < Decls.size(); i++)
	{
		bool needDelete = false;

		for (auto decl : localDecls)
		{
			if (Decls[i] == decl)
			{
				needDelete = true;
				break;
			}
		}

		if (needDelete)
		{
			Decls.erase(Decls.begin() + i);
			i--;
		}
	}
}