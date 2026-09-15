// main.c - game entry point and main loop

#include "raylib.h"
#include "game_state.h"
#include "player.h"
#include "level.h"
#include "anchor.h"
#include "collision.h"
#include "audio_manager.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define TARGET_FPS 60
#define FAIL_Y_THRESHOLD (SCREEN_HEIGHT + 180)

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Time Anchor");
    SetTargetFPS(TARGET_FPS);
    SetExitKey(KEY_NULL);  // ESC is for pause menu

    InitAudioDevice();
    AudioManager_Init();

    GameState_Init();
    Player_Init();
    Level_Init();
    Anchor_Init();

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // update based on current state
        switch (GameState_GetCurrent()) {
            case STATE_MENU:
                GameState_UpdateMenu();
                break;

            case STATE_LEVEL_SELECT:
                GameState_UpdateLevelSelect();
                break;

            case STATE_PLAYING:
                Player_Update(dt);
                Anchor_Update(dt);
                Level_Update(dt);
                Collision_Update();

                // check if player fell off screen
                if (Player_GetPosition().y > FAIL_Y_THRESHOLD) {
                    GameState_TriggerFail();
                    break;
                }

                if (IsKeyPressed(KEY_ESCAPE)) {
                    GameState_SetCurrent(STATE_PAUSED);
                }
                break;

            case STATE_PAUSED:
                GameState_UpdatePause();
                break;

            case STATE_LEVEL_COMPLETE:
                GameState_UpdateLevelComplete();
                break;

            case STATE_VICTORY:
                GameState_UpdateVictory();
                break;

            case STATE_FAIL:
                GameState_UpdateFail();
                break;

            default:
                GameState_SetCurrent(STATE_MENU);
                break;
        }

        // draw
        BeginDrawing();
        ClearBackground((Color){20, 20, 40, 255});

        switch (GameState_GetCurrent()) {
            case STATE_MENU:
                GameState_DrawMenu();
                break;

            case STATE_LEVEL_SELECT:
                GameState_DrawLevelSelect();
                break;

            case STATE_PLAYING:
                Level_Draw();
                Anchor_Draw();
                Player_Draw();
                break;

            case STATE_PAUSED:
                Level_Draw();
                Anchor_Draw();
                Player_Draw();
                GameState_DrawPause();
                break;

            case STATE_LEVEL_COMPLETE:
                GameState_DrawLevelComplete();
                break;

            case STATE_VICTORY:
                GameState_DrawVictory();
                break;

            case STATE_FAIL:
                Level_Draw();
                Anchor_Draw();
                Player_Draw();
                GameState_DrawFail();
                break;

            default:
                GameState_DrawMenu();
                break;
        }

        EndDrawing();
    }

    AudioManager_Cleanup();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
