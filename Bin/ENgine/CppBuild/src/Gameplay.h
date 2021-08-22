#pragma once

#include "Root/Scenes/SceneEntity.h"

extern "C" CLASS_DECLSPEC void register_code(eastl::vector<Oak::ClassFactorySceneEntity*>& Decls);
extern "C" CLASS_DECLSPEC void recreate_entites(eastl::vector<Oak::SceneEntity*>& entities);
extern "C" CLASS_DECLSPEC void unregister_code(eastl::vector<Oak::ClassFactorySceneEntity*>& Decls);