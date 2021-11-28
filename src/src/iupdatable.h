#pragma once

class Game;

class IUpdatable
{
public:
	IUpdatable()	{ };
	virtual ~IUpdatable()	{ };
	
	virtual void Update(const int ticks, Game *game=nullptr)=0;

};
