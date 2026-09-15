// game_state.c - handles menu, level select, pause, fail, victory states

#include <stdio.h>
#include "raylib.h"
#include "game_state.h"
#include "level.h"
#include "player.h"
#include "anchor.h"
#include "audio_manager.h"

#define TOTAL_LEVELS 5

static GameState currentState = STATE_MENU;
static int menuSelection = 0;
static int levelSelection = 0;
static int pauseSelection = 0;
static float failTimer = 0.0f;

// for delayed transitions (lets sounds play before switching)
static bool pendingTransition = false;
static GameState pendingState = STATE_MENU;
static float transitionDelay = 0.0f;
static int pendingLevelIdx = -1;
static bool pendingQuit = false;

static void StartDelayedTransition(GameState target, float delay) {
    pendingTransition = true;
    pendingState = target;
    transitionDelay = delay;
    pendingLevelIdx = -1;
    pendingQuit = false;
}

static void StartDelayedLevelTransition(int lvlIdx, float delay) {
    pendingTransition = true;
    pendingState = STATE_PLAYING;
    transitionDelay = delay;
    pendingLevelIdx = lvlIdx;
    pendingQuit = false;
}

static void StartDelayedQuit(float delay) {
    pendingTransition = true;
    pendingQuit = true;
    transitionDelay = delay;
}

static const char* GetFailMessage(int lvlIdx) {
    if (lvlIdx == 0) return "ANCHOR BEFORE YOU JUMP";
    return "ANCHOR LOST";
}

void GameState_TriggerFail(void) {
    currentState = STATE_FAIL;
    failTimer = 0.0f;
    AudioManager_PlaySound(SOUND_SPIKE_DEATH);
}

void GameState_Init(void) {
    currentState = STATE_MENU;
    menuSelection = 0;
    levelSelection = 0;
    pauseSelection = 0;
    failTimer = 0.0f;
    pendingTransition = false;
    pendingState = STATE_MENU;
    transitionDelay = 0.0f;
    pendingLevelIdx = -1;
    pendingQuit = false;
}

GameState GameState_GetCurrent(void) { return currentState; }

void GameState_SetCurrent(GameState state) { currentState = state; }

// ---- MAIN MENU ----

void GameState_UpdateMenu(void) {
    if (pendingTransition) {
        transitionDelay -= GetFrameTime();
        if (transitionDelay <= 0.0f) {
            pendingTransition = false;
            if (pendingQuit) CloseWindow();
            else currentState = pendingState;
        }
        return;
    }

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        menuSelection = (menuSelection == 0) ? 1 : 0;
        AudioManager_PlaySound(SOUND_MENU_MOVE);
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        menuSelection = (menuSelection == 1) ? 0 : 1;
        AudioManager_PlaySound(SOUND_MENU_MOVE);
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        AudioManager_PlaySound(SOUND_MENU_SELECT);
        if (menuSelection == 0) {
            StartDelayedTransition(STATE_LEVEL_SELECT, 0.15f);
            levelSelection = 0;
        } else {
            StartDelayedQuit(0.25f);
        }
    }
}

void GameState_DrawMenu(void) {
    const char* title = "TIME ANCHOR";
    int tw = MeasureText(title, 60);
    DrawText(title, 640 - tw/2, 150, 60, (Color){100, 200, 255, 255});

    const char* sub = "A Puzzle Platformer";
    int sw = MeasureText(sub, 24);
    DrawText(sub, 640 - sw/2, 220, 24, (Color){150, 150, 180, 255});

    const char* items[] = {"PLAY", "QUIT"};
    for (int i = 0; i < 2; i++) {
        Color c = (i == menuSelection) ? GOLD : WHITE;
        int iw = MeasureText(items[i], 32);
        if (i == menuSelection) DrawText(">", 640 - iw/2 - 30, 350 + i*60, 32, GOLD);
        DrawText(items[i], 640 - iw/2, 350 + i*60, 32, c);
    }

    DrawText("ARROWS/WASD to navigate, ENTER/SPACE to select", 640 - 200, 600, 16, GRAY);
}

// ---- LEVEL SELECT ----

static bool IsLevelUnlocked(int idx) {
    if (idx == 0) return true;
    return Level_IsCompleted(idx - 1);
}

