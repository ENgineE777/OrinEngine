
#pragma once

#include "Support/Defines.h"


namespace Oak
{
	enum class CLASS_DECLSPEC AliasAction
	{
		Pressed /*!< Hardware button in pressed state */,
		JustPressed /*!< Harwdware button was just pressed */
	};

	/**
	\ingroup gr_code_root_controls
	*/

	/**
	\brief Controls

	This service class allows to read user input. System based on work with alliases. That means
	no hardcoded keys. User will works with alliases that can be accosiated with harwdare keys. This
	aproach allow to elimanate harcoded names of a hardware keys.
	Alises store in JSON file and can be loaded via call of Controls::Load.
	For debug porposes there are methods DebugKeyPressed and DebugHotKeyPressed for direct for with
	names of hardware keys.
	Servics also allows to work with differen inpunt devices like keybord, mouse and gamepad.
	All list of supported hardware kes can be found in files settings/controls/hardware_win and
	settings/controls/hardware_mobile.

	Sample of declaration of alias

	\code
	"name" : "Character.MOVE_VERT", // name of alias, index of alias shoudl be obtained via call Controls::GetAlias
	"AliasesRef" : [
		{ "names" : ["KEY_W"], "modifier" : 1.0 }, //assoiate key 'W' with alias
		{ "names" : ["KEY_S"], "modifier" : -1.0 }, //assoiate key 'S' with alias, multiplier -1.0f should aplyed to returned value via call Controls::GetAliasValue
		{ "names" : ["JOY_LEFT_STICK_V"] }  //assoiate vertical movement of left stick of gamepad
	]
	\endcode

	Sample of declaration of alias which uses combination of keys.

	\code
	"name" : "Character.GRAB", // name of alias, index of alias shoudl be obtained via call Controls::GetAlias
	"AliasesRef" : [
		{ "names" : ["KEY_W", "KEY_F"], //assoiate combination of keys 'W' and 'F' with alias
	]
	\endcode

	*/

	class CLASS_DECLSPEC IControls
	{
	public:

		/**
			\brief Load set of aliases for a file.

			\param[in] aliases Full path to a file with alias

			\return Returns True if aliases was successfully loade. Otherwise false will be returned.
		*/
		virtual bool LoadAliases(const char* aliases) = 0;

		/**
			\brief Get id of alias

			\param[in] name Name of a alias

			\return Id of alias. -1 will be returned in case alias doesn't exist.

		*/
		virtual int GetAlias(const char* name) = 0;

		/**
			\brief Get state of a alias.

			\param[in] alias Id of a alias
			\param[in] action State of a alias.

			\return Returns true in case alias in requsted state. Otherwise false will be returned.
		*/
		virtual bool GetAliasState(int alias, AliasAction action = AliasAction::JustPressed) = 0;

		/**
			\brief Get value of a alias. Keys are returning ether 0 or 1. Sticks of gamepad can have value from -1.0f to 1.0f

			\param[in] alias Id of a alias
			\param[in] delta Indicates if should returned falue or difference this last frame

			\return Value of associated hardware keys

		*/
		virtual float GetAliasValue(int alias, bool delta) = 0;

		/**
			\brief Get name of cuurentlu active hardware alias. This method needed for redifining controls via UI of a game

			\param[out] device_index Internal index of hardware device

			\return Name of hardware key

		*/
		virtual const char* GetActivatedKey(int& device_index) = 0;

		/**
			\brief Check state of hardware alias

			\param[in] name Name of a hardware alias
			\param[in] action State of a alias.
			\param[in] ignore_focus Input are reciving only in focused state. This flag allows to recive stete even in unfocused state.

			\return State of hardwrae alias.
		*/
		virtual bool DebugKeyPressed(const char* name, AliasAction action = AliasAction::JustPressed, bool ignore_focus = false) = 0;

		/**
			\brief Check state of up to 3 hardware alias at same time. This allows to check hot kee constoted from severel keys.

			\param[in] name Name of a first hardware alias
			\param[in] name2 Name of a second hardware alias
			\param[in] name3 Name of a third hardware alias

			\return State of combination hardare aliases.
		*/
		virtual bool DebugHotKeyPressed(const char* name, const char* name2, const char* name3 = nullptr) = 0;

		/**
			\brief Get value of hardware alias

			\param[in] name Name of a first hardware alias
			\param[in] delta Indicates if should returned falue or difference this last frame
			\param[in] ignore_focus Input are reciving only in focused state. This flag allows to recive stete even in unfocused state.

			\return Value of combination hardare aliases.
		*/
		virtual float DebugKeyValue(const char* name, bool delta, bool ignoreFocus) = 0;
		/**
			\brief Check if gamepad is connected.

			\return True will be returned if at least one gamepad is connecteds. False will be returned if non og gamepads are connected.
		*/
		virtual bool IsGamepadConnected() = 0;
	};
}