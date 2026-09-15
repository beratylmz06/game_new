#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "raylib.h"
#include <stdbool.h>

typedef enum {
    STATE_MENU,
    STATE_LEVEL_SELECT,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_LEVEL_COMPLETE,
    STATE_VICTORY,
    STATE_FAIL
} GameState;

void GameState_Init(void);

GameState GameState_GetCurrent(void);
void GameState_SetCurrent(GameState state);

// Update per-state
void GameState_UpdateMenu(void);
void GameState_UpdateLevelSelect(void);
void GameState_UpdatePause(void);
void GameState_UpdateLevelComplete(void);
void GameState_UpdateVictory(void);
void GameState_UpdateFail(void);

// Draw per-state
void GameState_DrawMenu(void);
void GameState_DrawLevelSelect(void);
void GameState_DrawPause(void);
void GameState_DrawLevelComplete(void);
void GameState_DrawVictory(void);
void GameState_DrawFail(void);

// External trigger (main.c’den çağıracaksın)
void GameState_TriggerFail(void);

#endif
