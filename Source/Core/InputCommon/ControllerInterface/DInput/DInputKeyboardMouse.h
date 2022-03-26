// Copyright 2010 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#pragma once

#include <windows.h>

#include "DolphinWX/Frame.h"

#include "InputCommon/ControllerInterface/DInput/DInput8.h"
#include "InputCommon/ControllerInterface/Device.h"

namespace ciface
{
namespace DInput
{
extern void InitKeyboardMouse(IDirectInput8* const idi8, HWND _hwnd);
extern void Pass_Main_Frame_to_Keyboard_and_Mouse(CFrame *_main_frame);
static CFrame* main_frame = nullptr;

extern void Save_Keyboard_and_Mouse_Settings();
extern void Load_Keyboard_and_Mouse_Settings();

extern double cursor_sensitivity; // 2 for full screen mapping
extern unsigned char center_mouse_key;

class KeyboardMouse : public Core::Device
{
private:
	struct State
	{
		BYTE keyboard[256];
		DIMOUSESTATE2 mouse;
		struct
		{
			ControlState x, y;
		} cursor;
	};

	class Key : public Input
	{
	public:
		Key(u8 index, const BYTE& key) : m_key(key), m_index(index) {}
		std::string GetName() const override;
		ControlState GetState() const override;

	private:
		const BYTE& m_key;
		const u8 m_index;
	};

	class Button : public Input
	{
	public:
		Button(u8 index, const BYTE& button) : m_button(button), m_index(index) {}
		std::string GetName() const override;
		ControlState GetState() const override;

	private:
		const BYTE& m_button;
		const u8 m_index;
	};

	class Axis : public Input
	{
	public:
		Axis(u8 index, const LONG& axis, LONG range) : m_axis(axis), m_range(range), m_index(index) {}
		std::string GetName() const override;
		ControlState GetState() const override;

	private:
		const LONG& m_axis;
		const LONG m_range;
		const u8 m_index;
	};

	class Cursor : public Input
	{
	public:
		Cursor(u8 index, const ControlState& axis, const bool positive)
			: m_axis(axis), m_index(index), m_positive(positive)
		{
		}
		std::string GetName() const override;
		bool IsDetectable() override { return false; }
		ControlState GetState() const override;

	private:
		const ControlState& m_axis;
		const u8 m_index;
		const bool m_positive;
	};

public:
	void UpdateInput() override;

	KeyboardMouse(const LPDIRECTINPUTDEVICE8 kb_device, const LPDIRECTINPUTDEVICE8 mo_device);
	~KeyboardMouse();

	std::string GetName() const override;
	std::string GetSource() const override;

private:
	const LPDIRECTINPUTDEVICE8 m_kb_device;
	const LPDIRECTINPUTDEVICE8 m_mo_device;

	DWORD m_last_update;
	State m_state_in;

	template <typename T> T clamp(const T &n, const T &lower, const T &upper){return std::max(lower, std::min(n, upper));}
	void KeyboardMouse::GetMousePos(ControlState *const x, ControlState *const y);

	State current_state;
	bool player_requested_mouse_center = false;
	const double screen_height = static_cast<double>(GetSystemMetrics(SM_CYFULLSCREEN));
	const double screen_width =
	    static_cast<double>(GetSystemMetrics(SM_CXFULLSCREEN)); // there may be a better cast, i've not used them much
	const double screen_ratio = screen_width / screen_height;
	const POINT center_of_screen = POINT{static_cast<long>(screen_width / 2.0), static_cast<long>(screen_height / 2.0)};
	HWND active_window;

	// Sage 3/7/2022: Sensitivity has to be .51 or above in order to work
	//				  1.0 was equivalent before I modified the sens
	// Sage 3/25/2022: This is const because there is no way for the user to change this value.
	//				  The user may be able to change this value in the future, so it stays here for that case.
	const double inverse_axis_sensitivity = 0.85;
};
}
}