void GameState_UpdateLevelSelect(void) {
    if (pendingTransition) {
        transitionDelay -= GetFrameTime();
        if (transitionDelay <= 0.0f) {
            pendingTransition = false;
            if (pendingLevelIdx >= 0) {
                Level_Load(pendingLevelIdx);
                Player_Respawn();
            }
            currentState = pendingState;
        }
        return;
    }

    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        levelSelection = (levelSelection == 0) ? TOTAL_LEVELS - 1 : levelSelection - 1;
        AudioManager_PlaySound(SOUND_MENU_MOVE);
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        levelSelection = (levelSelection == TOTAL_LEVELS - 1) ? 0 : levelSelection + 1;
        AudioManager_PlaySound(SOUND_MENU_MOVE);
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        if (IsLevelUnlocked(levelSelection)) {
            AudioManager_PlaySound(SOUND_MENU_SELECT);
            AudioManager_PlaySound(SOUND_LEVEL_TRANSITION);
            Level_Load(levelSelection);
            Player_Respawn();
            currentState = STATE_PLAYING;
        }
    }

    if (IsKeyPressed(KEY_ESCAPE)) currentState = STATE_MENU;
}

void GameState_DrawLevelSelect(void) {
    const char* title = "SELECT LEVEL";
    int tw = MeasureText(title, 40);
    DrawText(title, 640 - tw/2, 100, 40, WHITE);

    for (int i = 0; i < TOTAL_LEVELS; i++) {
        int x = 200 + i * 180;
        int y = 300;
        bool unlocked = IsLevelUnlocked(i);
        bool done = Level_IsCompleted(i);
        
        Color boxC, textC;
        if (!unlocked) {
            boxC = (Color){40, 40, 50, 255};
            textC = (Color){80, 80, 90, 255};
        } else if (i == levelSelection) {
            boxC = done ? (Color){100, 200, 100, 255} : GOLD;
            textC = WHITE;
        } else {
            boxC = done ? (Color){50, 150, 50, 255} : DARKGRAY;
            textC = WHITE;
        }

        DrawRectangle(x, y, 120, 120, boxC);
        DrawRectangleLines(x, y, 120, 120, unlocked ? WHITE : (Color){60, 60, 70, 255});

        char num[4];
        sprintf(num, "%d", i + 1);
        int nw = MeasureText(num, 48);
        DrawText(num, x + 60 - nw/2, y + 35, 48, textC);

        if (done) DrawText("OK", x + 85, y + 10, 16, (Color){100, 255, 100, 255});
        if (!unlocked) DrawText("LOCKED", x + 25, y + 90, 14, (Color){100, 100, 110, 255});
    }

    const char* names[] = {"First Anchor", "Vertical Thinking", "Commitment", "Timed Platforms", "Final Loop"};
    int nw = MeasureText(names[levelSelection], 24);
    DrawText(names[levelSelection], 640 - nw/2, 450, 24, WHITE);
    
    if (!IsLevelUnlocked(levelSelection)) {
        const char* lockMsg = "Complete previous level to unlock";
        int lw = MeasureText(lockMsg, 18);
        DrawText(lockMsg, 640 - lw/2, 480, 18, (Color){200, 100, 100, 255});
    }

    DrawText("LEFT/RIGHT to pick, ENTER to start, ESC to go back", 640 - 220, 600, 16, GRAY);
}

// ---- PAUSE ----

void GameState_UpdatePause(void) {
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        pauseSelection = (pauseSelection == 0) ? 1 : 0;
        AudioManager_PlaySound(SOUND_MENU_MOVE);
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        pauseSelection = (pauseSelection == 1) ? 0 : 1;
        AudioManager_PlaySound(SOUND_MENU_MOVE);
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        AudioManager_PlaySound(SOUND_MENU_SELECT);
        currentState = (pauseSelection == 0) ? STATE_PLAYING : STATE_LEVEL_SELECT;
    }

    if (IsKeyPressed(KEY_ESCAPE)) currentState = STATE_PLAYING;
}

