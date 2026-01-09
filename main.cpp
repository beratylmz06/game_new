#include "raylib.h"
#include "Player.h"
#include <vector>
#include <string>

struct Hazard {
    Rectangle rect;     
    PlatformType type;  
};


struct LevelData {
    std::vector<Platform> platforms;
    std::vector<Hazard> hazards;
    Vector2 startPos;
    Rectangle goalRect;
};

int main() {
    InitWindow(800, 600, "SENG479 - Puzzle Platformer ");
    SetTargetFPS(60);

    Player player;

 
    std::vector<LevelData> levels;

    levels.push_back({
        { 
            { {0, 500, 200, 50}, PLATFORM_BOTH, DARKGRAY },
            { {250, 400, 150, 30}, PLATFORM_RED, MAROON },
            { {450, 300, 150, 30}, PLATFORM_BLUE, DARKBLUE },
            { {650, 200, 150, 50}, PLATFORM_BOTH, DARKGRAY }
        },
        {}, 
        { 50, 300 },
        { 700, 140, 40, 60 }
    });

   
    levels.push_back({
        { 
            { {0, 500, 100, 50}, PLATFORM_BOTH, DARKGRAY },
            { {150, 450, 100, 20}, PLATFORM_RED, MAROON },
            { {300, 350, 100, 20}, PLATFORM_BLUE, DARKBLUE },
            { {450, 250, 100, 20}, PLATFORM_RED, MAROON },
            { {600, 200, 200, 50}, PLATFORM_BOTH, DARKGRAY }
        },
        { 
            { {0, 580, 800, 20}, PLATFORM_BOTH } 
        },
        { 20, 300 }, 
        { 700, 140, 40, 60 }
    });

   
    levels.push_back({
        { 
            { {0, 500, 100, 50}, PLATFORM_BOTH, DARKGRAY },
            { {150, 400, 80, 20}, PLATFORM_RED, MAROON },
            { {300, 350, 80, 20}, PLATFORM_BLUE, DARKBLUE },
            { {450, 300, 80, 20}, PLATFORM_RED, MAROON },    
            { {600, 250, 80, 20}, PLATFORM_BLUE, DARKBLUE }, 
            { {700, 200, 100, 50}, PLATFORM_BOTH, DARKGRAY }
        },
        { 
            
            { {460, 280, 60, 20}, PLATFORM_BLUE }, 
            
           
            { {0, 580, 800, 20}, PLATFORM_BOTH } 
        },
        { 20, 300 },
        { 720, 140, 40, 60 }
    });

    int currentLevelIndex = 0; 
    int currentWorld = PLATFORM_RED; 
    bool gameFinished = false;

    player.position = levels[currentLevelIndex].startPos;

    while (!WindowShouldClose()) {
        
        
        if (!gameFinished) {
            float dt = GetFrameTime();
            LevelData& currentLevel = levels[currentLevelIndex];
            
            player.Update(dt, currentLevel.platforms, currentWorld);

            
            if (IsKeyPressed(KEY_SPACE)) {
                if (currentWorld == PLATFORM_RED) currentWorld = PLATFORM_BLUE;
                else currentWorld = PLATFORM_RED;
            }

            Rectangle playerRect = { player.position.x, player.position.y - 40, 40, 40 };

            if (CheckCollisionRecs(playerRect, currentLevel.goalRect)) {
                currentLevelIndex++; 
                if (currentLevelIndex < levels.size()) {
                    player.position = levels[currentLevelIndex].startPos;
                    player.velocity = { 0, 0 };
                    currentWorld = PLATFORM_RED; 
                } else {
                    gameFinished = true;
                }
            }

            
            for (Hazard& h : currentLevel.hazards) {
                
                if (h.type != PLATFORM_BOTH && h.type != currentWorld) continue;

                if (CheckCollisionRecs(playerRect, h.rect)) {
                    
                    player.position = currentLevel.startPos;
                    player.velocity = { 0, 0 };
                }
            }
            
            
            if (player.position.y > 800) {
                 player.position = currentLevel.startPos;
                 player.velocity = { 0, 0 };
            }
        }
        else { 
            if (IsKeyPressed(KEY_ENTER)) {
                gameFinished = false;
                currentLevelIndex = 0;
                player.position = levels[0].startPos;
                currentWorld = PLATFORM_RED;
            }
        }

       
        BeginDrawing();
            
            if (currentWorld == PLATFORM_RED) ClearBackground({40, 0, 0, 255}); 
            else ClearBackground({0, 0, 40, 255}); 

            if (!gameFinished) {
                LevelData& currentLevel = levels[currentLevelIndex];

              
                for (Platform& p : currentLevel.platforms) {
                    if (p.type == PLATFORM_BOTH || p.type == currentWorld) {
                        DrawRectangleRec(p.rect, p.color);
                    } else {
                        DrawRectangleRec(p.rect, Fade(p.color, 0.1f));
                        DrawRectangleLinesEx(p.rect, 1, Fade(WHITE, 0.3f));
                    }
                }

                
                for (Hazard& h : currentLevel.hazards) {
                    Color spikeColor = RED;
                    Color outlineColor = MAROON;
                    
                    
                    if (h.type != PLATFORM_BOTH && h.type != currentWorld) {
                         spikeColor = Fade(RED, 0.2f);
                         outlineColor = Fade(MAROON, 0.2f);
                    }

                    int spikeWidth = 20; 
                    int spikeCount = (int)(h.rect.width / spikeWidth);

                    for (int i = 0; i < spikeCount; i++) {
                        Vector2 p1 = { h.rect.x + (i * spikeWidth), h.rect.y + h.rect.height };
                        Vector2 p2 = { h.rect.x + (i * spikeWidth) + 10, h.rect.y };
                        Vector2 p3 = { h.rect.x + (i * spikeWidth) + 20, h.rect.y + h.rect.height };
                        
                        DrawTriangle(p2, p1, p3, spikeColor); 
                        DrawTriangleLines(p2, p1, p3, outlineColor); 
                    }
                }

                DrawRectangleRec(currentLevel.goalRect, GOLD);
                DrawRectangleLinesEx(currentLevel.goalRect, 2, ORANGE);

                player.Draw();
                DrawText(TextFormat("LEVEL: %d", currentLevelIndex + 1), 10, 10, 20, RAYWHITE);
            } 
            else {
                DrawRectangle(0, 0, 800, 600, BLACK);
                DrawText("CONGRATULATIONS! GAME OVER!", 200, 250, 30, GREEN);
                DrawText("[ENTER] Start Over-", 280, 300, 20, GRAY);
            }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}