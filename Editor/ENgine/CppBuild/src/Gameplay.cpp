#include "Gameplay.h"

#include "EASTL/algorithm.h"

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
	const size_t wasSize = Decls.size();

	auto& localDecls = Oak::ClassFactorySceneEntity::Decls();

	Decls.resize(wasSize + localDecls.size());
	eastl::copy(localDecls.begin(), localDecls.end(), Decls.begin() + wasSize);
}

void gather_copies(eastl::vector<Oak::ClassFactorySceneEntity*>& localDecls, 
				   eastl::vector<Oak::SceneEntity*>& entities,
				   eastl::vector<Oak::SceneEntity*>& toDelete, eastl::vector<Oak::SceneEntity*>& copies)
{
	for (int i = 0; i < entities.size(); i++)
	{
		Oak::SceneEntity* entity = entities[i];

		gather_copies(localDecls, entity->GetChilds(), toDelete, copies);

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
			Oak::SceneEntity* copy = entity->GetScene()->CreateEntity(entity->className, entity->prefabInstance);

			auto* parent = entity->GetParent();

			if (parent)
			{
				copy->SetSelfAsChild(parent, i);
			}
			else
			{
				entities[i] = copy;
			}

			copy->Copy(entity);
			copy->SetName(entity->GetName());
			copy->SetUID(entity->GetUID());
			copy->UpdateVisibility();

			auto references = entity->referenced;
			entity->referenced.clear();

			for (auto* ref : references)
			{
				ref->SetEntity(copy);
			}

			auto childs = entity->GetChilds();

			for (auto child : childs)
			{
				child->SetParent(copy);
			}

			toDelete.push_back(entity);
			copies.push_back(copy);
		}
	}
}

void recreate_entites(eastl::vector<Oak::SceneEntity*>& entities)
{
	auto& localDecls = Oak::ClassFactorySceneEntity::Decls();

	eastl::vector<Oak::SceneEntity*> toDelete;
	eastl::vector<Oak::SceneEntity*> copies;

	gather_copies(localDecls, entities, toDelete, copies);

	for (auto copy : copies)
	{
		copy->PostLoad();
	}

	for (auto entity : toDelete)
	{
		entity->Release();
	}
}

void unregister_code(eastl::vector<Oak::ClassFactorySceneEntity*>& Decls)
{
	auto& localDecls = Oak::ClassFactorySceneEntity::Decls();

	Decls.erase(eastl::remove_if(Decls.begin(), Decls.end(), [&](Oak::ClassFactorySceneEntity *decl)
	{
		return eastl::find(localDecls.begin(), localDecls.end(), decl) != localDecls.end();
	}),	Decls.end());
}