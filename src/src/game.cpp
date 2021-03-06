#include "game.h"
#include "statestartup.h"
#include "statemainmenu.h"
#include "stategameoverworld.h"
#include "stategamemap.h"
#include "stategamequestjournal.h"
#include "stategamecharactersheet.h"
#include "stategameinventory.h"
#include "stategameover.h"
#include "statesettings.h"
#include "wasm4.h"
#include "global.h"
#include "settings.h"
#include "printf.h"
#include "miscfuncs.h"
#include "wasmmath.h"

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

	m_states[STATE_STARTUP]=static_cast<IState *>(&StateStartup::Instance());
	m_updatables[STATE_STARTUP]=static_cast<IUpdatable *>(&StateStartup::Instance());

	m_states[STATE_MAINMENU]=static_cast<IState *>(&StateMainMenu::Instance());
	m_updatables[STATE_MAINMENU]=static_cast<IUpdatable *>(&StateMainMenu::Instance());
	m_drawables[STATE_MAINMENU]=static_cast<IDrawable *>(&StateMainMenu::Instance());
	m_inputhandlers[STATE_MAINMENU]=static_cast<IInputHandler *>(&StateMainMenu::Instance());

	m_states[STATE_SETTINGS]=static_cast<IState *>(&StateSettings::Instance());
	m_updatables[STATE_SETTINGS]=static_cast<IUpdatable *>(&StateSettings::Instance());
	m_drawables[STATE_SETTINGS]=static_cast<IDrawable *>(&StateSettings::Instance());
	m_inputhandlers[STATE_SETTINGS]=static_cast<IInputHandler *>(&StateSettings::Instance());

	m_states[STATE_GAMEOVERWORLD]=static_cast<IState *>(&StateGameOverworld::Instance());
	m_updatables[STATE_GAMEOVERWORLD]=static_cast<IUpdatable *>(&StateGameOverworld::Instance());
	m_drawables[STATE_GAMEOVERWORLD]=static_cast<IDrawable *>(&StateGameOverworld::Instance());
	m_inputhandlers[STATE_GAMEOVERWORLD]=static_cast<IInputHandler *>(&StateGameOverworld::Instance());

	m_states[STATE_GAMEMAP]=static_cast<IState *>(&StateGameMap::Instance());
	m_updatables[STATE_GAMEMAP]=static_cast<IUpdatable *>(&StateGameMap::Instance());
	m_drawables[STATE_GAMEMAP]=static_cast<IDrawable *>(&StateGameMap::Instance());
	m_inputhandlers[STATE_GAMEMAP]=static_cast<IInputHandler *>(&StateGameMap::Instance());

	m_states[STATE_GAMEQUESTJOURNAL]=static_cast<IState *>(&StateGameQuestJournal::Instance());
	m_updatables[STATE_GAMEQUESTJOURNAL]=static_cast<IUpdatable *>(&StateGameQuestJournal::Instance());
	m_drawables[STATE_GAMEQUESTJOURNAL]=static_cast<IDrawable *>(&StateGameQuestJournal::Instance());
	m_inputhandlers[STATE_GAMEQUESTJOURNAL]=static_cast<IInputHandler *>(&StateGameQuestJournal::Instance());

	m_states[STATE_GAMECHARACTERSHEET]=static_cast<IState *>(&StateGameCharacterSheet::Instance());
	m_updatables[STATE_GAMECHARACTERSHEET]=static_cast<IUpdatable *>(&StateGameCharacterSheet::Instance());
	m_drawables[STATE_GAMECHARACTERSHEET]=static_cast<IDrawable *>(&StateGameCharacterSheet::Instance());
	m_inputhandlers[STATE_GAMECHARACTERSHEET]=static_cast<IInputHandler *>(&StateGameCharacterSheet::Instance());

	m_states[STATE_GAMEINVENTORY]=static_cast<IState *>(&StateGameInventory::Instance());
	m_updatables[STATE_GAMEINVENTORY]=static_cast<IUpdatable *>(&StateGameInventory::Instance());
	m_drawables[STATE_GAMEINVENTORY]=static_cast<IDrawable *>(&StateGameInventory::Instance());
	m_inputhandlers[STATE_GAMEINVENTORY]=static_cast<IInputHandler *>(&StateGameInventory::Instance());

	m_states[STATE_GAMEOVER]=static_cast<IState *>(&StateGameOver::Instance());
	m_updatables[STATE_GAMEOVER]=static_cast<IUpdatable *>(&StateGameOver::Instance());
	m_drawables[STATE_GAMEOVER]=static_cast<IDrawable *>(&StateGameOver::Instance());
	m_inputhandlers[STATE_GAMEOVER]=static_cast<IInputHandler *>(&StateGameOver::Instance());

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
						strncpy(m_gamedata.m_gamemessages[j],m_gamedata.m_gamemessages[j+1],31);
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
			if(oldstate==STATE_SETTINGS)
			{
				SaveData();
			}
			LoadData();
			if(m_states[newstate])
			{
				m_states[newstate]->StateChanged(oldstate,params);
			}
			break;
		case STATE_SETTINGS:
		case STATE_GAMEOVERWORLD:
		case STATE_GAMEMAP:
		case STATE_GAMEQUESTJOURNAL:
		case STATE_GAMECHARACTERSHEET:
		case STATE_GAMEINVENTORY:
		case STATE_GAMEOVER:
			if(m_states[newstate])
			{
				m_states[newstate]->StateChanged(oldstate,params);
			}
			break;
		default:
			trace("Game::ChangeState State not impelemented!");
		}
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
		int16_t pos=24+(saveslot*500);
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
		buff[24+(saveslot*500)]=0;
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

		m_gamedata.WriteGameData(&buff[24+(m_gamedata.m_saveslot*500)]);

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

		if(m_gamedata.LoadGameData(&buff[24]))
		{
			m_saveslotused[0]=true;
		}
		if(m_gamedata.LoadGameData(&buff[524]))
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

int32_t Game::GetNextLevelExperience(const int16_t level) const
{
	return static_cast<int32_t>(level)*static_cast<int32_t>(level)*10;
}

int32_t Game::GetMaxExperienceGain(const int16_t level) const
{
	int32_t val=_sqrt(static_cast<int32_t>(level)*static_cast<int32_t>(level)*10);
	return (val < 1 ? 1 : val);
}

int32_t Game::GetMinExperienceGain(const int16_t level) const
{
	int32_t val=_sqrt(static_cast<int32_t>(level)*static_cast<int32_t>(level)*10)/10.0;
	return (val < 1 ? 1 : val);
}

int32_t Game::GetLevelMaxHealth(const int16_t level) const
{
	return 8+(level*2);
}
