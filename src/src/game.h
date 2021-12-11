#pragma once

#include <stdint.h>

#include "istate.h"
#include "iupdatable.h"
#include "idrawable.h"
#include "iinputhandler.h"
#include "modaldialog.h"
#include "gamedata.h"

class Game:public IUpdatable,public IDrawable,public IInputHandler
{
public:
	Game();
	~Game();
	
	static Game &Instance();
	
	enum GameState
	{
	STATE_STARTUP=0,
	STATE_MAINMENU,
	STATE_SETTINGS,
	STATE_GAMEOVERWORLD,
	STATE_GAMEMAP,
	STATE_GAMEQUESTJOURNAL,
	//STATE_GAMETOWN,
	//STATE_GAMEDUNGEON,
	//STATE_GAMEINVENTORY,
	STATE_GAMEOVER,
	STATE_MAX
	};
	
	bool HandleInput(const Input *input);
	void Update(const int ticks, Game *game=nullptr);
	void Draw();

	void ChangeState(const uint8_t newstate, void *params);

	void ShowModalDialog(ModalDialog *dialog);

	uint64_t GetTicks() const;

	bool SaveSlotUsed(const int8_t saveslot);
	GameData &GetGameData();
	void DeleteGameData(const int8_t saveslot);
	bool LoadGameData(const int8_t saveslot);
	void SaveGameData();	// saves current m_gamedata

	int32_t GetNextLevelExperience(const int16_t level) const;	// returns total experience required for the next level
	int32_t GetMaxExperienceGain(const int16_t level) const;	// maximum exp gain per kill/quest completion
	int32_t GetMinExperienceGain(const int16_t level) const;	// minimum exp gain per kill/quest completion
	int32_t GetLevelMaxHealth(const int16_t level) const;		// max health for level

private:
	uint8_t m_state;
	ModalDialog *m_modal;
	IState *m_states[STATE_MAX];
	IUpdatable *m_updatables[STATE_MAX];
	IDrawable *m_drawables[STATE_MAX];
	IInputHandler *m_inputhandlers[STATE_MAX];
	GameData m_gamedata;
	bool m_saveslotused[2];

	void LoadData();
	void SaveData();

};
