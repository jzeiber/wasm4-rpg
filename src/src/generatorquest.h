#pragma once

#include <stdint.h>
#include "questdata.h"
#include "gamedata.h"

void GenerateQuest(const uint64_t seed, const uint64_t wx, const uint64_t wy, GameData *gamedata, QuestData &quest);
