
#pragma once

#include "Scene.h"

namespace Oak
{
	class CLASS_DECLSPEC ISceneManager : public Object
	{
	public:

		/** 
		\brief Load a scene

		\param[in] name Name of a scene (filename without extension)
		*/
		virtual void LoadScene(const char* name) = 0;

		/**
		\brief Set visibility for scene objects from scene groups in all loaded scenes

		\param[in] group Name of a group
		\param[in] state State which will be set for secene objects
		*/
		virtual void SetScenesGroupsVisibilty(const char* group, bool set) = 0;

		/**
		\brief Unload a scene

		\param[in] name Name of a scene (filename without extension)
		*/
		virtual void UnloadScene(const char* name) = 0;
	};
}