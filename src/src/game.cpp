#include "game.h"
#include "statestartup.h"
#include "statemainmenu.h"
#include "stategameoverworld.h"
#include "stategamemap.h"
#include "stategamequestjournal.h"
#include "wasm4.h"
#include "global.h"
#include "settings.h"
#include "printf.h"
#include "miscfuncs.h"

Game::Game():m_state(STATE_STARTUP)
{
	trace("Game::Game()");
	for(int i=0; i<STATE_MAX; i++)
	{
		m_states[i]=nullptr;
		m_updatables[i]=nullptr;
		m_drawables[i]=nullptr;
		m_inputhandlers[i]=nullptr;
	}

	for(int i=0; i<2; i++)
	{
		m_saveslotused[i]=false;
	}

	m_modal=nullptr;

	m_states[STATE_STARTUP]=dynamic_cast<IState *>(&StateStartup::Instance());
	m_updatables[STATE_STARTUP]=dynamic_cast<IUpdatable *>(&StateStartup::Instance());

	m_states[STATE_MAINMENU]=dynamic_cast<IState *>(&StateMainMenu::Instance());
	m_updatables[STATE_MAINMENU]=dynamic_cast<IUpdatable *>(&StateMainMenu::Instance());
	m_drawables[STATE_MAINMENU]=dynamic_cast<IDrawable *>(&StateMainMenu::Instance());
	m_inputhandlers[STATE_MAINMENU]=dynamic_cast<IInputHandler *>(&StateMainMenu::Instance());

	/*
	m_states[STATE_GAME]=dynamic_cast<IState *>(&StateGame::Instance());
	m_updatables[STATE_GAME]=dynamic_cast<IUpdatable *>(&StateGame::Instance());
	m_drawables[STATE_GAME]=dynamic_cast<IDrawable *>(&StateGame::Instance());
	m_inputhandlers[STATE_GAME]=dynamic_cast<IInputHandler *>(&StateGame::Instance());
	*/

	m_states[STATE_GAMEOVERWORLD]=dynamic_cast<IState *>(&StateGameOverworld::Instance());
	m_updatables[STATE_GAMEOVERWORLD]=dynamic_cast<IUpdatable *>(&StateGameOverworld::Instance());
	m_drawables[STATE_GAMEOVERWORLD]=dynamic_cast<IDrawable *>(&StateGameOverworld::Instance());
	m_inputhandlers[STATE_GAMEOVERWORLD]=dynamic_cast<IInputHandler *>(&StateGameOverworld::Instance());

	m_states[STATE_GAMEMAP]=dynamic_cast<IState *>(&StateGameMap::Instance());
	m_updatables[STATE_GAMEMAP]=dynamic_cast<IUpdatable *>(&StateGameMap::Instance());
	m_drawables[STATE_GAMEMAP]=dynamic_cast<IDrawable *>(&StateGameMap::Instance());
	m_inputhandlers[STATE_GAMEMAP]=dynamic_cast<IInputHandler *>(&StateGameMap::Instance());

	m_states[STATE_GAMEQUESTJOURNAL]=dynamic_cast<IState *>(&StateGameQuestJournal::Instance());
	m_updatables[STATE_GAMEQUESTJOURNAL]=dynamic_cast<IUpdatable *>(&StateGameQuestJournal::Instance());
	m_drawables[STATE_GAMEQUESTJOURNAL]=dynamic_cast<IDrawable *>(&StateGameQuestJournal::Instance());
	m_inputhandlers[STATE_GAMEQUESTJOURNAL]=dynamic_cast<IInputHandler *>(&StateGameQuestJournal::Instance());

}

Game::~Game()
{

}
	
Game &Game::Instance()
{
	static Game g;
	return g;
}

GameData &Game::GetGameData()
{
	return m_gamedata;
}

void Game::Update(const int ticks, Game *game)
{
	//m_ticks+=ticks;
	m_gamedata.m_ticks+=ticks;

	// check for expired messages
	{
		bool rotate=false;
		for(int i=0; i<MAX_GAMEMESSAGE; i++)
		{
			if(m_gamedata.m_gamemessagedecay[i]!=0 && m_gamedata.m_gamemessagedecay[i] < m_gamedata.m_ticks)
			{
				m_gamedata.m_gamemessagedecay[i]=0;
				m_gamedata.m_gamemessages[i][0]='\0';
				rotate=true;
			}
		}
		// rotate messages up
		if(rotate==true)
		{
			for(int i=0; i<MAX_GAMEMESSAGE-1; i++)
			{
				for(int j=0; j<MAX_GAMEMESSAGE-1; j++)
				{
					if(m_gamedata.m_gamemessages[j][0]=='\0' && m_gamedata.m_gamemessages[j+1][0]!='\0')
					{
						strncpy(m_gamedata.m_gamemessages[j],m_gamedata.m_gamemessages[j+1],19);
						m_gamedata.m_gamemessagedecay[j]=m_gamedata.m_gamemessagedecay[j+1];

						m_gamedata.m_gamemessages[j+1][0]='\0';
						m_gamedata.m_gamemessagedecay[j+1]=0;
					}
				}
			}
		}
	}

	if(m_modal)	// update modal first, when modal is closed, parent will then get update immediately after
	{
		m_modal->Update(ticks,this);
		if(m_modal->Shown()==false)
		{
			m_modal=nullptr;
		}
	}
	if(m_state>=0 && m_state<STATE_MAX && m_updatables[m_state]!=nullptr && (!m_modal || (m_modal->AllowParentUpdate())))
	{
		m_updatables[m_state]->Update(ticks,this);
	}

}

