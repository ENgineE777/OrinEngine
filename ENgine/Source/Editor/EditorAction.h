#pragma once

namespace Oak
{
	class IEditorAction
	{
		friend class Editor;

	protected:
		void* owner = nullptr;
	public:

		IEditorAction(void* setOwner)
		{
			owner = setOwner;
		}

		virtual void Apply() = 0;
		virtual void Undo() = 0;
		virtual void Release()
		{
			delete this;
		}
	};
}
