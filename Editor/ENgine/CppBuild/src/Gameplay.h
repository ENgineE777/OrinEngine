#pragma once

#include "Root/Scenes/SceneEntity.h"

extern "C" __declspec(dllexport) void register_code(eastl::vector<Orin::ClassFactorySceneEntity*>& Decls);
extern "C" __declspec(dllexport) void recreate_entites(eastl::vector<Orin::SceneEntity*>& entities);
extern "C" __declspec(dllexport) void unregister_code(eastl::vector<Orin::ClassFactorySceneEntity*>& Decls);