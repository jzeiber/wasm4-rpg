#include "input.h"
#include "wasm4.h"

Input::Input():m_lastmousex(*MOUSE_X),m_lastmousey(*MOUSE_Y),m_lastmousebuttons(*MOUSE_BUTTONS),
				m_mousex(*MOUSE_X),m_mousey(*MOUSE_Y),m_mousebuttons(*MOUSE_BUTTONS)
{
	m_gamepad[0]=*GAMEPAD1;
	m_gamepad[1]=*GAMEPAD2;
	m_gamepad[2]=*GAMEPAD3;
	m_gamepad[3]=*GAMEPAD4;
	for(int i=0; i<4; i++)
	{
		m_lastgamepad[i]=m_gamepad[i];
	}
}

Input::~Input()
{

}
	
Input &Input::Instance()
{
	static Input i;
	return i;
}

void Input::Update()
{
	m_lastmousex=m_mousex;
	m_lastmousey=m_mousey;
	m_lastmousebuttons=m_mousebuttons;
	for(int i=0; i<4; i++)
	{
		m_lastgamepad[i]=m_gamepad[i];
	}
	
	m_mousex=*MOUSE_X;
	m_mousey=*MOUSE_Y;
	m_mousebuttons=*MOUSE_BUTTONS;
	m_gamepad[0]=*GAMEPAD1;
	m_gamepad[1]=*GAMEPAD2;
	m_gamepad[2]=*GAMEPAD3;
	m_gamepad[3]=*GAMEPAD4;
}

int16_t Input::MouseX() const
{
	return m_mousex;
}

int16_t Input::MouseY() const
{
	return m_mousey;
}

bool Input::MouseButtonDown(const uint8_t button) const
{
	return ((m_mousebuttons >> (button-1)) & 0x1) == 0x1;
}

bool Input::MouseButtonClick(const uint8_t button) const
{
	return (((m_mousebuttons >> (button-1)) & 0x1) == 0x1) && !(((m_lastmousebuttons >> (button-1)) & 0x1) == 0x1);
}

bool Input::GamepadButtonDown(const uint8_t gamepad, const uint8_t button) const
{
	if(gamepad<1 || gamepad>4)
	{
		return false;
	}
	return (m_gamepad[gamepad-1] & button)==button;
}

bool Input::GamepadButtonPress(const uint8_t gamepad, const uint8_t button) const
{
	if(gamepad<1 || gamepad>4)
	{
		return false;
	}
	return (m_gamepad[gamepad-1] & button)==button && !((m_lastgamepad[gamepad-1] & button)==button);
}

void Input::ResetLastInput()
{
	m_lastmousex=m_mousex;
	m_lastmousey=m_mousey;
	m_lastmousebuttons=m_mousebuttons;
	for(int i=0; i<4; i++)
	{
		m_lastgamepad[i]=m_gamepad[i];
	}
}
