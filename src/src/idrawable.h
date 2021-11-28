#pragma once

class IDrawable
{
public:
	IDrawable()	{ };
	virtual ~IDrawable()	{ };
	
	virtual void Draw()=0;
};
