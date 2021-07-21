
#pragma once

#include "Root/Render/Device.h"
#include "Root/Render/Program.h"
#include "Root/TaskExecutor/TaskExecutor.h"
#include <eastl/vector.h>
#include <eastl/map.h>

namespace Oak
{
	/**
	\ingroup gr_code_root_render
	*/

	enum class CLASS_DECLSPEC ScreenCorner
	{
		LeftTop,
		RightTop,
		LeftBottom,
		RightBottom
	};

	/**
	\ingroup gr_code_root_render
	*/

	enum class CLASS_DECLSPEC TransformStage
	{
		World = 0 /*!< World trasformation */,
		View /*!< View trasformation */,
		Projection /*!< Projection trasformation */,
		WrldViewProj /*!< Final transformation */
	};

	/**
	\ingroup gr_code_root_render
	*/

	/**
	\brief Render

	This is main class of render system.

	*/

	class CLASS_DECLSPEC IRender
	{
	public:

		/**
		\brief Get pointer to a device interface

		\return Pointer to a device interface
		*/

		virtual Device* GetDevice() = 0;

		/**
		\brief Set transform

		\param[in] trans Type of transform
		\param[in] mat Trnasform itself
		*/
		virtual void SetTransform(TransformStage trans, Math::Matrix mat) = 0;

		/**
		\brief Get transform

		\param[in] trans Type of transform
		\param[out] mat Trnasform itself
		*/
		virtual void GetTransform(TransformStage trans, Math::Matrix& mat) = 0;

		/**
		\brief Get pointer to a program by name

		\param[in] name Name of a program
		\param[in] file Name of a file from which ctreation was requested
		\param[in] line Number of a line from which ctreation was requested

		\return Pointer to a program
		*/
		virtual ProgramRef GetProgram(const char* name, const char* file, int line) = 0;

		/**
		\brief Load texture

		\param[in] name Full path to a texture
		\param[in] file Name of a file from which ctreation was requested
		\param[in] line Number of a line from which ctreation was requested

		\return Pointer to a texture
		*/
		virtual TextureRef LoadTexture(const char* name, const char* file, int line) = 0;

		/**
		\brief Load texture

			\param[in] texture Texture reference
			\param[in] name Full path to a texture
		*/
		virtual void LoadTexture(TextureRef& texture, const char* name) = 0;

		/**
		\brief Creates new task pool in group render task pool
		\param[in] file Name of a file from which ctreation was requested
		\param[in] line Number of a line from which ctreation was requested

		\return Pointer to a task pool
		*/
		virtual TaskExecutor::SingleTaskPool* AddTaskPool(const char* file, int line) = 0;

		/**
		\brief Deletes task pool from a group render task pool

		\param[in] pool Pointer to a task pool
		*/
		virtual void DelTaskPool(TaskExecutor::SingleTaskPool* pool) = 0;

		/**
		\brief Add execution level in group task pool

		\param[in] level Level of execution
		*/
		virtual void AddExecutedLevelPool(int level) = 0;

		/**
		\brief Execute all tasks in group task pool with particula level of execution

		\param[in] level Level of execution
		\param[in] dt Deletaime since last frame
		*/
		virtual void ExecutePool(int level, float dt) = 0;

		/**
		\brief Draw debug line in 3D space

		\param[in] from Position of first point
		\param[in] from_clr Color of first color
		\param[in] to Position of second point
		\param[in] to_clr Color of second point
		\param[in] use_depth Should depth be ignored

		*/
		virtual void DebugLine(Math::Vector3 from, Color from_clr, Math::Vector3 to, Color to_clr, bool use_depth = true) = 0;

		/**
		\brief Draw debug line in 2D space

		\param[in] from Position of first point
		\param[in] from_clr Color of first color
		\param[in] to Position of second point
		\param[in] to_clr Color of second point

		*/
		virtual void DebugLine2D(Math::Vector2 from, Color from_clr, Math::Vector2 to, Color to_clr) = 0;

		/**
		\brief Draw rectangle constided from lines in 2D space

		\param[in] from Left top point
		\param[in] to Right bottom point
		\param[in] color Color of a rectangle

		*/
		virtual void DebugRect2D(Math::Vector2 from, Math::Vector2 to, Color color) = 0;

		/**
		\brief Draw debug sphere in 3D space

		\param[in] pos Postion of a sphere
		\param[in] color Color of a sphere
		\param[in] radius Radius of a sphere
		\param[in] full_shade Render with ligtning

		*/
		virtual void DebugSphere(Math::Vector3 pos, Color color, float radius, bool full_shade = true) = 0;

		/**
		\brief Draw debug box in 3d space

		\param[in] pos Transformation of a box
		\param[in] color Color of a box
		\param[in] scale Size of a box

		*/
		virtual void DebugBox(Math::Matrix transform, Color color, Math::Vector3 scale) = 0;

		/**
		\brief Draw debug triangle in 3D space

		\param[in] p1 First point of a triangle
		\param[in] p2 Second point of a triangle
		\param[in] p3 Third point of a triangle
		\param[in] color Color of a trinagle

		*/
		virtual void DebugTriangle(Math::Vector3 p1, Math::Vector3 p2, Math::Vector3 p3, Color color) = 0;

		/**
		\brief Draw debug triangle in 2D space

		\param[in] p1 First point of a triangle
		\param[in] p2 Second point of a triangle
		\param[in] p3 Third point of a triangle
		\param[in] color Color of a trinagle

		*/
		virtual void DebugTriangle2D(Math::Vector2 p1, Math::Vector2 p2, Math::Vector2 p3, Color color) = 0;

		/**
		\brief Print debug text in 2D space

		\param[in] pos Postion of a text in screen space
		\param[in] corner Corner aligment
		\param[in] color Color of a text
		\param[in] text Text format string of a text

		*/
		virtual void DebugPrintText(Math::Vector2 pos, ScreenCorner corner, Color color, const char* text, ...) = 0;

		/**
		\brief Print debug text in 3D space

		\param[in] pos Postion of a text
		\param[in] dist Distance to a camera in which text is visible 
		\param[in] color Color of a text
		\param[in] text Text format string of a text

		*/
		virtual void DebugPrintText(Math::Vector3 pos, float dist, Color color, const char* text, ...) = 0;

		/**
		\brief Draw debug sprite

		\param[in] texture Texture of a sprite
		\param[in] pos Postion of a sprite
		\param[in] size Size of a sprite
		\param[in] color Color of a sprite
		\param[in] offset Offset of local origin
		\param[in] angle Rotation around local origin

		*/
		virtual void DebugSprite(Texture* texture, Math::Vector2 pos, Math::Vector2 size, Color color = COLOR_WHITE, Math::Vector2 offset = 0.0f, float angle = 0.0f) = 0;

		/**
		\brief Get pointer to texture filled by white color

		\return Pointer to texture filled by white color
		*/
		virtual TextureRef GetWhiteTexture() = 0;
		
		/**
		\brief Transform pount to a screen

		\param[in] pos Position of a point
		\param[in] type Type of transform. 0 - transform in post camera space in range -1.0f .. 1.0f. 1 - transform in post camera space in range 0 .. 1.0f. 2 - transform in post camera space in screen coordinates

		\return Result of transformation
		*/
		virtual Math::Vector3 TransformToScreen(Math::Vector3 pos, int type) = 0;
	};
}