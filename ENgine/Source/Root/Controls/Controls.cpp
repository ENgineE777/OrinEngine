
#include "Root/Root.h"
#include "Support/StringUtils.h"
#include "Root/Files/JsonReader.h"

namespace Orin
{
	Controls::AliasMappig::AliasMappig(const char* name)
	{
		this->name = name;

		this->alias = root.controls.GetAlias(name);

		if (this->alias != -1)
		{
			Alias& alias = root.controls.aliases[this->alias];
			int count = (int)alias.aliasesRef.size();

			if (count)
			{
				bindedNames.resize(count);

				for (auto& bindedName : bindedNames)
				{
					int index = (int)(&bindedName - &bindedNames[0]);
					int bind_count = (int)alias.aliasesRef[index].refs.size();

					if (bind_count)
					{
						bindedName.resize(bind_count);

						for (auto& bndName : bindedName)
						{
							int bind_index = (int)(&bndName - &bindedName[0]);

							bndName.name = alias.aliasesRef[index].refs[bind_index].name;
							bndName.device_index = alias.aliasesRef[index].refs[bind_index].device_index;
						}
					}
				}
			}
		}
	}

	bool Controls::AliasMappig::IsContainHAlias(const char* halias)
	{
		for (auto bindedName : bindedNames)
		{
			for (auto bndName : bindedName)
			{
				if (StringUtils::IsEqual(bndName.name.c_str(), halias))
				{
					return true;
				}
			}
		}

		return false;
	}

	bool Controls::Init(const char* name_haliases, bool allowDebugKeys)
	{
		this->allowDebugKeys = allowDebugKeys;

	#ifdef PLATFORM_WINDOWS
		for (int i = 0; i< XUSER_MAX_COUNT; i++)
		{
			joy_active[i] = false;
		}

		HWND hwnd = 0;

		HRESULT res;

		res = DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&pDI, nullptr);

		if (res < 0)
		{
			OAK_ALERT("DirectInput8Create failed with error");
			return false;
		}

		res = pDI->CreateDevice(GUID_SysKeyboard, &pKeyboard, nullptr);

		if (res < 0)
		{
			OAK_ALERT("pDI->CreateDevice failed with error");
			return false;
		}

		res = pKeyboard->SetDataFormat(&c_dfDIKeyboard);

		if (res < 0)
		{
			OAK_ALERT("pKeyboard->SetDataFormat failed with error");
			return false;
		}

		if (hwnd != 0)
		{
			res = pKeyboard->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);