uint64_t Game::GetTicks() const
{
	return m_gamedata.m_ticks;
}

void Game::Draw()
{
	if(m_state>=0 && m_state<STATE_MAX && m_drawables[m_state]!=nullptr && (!m_modal || (m_modal->AllowParentDraw())))
	{
		m_drawables[m_state]->Draw();
	}
	if(m_modal)
	{
		m_modal->Draw();
	}
}

bool Game::HandleInput(const Input *input)
{
	bool handled=false;
	if(m_state>=0 && m_state<STATE_MAX && m_inputhandlers[m_state]!=nullptr && (!m_modal || (m_modal->AllowParentInput())))
	{
		handled=m_inputhandlers[m_state]->HandleInput(input);
	}
	if(m_modal)
	{
		handled=m_modal->HandleInput(input);
	}
	return handled;
}

void Game::ChangeState(const uint8_t newstate, void *params)
{
	if(newstate>=0 && newstate<STATE_MAX && newstate!=m_state)
	{
		const uint8_t oldstate=m_state;
		switch(newstate)
		{
		case STATE_MAINMENU:
			LoadData();
			if(m_states[newstate])
			{
				m_states[newstate]->StateChanged(oldstate,params);
			}
			break;
		case STATE_GAMEOVERWORLD:
		case STATE_GAMEMAP:
		case STATE_GAMEQUESTJOURNAL:
			if(m_states[newstate])
			{
				m_states[newstate]->StateChanged(oldstate,params);
			}
			break;
		default:
			trace("Game::ChangeState State not impelemented!");
		}
		// TODO - initialize state
		m_state=newstate;
	}
}

void Game::ShowModalDialog(ModalDialog *dialog)
{
	m_modal=dialog;
	if(m_modal)
	{
		m_modal->Show();
	}
}

bool Game::SaveSlotUsed(const int8_t saveslot)
{
	if(saveslot>=0 && saveslot<2)
	{
		return m_saveslotused[saveslot];
	}
	return false;
}

bool Game::LoadGameData(const int8_t saveslot)
{
	if(SaveSlotUsed(saveslot)==false)
	{
		return false;
	}

	uint8_t buff[1024];
	uint32_t bytes=diskr(buff,1024);
	if(bytes==1024)
	{
		int16_t pos=12+(saveslot*500);
		if(m_gamedata.LoadGameData(&buff[pos])==true)
		{
			return true;
		}
	}
	return false;
}

void Game::DeleteGameData(const int8_t saveslot)
{
	if(saveslot>=0 && saveslot<2 && m_saveslotused[saveslot]==true)
	{
		m_saveslotused[saveslot]=false;
		uint8_t buff[1024];
		diskr(buff,1024);
		buff[12+(saveslot*500)]=0;
		diskw(buff,1024);
	}
}

void Game::SaveGameData()
{
	if(m_gamedata.m_saveslot>=0 && m_gamedata.m_saveslot<2)
	{
		uint8_t buff[1024];
		
		diskr(buff,1024);
		buff[0]='R';
		buff[1]='S';
		buff[2]='G';
		buff[3]='F';

		Settings::Instance().WriteSettings(&buff[4]);

		m_gamedata.WriteGameData(&buff[12+(m_gamedata.m_saveslot*500)]);

		diskw(buff,1024);
	}
}

void Game::LoadData()
{
	uint8_t buff[1024];
	uint32_t bytes=diskr(buff,1024);
	if(bytes==1024 && buff[0]=='R' && buff[1]=='S' && buff[2]=='G' && buff[3]=='F')
	{
		Settings::Instance().LoadSettings(&buff[4]);

		if(m_gamedata.LoadGameData(&buff[12]))
		{
			m_saveslotused[0]=true;
		}
		if(m_gamedata.LoadGameData(&buff[512]))
		{
			m_saveslotused[1]=true;
		}

	}
}

void Game::SaveData()
{
	uint8_t buff[1024];
	
	diskr(buff,1024);
	buff[0]='R';
	buff[1]='S';
	buff[2]='G';
	buff[3]='F';

	Settings::Instance().WriteSettings(&buff[4]);

	diskw(buff,1024);
}
