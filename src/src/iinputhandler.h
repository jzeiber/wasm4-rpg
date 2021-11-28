#pragma once

#include "input.h"

class IInputHandler
{
public:
	IInputHandler()	{ };
	virtual ~IInputHandler()	{ };
	
	virtual bool HandleInput(const Input *i)=0;
};