			if (res < 0)
			{
				OAK_ALERT("pKeyboard->SetCooperativeLevel failed with error");

				return false;
			}
		}

		DIPROPDWORD dipdw;

		dipdw.diph.dwSize = sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		dipdw.diph.dwObj = 0;
		dipdw.diph.dwHow = DIPH_DEVICE;
		dipdw.dwData = 16;

		res = pKeyboard->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);

		if (res < 0)
		{
			OAK_ALERT("pKeyboard->SetProperty failed with error");
			return false;
		}

		pKeyboard->Acquire();

		res = pDI->CreateDevice(GUID_SysMouse, &pMouse, nullptr);

		if (res < 0)
		{
			OAK_ALERT("pKeyboard->SetProperty failed with error");
			return false;
		}

		res = pMouse->SetDataFormat(&c_dfDIMouse2);
		
		if (res < 0)
		{
			OAK_ALERT("pMouse->SetDataFormat failed with error");
			return false;
		}

		if (hwnd != 0)
		{
			res = pMouse->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);

			if (res < 0)
			{
				OAK_ALERT("pMouse->SetCooperativeLevel failed with error");
				return false;
			}
		}

		dipdw.diph.dwSize = sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		dipdw.diph.dwObj = 0;
		dipdw.diph.dwHow = DIPH_DEVICE;
		dipdw.dwData = 16;

		res = pMouse->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);

		if (res < 0)
		{
			OAK_ALERT("pMouse->SetProperty failed with error");
			return false;
		}

		pMouse->Acquire();

		ms_x = ms_y = ms_z = 0 ;
		prev_ms_x = prev_ms_y = prev_ms_z = -1000;
	#endif

		JsonReader reader;

		if (reader.ParseFile(name_haliases))
		{
			while (reader.EnterBlock("keyboard"))
			{
				haliases.push_back(HardwareAlias());
				HardwareAlias& halias = haliases[haliases.size() - 1];

				halias.device = Device::Keyboard;
				reader.Read("name", halias.name);
				reader.Read("index", halias.index);

				debeugMap[halias.name] = (int)haliases.size() - 1;

				reader.LeaveBlock();
			}

			while (reader.EnterBlock("mouse"))
			{
				haliases.push_back(HardwareAlias());
				HardwareAlias& halias = haliases[(int)haliases.size() - 1];

				halias.device = Device::Mouse;
				reader.Read("name", halias.name);
				reader.Read("index", halias.index);

				debeugMap[halias.name] = (int)haliases.size() - 1;

				reader.LeaveBlock();
			}

			while (reader.EnterBlock("joystick"))
			{
				haliases.push_back(HardwareAlias());
				HardwareAlias& halias = haliases[(int)haliases.size() - 1];

				halias.device = Device::Joystick;
				reader.Read("name", halias.name);
				reader.Read("index", halias.index);

				debeugMap[halias.name] = (int)haliases.size() - 1;

				reader.LeaveBlock();
			}

			while (reader.EnterBlock("touch"))
			{
				haliases.push_back(HardwareAlias());
				HardwareAlias& halias = haliases[(int)haliases.size() - 1];

				halias.device = Device::Touch;
				reader.Read("name", halias.name);
				reader.Read("index", halias.index);

				debeugMap[halias.name] = (int)haliases.size() - 1;

				reader.LeaveBlock();
			}
		}
		else
		{
			OAK_ALERT("Controls: can't load name_haliases");
			return false;
		}

		return true;
	}

	bool Controls::LoadAliases(const char* name_aliases)
	{
		JsonReader reader;

		bool res = false;

		if (reader.ParseFile(name_aliases))
		{
			res = true;

			while (reader.EnterBlock("Aliases"))
			{
				eastl::string name;
				reader.Read("name", name);

				int index = GetAlias(name.c_str());

				Alias* alias;

				if (index == -1)
				{
					aliases.push_back(Alias());
					alias = &aliases.back();

					alias->name = name;
					aliasesMap[name] = (int)aliases.size() - 1;
				}
				else
				{
					alias = &aliases[index];
					alias->aliasesRef.clear();
				}

				while (reader.EnterBlock("AliasesRef"))
				{
					alias->aliasesRef.push_back(AliasRef());
					AliasRef& aliasRef = alias->aliasesRef.back();

					while (reader.EnterBlock("names"))
					{
						eastl::string name;

						if (reader.IsString("") && reader.Read("", name))
						{
							aliasRef.refs.push_back(AliasRefState());
							aliasRef.refs.back().name = name;
						}
						else
						{
							if (aliasRef.refs.size() != 0)
							{
								reader.Read("", aliasRef.refs.back().device_index);
							}
						}

						reader.LeaveBlock();
					}

					reader.Read("modifier", aliasRef.modifier);

					reader.LeaveBlock();
				}

				reader.LeaveBlock();
			}

			ResolveAliases();
		}

		return res;
	}

	void Controls::ResolveAliases()
	{
		for (auto&  alias : aliases)
		{
			for (auto&  aliasRef : alias.aliasesRef)
			{
				for (auto& ref : aliasRef.refs)
				{
					int index = GetAlias(ref.name.c_str());

					if (index != -1)
					{
						ref.aliasIndex = index;
						ref.refer2hardware = false;
					}
					else
					{
						for (int l = 0; l < (int)haliases.size(); l++)
						{
							if (StringUtils::IsEqual(haliases[l].name.c_str(), ref.name.c_str()))
							{
								ref.aliasIndex = l;
								ref.refer2hardware = true;
								break;
							}
						}
					}

					if (ref.aliasIndex == -1)
					{
						root.Log("Controls", "alias %s has invalid reference %s", alias.name.c_str(), ref.name.c_str());
					}
				}
			}
		}

		for (auto&  alias : aliases)
		{
			CheckDeadEnds(alias);
		}
	}

	void Controls::CheckDeadEnds(Alias& alias)
	{
		alias.visited = true;

		for (auto& aliasRef : alias.aliasesRef)
		{
			for (auto& ref : aliasRef.refs)
			{
				if (ref.aliasIndex != -1 && !ref.refer2hardware)
				{
					if (aliases[ref.aliasIndex].visited)
					{
						ref.aliasIndex = -1;
						printf("alias %s has circular reference %s", alias.name.c_str(), ref.name.c_str());
					}
					else
					{
						CheckDeadEnds(aliases[ref.aliasIndex]);
					}
				}
			}
		}

		alias.visited = false;
	}

	void Controls::SupressAlias(int alias_index)
	{
		supressed_aliases.push_back(alias_index);
	}

	void Controls::SetFocused(bool set_focused)
	{
		focused = set_focused;
	}

	int Controls::GetAlias(const char* name)
	{
		if (aliasesMap.find(name) == aliasesMap.end())
		{
			return -1;
		}

		return aliasesMap[name];
	}

	bool Controls::GetHardwareAliasState(int index, AliasAction action, int device_index, bool ignore_focus)
	{
		if (!ignore_focus && !focused)
		{
			return false;
		}

		HardwareAlias& halias = haliases[index];

		switch (halias.device)
		{ 
	#ifdef PLATFORM_WINDOWS
			case Device::Joystick:
			{
				if (halias.index<100 || halias.index > 109)
				{
					for (int i = 0; i < XUSER_MAX_COUNT; i++)
					{
						if (!joy_active[i])
						{
							continue;
						}

						bool res = false;

						if (device_index != -1 && device_index != i)
						{
							continue;
						}

						int index = i;

						if (action == AliasAction::JustPressed)
						{
							res = (!(joy_prev_states[index].Gamepad.wButtons & halias.index) && joy_states[index].Gamepad.wButtons & halias.index);
						}

						if (action == AliasAction::Pressed)
						{
							res = (joy_states[index].Gamepad.wButtons & halias.index) !=  0;
						}

						if (res)
						{
							return true;
						}
					}
				}
				else
				{
					float val = GetHardwareAliasValue(index, false, device_index, false);

					if (action == AliasAction::Pressed)
					{
						return val > 0.99f;
					}

					float prev_val = val - GetHardwareAliasValue(index, true, device_index, false);

					return (val > 0.99f) && (prev_val < 0.99f);
				}

				break;
			}
			case Device::Keyboard:
			{
				if (action == AliasAction::JustPressed)
				{
					return (btns[halias.index] == 1);
				}

				if (action == AliasAction::Pressed)
				{
					return (btns[halias.index] > 0);
				}
				break;
			}
			case Device::Mouse:
			{
				if (halias.index < 10)
				{
					if (action == AliasAction::JustPressed)
					{
						return (ms_bts[halias.index] == 1);
					}

					if (action == AliasAction::Pressed)
					{
						return (ms_bts[halias.index] > 0);
					}
				}
				break;
			}
	#endif
			case Device::Touch:
			{
				if (halias.index < 10)
				{
					if (action == AliasAction::JustPressed)
					{
						return (touches[halias.index].state == 1);
					}

					if (action == AliasAction::Pressed)
					{
						return (touches[halias.index].state > 0);
					}
				}
				break;
			}
		}

		return false;
	}

	bool Controls::GetAliasState(int index, AliasAction action)
	{
		for (auto alias : supressed_aliases)
		{
			if (alias == index)
			{
				return false;
			}
		}

		if (index == -1 || index >= (int)aliases.size())
		{
			return false;
		}

		Alias& alias = aliases[index];

		for (auto& aliasRef : alias.aliasesRef)
		{
			bool prepare_val = false;
			bool val = false;

			for (auto& ref : aliasRef.refs)
			{
				if (ref.aliasIndex == -1)
				{
					continue;
				}

				if (!prepare_val)
				{
					val = true;
					prepare_val = true;
				}

				if (ref.refer2hardware)
				{
					val &= GetHardwareAliasState(ref.aliasIndex, AliasAction::Pressed, ref.device_index, false);
				}
				else
				{
					val &= GetAliasState(ref.aliasIndex, AliasAction::Pressed);
				}
			}

			if (action == AliasAction::JustPressed && val)
			{
				val = false;

				for (auto& ref : aliasRef.refs)
				{
					if (ref.aliasIndex == -1)
					{
						continue;
					}

					if (ref.refer2hardware)
					{
						val |= GetHardwareAliasState(ref.aliasIndex, AliasAction::JustPressed, ref.device_index, false);
					}
					else
					{
						val |= GetAliasState(ref.aliasIndex, AliasAction::JustPressed);
					}
				}
			}

			if (val)
			{
				return true;
			}
		}

		return false;
	}

	float Controls::GetHardwareAliasValue(int index, bool delta, int device_index, bool ignore_focus)
	{
		if (!ignore_focus && !focused)
		{
			return 0.0f;
		}

		HardwareAlias& halias = haliases[index];

		switch (halias.device)
		{
	#ifdef PLATFORM_WINDOWS
			case Device::Joystick:
			{
				if (halias.index >= 100 && halias.index <= 109)
				{
					float val = 0.0f;

					for (int i = 0; i < XUSER_MAX_COUNT; i++)
					{
						if (!joy_active[i])
						{
							continue;
						}

						if (device_index != -1 && device_index != i)
						{
							continue;
						}

						int index = i;

						if (halias.index == 100 || halias.index == 101)
						{
							val = GetJoyStickValue((float)joy_states[index].Gamepad.sThumbLX);

							if (delta)
							{
								val = val - GetJoyStickValue((float)joy_prev_states[index].Gamepad.sThumbLX);
							}

							if (halias.index == 101)
							{
								val = -val;
							}
						}
						else
						if (halias.index == 102 || halias.index == 103)
						{
							val = GetJoyStickValue((float)joy_states[index].Gamepad.sThumbLY);

							if (delta)
							{
								val = val - GetJoyStickValue((float)joy_prev_states[index].Gamepad.sThumbLY);
							}

							if (halias.index == 103)
							{
								val = -val;
							}
						}
						else
						if (halias.index == 104)
						{
							val = GetJoyTrigerValue((float)joy_states[index].Gamepad.bLeftTrigger);

							if (delta)
							{
								val = val - GetJoyTrigerValue((float)joy_prev_states[index].Gamepad.bLeftTrigger);
							}
						}
						else
						if (halias.index == 105 || halias.index == 106)
						{
							val = GetJoyStickValue((float)joy_states[index].Gamepad.sThumbRX);

							if (delta)
							{
								val = val - GetJoyStickValue((float)joy_prev_states[index].Gamepad.sThumbRX);
							}

							if (halias.index == 106)
							{
								val = -val;
							}
						}
						else
						if (halias.index == 107 || halias.index == 108)
						{
							val = GetJoyStickValue((float)joy_states[index].Gamepad.sThumbRY);

							if (delta)
							{
								val = val - GetJoyStickValue((float)joy_prev_states[index].Gamepad.sThumbRY);
							}

							if (halias.index == 108)
							{
								val = -val;
							}
						}
						else
						if (halias.index == 109)
						{
							val = GetJoyTrigerValue((float)joy_states[index].Gamepad.bRightTrigger);

							if (delta)
							{
								val = val - GetJoyTrigerValue((float)joy_prev_states[index].Gamepad.bRightTrigger);
							}
						}

						if (fabs(val) > 0.01f)
						{
							break;
						}
					}

					return val;
				}
				else
				{
					return GetHardwareAliasState(index, AliasAction::Pressed, device_index, false) ? 1.0f : 0.0f;
				}

				break;
			}
			case Device::Keyboard:
			{
				if (btns[halias.index])
				{
					return 1.0f;
				}
				break;
			}
			case Device::Mouse:
			{
				if (halias.index < 10)
				{
					if (ms_bts[halias.index])
					{
						return 1.0f;
					}
				}

				if (halias.index == 10)
				{
					if (delta)
					{
						if (prev_ms_x == -1000)
						{
							return 0;
						}

						return (float)(ms_x - prev_ms_x);
					}

					return (float)ms_x;
				}
				else
				if (halias.index == 11)
				{
					if (delta)
					{
						if (prev_ms_y == -1000)
						{
							return 0;
						}

						return (float)(ms_y - prev_ms_y);
					}

					return (float)ms_y;
				}
				else
				if (halias.index == 12)
				{
					if (delta)
					{
						if (prev_ms_z == -1000)
						{
							return 0;
						}

						return (float)(ms_z - prev_ms_z);
					}

					return (float)ms_z;
				}
				break;
			}
	#endif
			case Device::Touch:
			{
				if (halias.index < 10)
				{
					if (touches[halias.index].state > 0)
					{
						return 1.0f;
					}
				}
				else
				if (halias.index < 20)
				{
					if (delta)
					{
						return (float)(touches[halias.index - 10].x - touches[halias.index - 10].prev_x);
					}

					return (float)touches[halias.index - 10].x;
				}
				else
				if (halias.index < 30)
				{
					if (delta)
					{
						return (float)(touches[halias.index - 20].y - touches[halias.index - 20].prev_y);
					}

					return (float)touches[halias.index - 20].y;
				}
				break;
			}
		}

		return 0.0f;
	}

	float Controls::GetAliasValue(int index, bool delta)
	{
		for (auto alias : supressed_aliases)
		{
			if (alias == index)
			{
				return false;
			}
		}

		if (index == -1 || index >= (int)aliases.size())
		{
			return 0.0f;
		}

		Alias& alias = aliases[index];

		float val = 0.0f;

		for (auto& aliasRef : alias.aliasesRef)
		{
			for (auto& ref : aliasRef.refs)
			{
				if (ref.aliasIndex == -1)
				{
					continue;
				}

				float refValue = 1.0;

				if (ref.refer2hardware)
				{
					refValue *= GetHardwareAliasValue(ref.aliasIndex, delta, ref.device_index, false);
				}
				else
				{
					refValue *= GetAliasValue(ref.aliasIndex, delta);
				}

				refValue *= aliasRef.modifier;

				val += refValue;

				if (ref.refer2hardware)
				{
					HardwareAlias& halias = haliases[ref.aliasIndex];
					
					if (halias.device == Device::Keyboard || halias.device == Device::Joystick)
					{
						val = Math::Clamp(val, -1.0f, 1.0f);
					}
				}
			}
		}

		return val;
	}

	const char* Controls::GetActivatedKey(int& device_index)
	{
		for (auto& halias : haliases)
		{
			int index = (int)(&halias - &haliases[0]);

			int count = 1;

	#ifdef PLATFORM_WINDOWS
			if (halias.device == Device::Joystick)
			{
				count = XUSER_MAX_COUNT;
			}
	#endif

			for (device_index = 0; device_index<count; device_index++)
			{
				if (GetHardwareAliasState(index, AliasAction::JustPressed, device_index, false))
				{
					return halias.name.c_str();
				}
			}
		}

		return nullptr;
	}

	bool Controls::DebugKeyPressed(const char* name, AliasAction action, bool ignore_focus)
	{
		if (!allowDebugKeys || !name)
		{
			return false;
		}

		if (debeugMap.find(name) == debeugMap.end())
		{
			return false;
		}

		return GetHardwareAliasState(debeugMap[name], action, 0, ignore_focus);
	}

	bool Controls::DebugHotKeyPressed(const char* name, const char* name2, const char* name3)
	{
		if (!allowDebugKeys)
		{
			return false;
		}

		bool active = DebugKeyPressed(name, AliasAction::Pressed) & DebugKeyPressed(name2, AliasAction::Pressed);

		if (name3)
		{
			active &= DebugKeyPressed(name3, AliasAction::Pressed);
		}

		if (active)
		{
			if (DebugKeyPressed(name) | DebugKeyPressed(name2) | DebugKeyPressed(name3))
			{
				return true;
			}
		}

		return false;
	}

	float Controls::DebugKeyValue(const char* name, bool delta, bool ignoreFocus)
	{
		if (!allowDebugKeys || !name)
		{
			return 0.0f;
		}

		if (debeugMap.find(name) == debeugMap.end())
		{
			return false;
		}

		return GetHardwareAliasValue(debeugMap[name], delta, 0, ignoreFocus);
	}

	bool Controls::IsGamepadConnected()
	{
	#ifdef PLATFORM_WINDOWS
		for (int i = 0; i < XUSER_MAX_COUNT; i++)
		{
			if (joy_active[i])
			{
				return true;
			}
		}
	#endif

		return false;
	}

	#ifdef PLATFORM_WINDOWS
	void Controls::OverrideMousePos(int mx, int my)
	{
		if (prev_ms_x == -1000)
		{
			prev_ms_x = mx;
			ms_x = mx;

			prev_ms_y = my;
			ms_y = my;
		}

		prev_ms_x = ms_x;
		ms_x = mx;

		prev_ms_y = ms_y;
		ms_y = my;
	}
	#endif

	void Controls::TouchStart(int index, int x, int y)
	{
		buffered_touches[index].state = 1;
		buffered_touches[index].x = x;
		buffered_touches[index].y = y;
	}

	void Controls::TouchUpdate(int index, int x, int y)
	{
		buffered_touches[index].x = x;
		buffered_touches[index].y = y;
	}

	void Controls::TouchEnd(int index)
	{
		buffered_touches[index].state = 0;
	}

	void Controls::Update(float dt)
	{
		for (int i = 0; i < TouchCount; i++)
		{
			if (buffered_touches[i].state == 1)
			{
				if (touches[i].state == 0)
				{
					touches[i].state = 1;
					touches[i].x = touches[i].prev_x = buffered_touches[i].x;
					touches[i].y = touches[i].prev_y = buffered_touches[i].y;
				}
				else
				{
					touches[i].state = 2;

					touches[i].prev_x = touches[i].x;
					touches[i].prev_y = touches[i].y;

					touches[i].x = buffered_touches[i].x;
					touches[i].y = buffered_touches[i].y;
				}
			}
			else
			{
				if (touches[i].state != 0)
				{
					touches[i].state = 0;
					touches[i].x = touches[i].y = touches[i].prev_x = touches[i].prev_y = 0;
				}
			}
		}

		supressed_aliases.clear();

	#ifdef PLATFORM_WINDOWS
		static byte tmp_diks[256];
		ZeroMemory(tmp_diks, sizeof(tmp_diks));
		HRESULT hr = pKeyboard->GetDeviceState(sizeof(tmp_diks), tmp_diks);

		for (int i = 0; i<256; i++)
		{
			if (tmp_diks[i])
			{
				btns[i]++;

				if (btns[i]>2)
				{
					btns[i] = 2;
				}
			}
			else
			{
				btns[i] = 0;
			}
		}

		if (FAILED(hr))
		{
			hr = pKeyboard->Acquire();

			return;
		}

		ZeroMemory(&dims2, sizeof(dims2));
		hr = pMouse->GetDeviceState(sizeof(DIMOUSESTATE2), &dims2);

		for (int i = 0; i<4; i++)
		{
			if (dims2.rgbButtons[i] & 0x80)
			{
				ms_bts[i]++;

				if (ms_bts[i]>2)
				{
					ms_bts[i] = 2;
				}
			}
			else
			{
				ms_bts[i] = 0;
			}
		}

		if (FAILED(hr))
		{
			hr = pMouse->Acquire();

			return;
		}

		prev_ms_x = ms_x;
		prev_ms_y = ms_y;

		prev_ms_z = ms_z;
		ms_z += dims2.lZ;

		for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
		{
			if (joy_active[i])
			{
				memcpy(&joy_prev_states[i], &joy_states[i], sizeof(XINPUT_STATE));
			}

			ZeroMemory(&joy_states[i], sizeof(XINPUT_STATE));

			if (XInputGetState(i, &joy_states[i]) == ERROR_SUCCESS)
			{ 
				if (!joy_active[i])
				{
					memcpy(&joy_prev_states[i], &joy_states[i], sizeof(XINPUT_STATE));
				}

				joy_active[i] = true;
			}
			else
			{
				joy_active[i] = false;
			}
		}
	#endif
	}

	void Controls::Release()
	{
		RELEASE(pKeyboard);
		RELEASE(pMouse);
		RELEASE(pDI);
	}
}