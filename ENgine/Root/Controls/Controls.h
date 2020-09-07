
#pragma once

#include <cinttypes>
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/map.h>


#ifdef PLATFORM_WIN

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <xinput.h>

#endif

namespace Oak
{
	enum class AliasAction
	{
		Active,
		Activated
	};

	class Controls
	{
	public:

	private:

		enum class Device
		{
			Keyboard,
			Mouse,
			Joystick,
			Touch
		};

		enum Consts
		{
			TouchCount = 10
		};

		bool focused = true;

		eastl::vector<int> supressed_aliases;

	#ifdef PLATFORM_WIN
		uint8_t                 btns[256];

		LPDIRECTINPUT8          pDI;
		LPDIRECTINPUTDEVICE8    pKeyboard;
		LPDIRECTINPUTDEVICE8    pMouse;

		uint32_t                dwElements;
		byte                    diks[256];
		DIMOUSESTATE2           dims2;
		uint32_t                dwMsElements;
		byte                    ms_bts[10];
		int                     ms_x, ms_y, ms_z;
		int                     prev_ms_x, prev_ms_y, prev_ms_z;
		XINPUT_STATE            joy_prev_states[XUSER_MAX_COUNT];
		XINPUT_STATE            joy_states[XUSER_MAX_COUNT];
		bool                    joy_active[XUSER_MAX_COUNT];

		inline float GetJoyTrigerValue(float val)
		{
			return val / 255.0f;
		}

		inline float GetJoyStickValue(float val)
		{
			val = fmaxf(-1, (float)val / 32767);
			float deadzone = 0.05f;
			val = (abs(val) < deadzone ? 0 : (abs(val) - deadzone) * (val / abs(val)));

			return val /= 1.0f - deadzone;
		}
	#endif

		struct HardwareAlias
		{
			eastl::string name;
			Device device;
			int index;
			float value;
		};

		struct AliasRefState
		{
			eastl::string name;
			int         aliasIndex = -1;
			bool        refer2hardware = false;
			int         device_index = -1;
		};

		struct AliasRef
		{
			float       modifier = 1.0f;
			eastl::vector<AliasRefState> refs;
		};

		struct Alias
		{
			eastl::string name;
			bool visited = false;
			eastl::vector<AliasRef> aliasesRef;
		};

		bool allowDebugKeys = false;
		eastl::vector<HardwareAlias> haliases;
		eastl::map<eastl::string, int> debeugMap;

		eastl::vector<Alias> aliases;
		eastl::map<eastl::string, int> aliasesMap;

		void  ResolveAliases();
		void  CheckDeadEnds(Alias& alias);
		bool  GetHardwareAliasState(int alias, AliasAction action, int device_index, bool ignore_focus);
		float GetHardwareAliasValue(int alias, bool delta, int device_index, bool ignore_focus);

		struct TouchState
		{
			int state = 0;
			int x = 0;
			int y = 0;
			int prev_x = 0;
			int prev_y = 0;
		};

		TouchState buffered_touches[TouchCount];
		TouchState touches[TouchCount];

	public:

		struct AliasMappig
		{
			eastl::string name;
			int    alias = -1;

			struct BindName
			{
				int device_index = -1;
				eastl::string name;
			};

			eastl::vector<eastl::vector<BindName>> bindedNames;

			AliasMappig(const char* name);
			bool IsContainHAlias(const char* halias);
		};

		bool  Init(const char* haliases, bool allowDebugKeys);

		bool  LoadAliases(const char* aliases);

		int   GetAlias(const char* name);

		bool  GetAliasState(int alias, AliasAction action = AliasAction::Activated);

		float GetAliasValue(int alias, bool delta);

		const char* GetActivatedKey(int& device_index);

		bool  DebugKeyPressed(const char* name, AliasAction action = AliasAction::Activated, bool ignore_focus = false);

		bool  DebugHotKeyPressed(const char* name, const char* name2, const char* name3 = nullptr);

		bool IsGamepadConnected();

		void SupressAlias(int alias_index);

		void SetFocused(bool set_focused);

	#ifdef PLATFORM_WIN
		void OverrideMousePos(int mx, int my);
	#endif

		void TouchStart(int index, int x, int y);
		void TouchUpdate(int index, int x, int y);
		void TouchEnd(int index);

		void Update(float dt);

		void Release();
	};
}