void GameState_DrawPause(void) {
    DrawRectangle(0, 0, 1280, 720, (Color){0, 0, 0, 150});
    DrawRectangle(440, 200, 400, 320, (Color){30, 30, 50, 240});
    DrawRectangleLines(440, 200, 400, 320, WHITE);

    const char* title = "PAUSED";
    int tw = MeasureText(title, 40);
    DrawText(title, 640 - tw/2, 230, 40, WHITE);

    const char* items[] = {"RESUME", "LEVEL SELECT"};
    for (int i = 0; i < 2; i++) {
        Color c = (i == pauseSelection) ? GOLD : WHITE;
        int iw = MeasureText(items[i], 28);
        if (i == pauseSelection) DrawText(">", 640 - iw/2 - 25, 330 + i*50, 28, GOLD);
        DrawText(items[i], 640 - iw/2, 330 + i*50, 28, c);
    }
}

// ---- LEVEL COMPLETE ----

void GameState_UpdateLevelComplete(void) {
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        int next = Level_GetCurrentIndex() + 1;
        if (next < TOTAL_LEVELS) {
            AudioManager_PlaySound(SOUND_LEVEL_TRANSITION);
            Level_Load(next);
            Player_Respawn();
            currentState = STATE_PLAYING;
        } else {
            currentState = STATE_VICTORY;
        }
    }
    if (IsKeyPressed(KEY_ESCAPE)) currentState = STATE_LEVEL_SELECT;
}

void GameState_DrawLevelComplete(void) {
    DrawRectangle(0, 0, 1280, 720, (Color){0, 0, 0, 150});
    DrawRectangle(340, 200, 600, 300, (Color){30, 50, 30, 240});
    DrawRectangleLines(340, 200, 600, 300, (Color){100, 255, 100, 255});

    const char* title = "LEVEL COMPLETE!";
    int tw = MeasureText(title, 48);
    DrawText(title, 640 - tw/2, 250, 48, (Color){100, 255, 100, 255});

    const char* names[] = {"First Anchor", "Vertical Thinking", "Commitment", "Anchor Logic", "Final Loop"};
    int idx = Level_GetCurrentIndex();
    int nw = MeasureText(names[idx], 24);
    DrawText(names[idx], 640 - nw/2, 320, 24, WHITE);

    DrawText("Press ENTER for next level", 640 - 130, 420, 20, WHITE);
    DrawText("Press ESC for level select", 640 - 130, 450, 20, GRAY);
}

// ---- VICTORY ----

void GameState_UpdateVictory(void) {
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ESCAPE)) {
        currentState = STATE_MENU;
    }
}

void GameState_DrawVictory(void) {
    for (int i = 0; i < 720; i++) {
        int r = 20 + (i * 30) / 720;
        int g = 20 + (i * 50) / 720;
        int b = 50 + (i * 30) / 720;
        DrawLine(0, i, 1280, i, (Color){r, g, b, 255});
    }

    const char* title = "CONGRATULATIONS!";
    int tw = MeasureText(title, 60);
    DrawText(title, 640 - tw/2, 200, 60, GOLD);

    const char* sub = "You have mastered the Time Anchor";
    int sw = MeasureText(sub, 28);
    DrawText(sub, 640 - sw/2, 290, 28, WHITE);

    DrawText("All 5 levels completed!", 640 - 110, 380, 24, (Color){100, 255, 100, 255});
    DrawText("Press any key to return to menu", 640 - 150, 550, 20, GRAY);
}

// ---- FAIL ----

void GameState_UpdateFail(void) {
    failTimer += GetFrameTime();
    if (failTimer < 1.0f) return;  // show fail screen for at least 1 sec

    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
        if (Anchor_IsPlaced()) {
            Player_SetPosition(Anchor_GetPosition());
            Player_ResetVelocity();
        } else {
            Player_Respawn();
        }
        currentState = STATE_PLAYING;
        failTimer = 0.0f;
    }
}

void GameState_DrawFail(void) {
    DrawRectangle(0, 0, 1280, 720, (Color){0, 0, 0, 140});

    const char* msg = GetFailMessage(Level_GetCurrentIndex());
    int tw = MeasureText(msg, 40);
    DrawText(msg, 640 - tw/2, 320, 40, RAYWHITE);

    const char* hint = "Try placing your anchor closer to safety";
    int hw = MeasureText(hint, 18);
    DrawText(hint, 640 - hw/2, 380, 18, (Color){200, 200, 220, 255});
}
