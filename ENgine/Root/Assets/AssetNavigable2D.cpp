#include "Root/Root.h"
#include "AssetNavigable2D.h"

#ifdef OAK_EDITOR
#include "Editor/EditorDrawer.h"
#endif

namespace Oak
{
	void AssetNavigable2D::Draw(float dt)
	{
		editorDrawer.SetCameraMatrices(camPos, root.render.GetDevice()->GetHeight() / camZoom, root.render.GetDevice()->GetAspect());

		editorDrawer.DrawCheckerBoard(camPos, Math::Vector2((float)root.render.GetDevice()->GetWidth(), (float)root.render.GetDevice()->GetHeight()), camZoom);
	}

	void AssetNavigable2D::OnMouseMove(Math::Vector2 ms)
	{
		mousePos = camPos + Math::Vector2(prevMs.x - root.render.GetDevice()->GetWidth() * 0.5f, -prevMs.y + root.render.GetDevice()->GetHeight() * 0.5f) / camZoom;

		Math::Vector2 delta(prevMs.x - ms.x, ms.y - prevMs.y);
		prevMs = ms;

		delta /= camZoom;

		if (dargField)
		{
			camPos += delta;
		}
	}

	void AssetNavigable2D::OnMiddleMouseDown()
	{
		dargField = true;
	}

	void AssetNavigable2D::OnMiddleMouseUp()
	{
		dargField = false;
	}
};