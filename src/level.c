/**
 * level.c - Level data and rendering implementation
 * Enhanced with visual variety, spike hazards, and atmospheric effects
 */

#include "level.h"
#include "anchor.h"
#include "game_state.h"
#include "audio_manager.h"
#include <math.h>

#define TOTAL_LEVELS 5
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

static Level currentLevel;
static bool levelCompletionStatus[TOTAL_LEVELS] = {false};
static float glowTimer = 0.0f;  // For animated glow effects

// Forward declarations for level data
static void Level_LoadLevel1(void);
static void Level_LoadLevel2(void);
static void Level_LoadLevel3(void);
static void Level_LoadLevel4(void);
static void Level_LoadLevel5(void);

// Helper to add a spike (default upward-pointing floor spike)
static void AddSpike(float x, float y, float width, float height) {
    if (currentLevel.spikeCount >= MAX_SPIKES) return;
    currentLevel.spikes[currentLevel.spikeCount++] = (Spike){
        .position = (Vector2){x, y},
        .width = width,
        .height = height,
        .isActive = true,
        .direction = SPIKE_UP
    };
}

// Helper to add a directional spike (wall spikes, ceiling spikes)
static void AddDirectionalSpike(float x, float y, float width, float height, SpikeDirection dir) {
    if (currentLevel.spikeCount >= MAX_SPIKES) return;
    currentLevel.spikes[currentLevel.spikeCount++] = (Spike){
        .position = (Vector2){x, y},
        .width = width,
        .height = height,
        .isActive = true,
        .direction = dir
    };
}

// Helper to create platform with material
static Platform CreatePlatform(Rectangle bounds, PlatformType type, PlatformMaterial material) {
    return (Platform){
        .bounds = bounds,
        .type = type,
        .material = material,
        .isActive = true,
        .startPos = {0},
        .endPos = {0},
        .speed = 0,
        .progress = 0,
        .timer = 0,
        .onDuration = 0,
        .offDuration = 0,
        .timedIsOn = true
    };
}

static Platform CreateMovingPlatform(Rectangle bounds, Vector2 start, Vector2 end, float speed, PlatformMaterial material) {
    return (Platform){
        .bounds = bounds,
        .type = PLATFORM_MOVING,
        .material = material,
        .isActive = true,
        .startPos = start,
        .endPos = end,
        .speed = speed,
        .progress = 0.0f,
        .timer = 0, .onDuration = 0, .offDuration = 0, .timedIsOn = true
    };
}

// Helper to create timed platform (toggles ON/OFF)
static Platform CreateTimedPlatform(Rectangle bounds, float onDuration, float offDuration, bool startOn, PlatformMaterial material) {
    return (Platform){
        .bounds = bounds,
        .type = PLATFORM_TIMED,
        .material = material,
        .isActive = startOn,
        .startPos = {0}, .endPos = {0},
        .speed = 0, .progress = 0,
        .timer = 0.0f,
        .onDuration = onDuration,
        .offDuration = offDuration,
        .timedIsOn = startOn
    };
}

void Level_Init(void) {
    currentLevel.levelIndex = 0;
    currentLevel.platformCount = 0;
    currentLevel.disabledZoneCount = 0;
    currentLevel.spikeCount = 0;
    glowTimer = 0.0f;
}

void Level_Load(int levelIndex) {
    // Clear current level
    currentLevel.platformCount = 0;
    currentLevel.disabledZoneCount = 0;
    currentLevel.spikeCount = 0;
    currentLevel.levelIndex = levelIndex;
    currentLevel.isCompleted = levelCompletionStatus[levelIndex];
    
    // Load level data based on index
    switch (levelIndex) {
        case 0: Level_LoadLevel1(); break;
        case 1: Level_LoadLevel2(); break;
        case 2: Level_LoadLevel4(); break;  // Level 3 -> Timed Platforms (eski Level 4)
        case 3: Level_LoadLevel3(); break;  // Level 4 -> Moving Platforms (eski Level 3)
        case 4: Level_LoadLevel5(); break;
        default: Level_LoadLevel1(); break;
    }
    
    // Remove any existing anchor
    Anchor_Remove();
}

void Level_Update(float deltaTime) {
    // Update glow animation
    glowTimer += deltaTime;
    if (glowTimer > 6.28318f) glowTimer -= 6.28318f;
    
    // Update moving platforms
    for (int i = 0; i < currentLevel.platformCount; i++) {
        Platform* p = &currentLevel.platforms[i];
        
        if (p->type == PLATFORM_MOVING) {
            // Update progress
            p->progress += p->speed * deltaTime;
            if (p->progress > 1.0f) {
                p->progress = 1.0f;
                p->speed = -fabsf(p->speed);  // Reverse direction
            } else if (p->progress < 0.0f) {
                p->progress = 0.0f;
                p->speed = fabsf(p->speed);
            }
            
            // Interpolate position
            p->bounds.x = p->startPos.x + (p->endPos.x - p->startPos.x) * p->progress;
            p->bounds.y = p->startPos.y + (p->endPos.y - p->startPos.y) * p->progress;
        }
        
        if (p->type == PLATFORM_ANCHOR_REACTIVE) {
            // Platform is active only when anchor is placed
            p->isActive = Anchor_IsPlaced();
        }
        
        if (p->type == PLATFORM_TIMED) {
            // Update timer
            p->timer += deltaTime;
            
            if (p->timedIsOn) {
                // Platform is currently ON
                if (p->timer >= p->onDuration) {
                    p->timedIsOn = false;
                    p->isActive = false;
                    p->timer = 0.0f;
                }
            } else {
                // Platform is currently OFF
                if (p->timer >= p->offDuration) {
                    p->timedIsOn = true;
                    p->isActive = true;
                    p->timer = 0.0f;
                }
            }
        }
    }
}

// Draw background with vertical gradient
void Level_DrawBackground(void) {
    // Dark navy at top, slightly lighter at bottom
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        float t = (float)y / SCREEN_HEIGHT;
        unsigned char r = (unsigned char)(15 + t * 15);   // 15 -> 30
        unsigned char g = (unsigned char)(15 + t * 20);   // 15 -> 35
        unsigned char b = (unsigned char)(35 + t * 25);   // 35 -> 60
        DrawLine(0, y, SCREEN_WIDTH, y, (Color){r, g, b, 255});
    }
}

// Get platform color based on material
static Color GetPlatformColor(Platform* p) {
    switch (p->material) {
        case MATERIAL_STONE:
            return (Color){60, 55, 65, 255};      // Dark gray/purple
        case MATERIAL_METAL:
            return (Color){70, 80, 100, 255};     // Blue steel
        case MATERIAL_ANCIENT:
            return (Color){85, 70, 55, 255};      // Warm brown
        default:
            return (Color){70, 70, 80, 255};
    }
}

static Color GetPlatformBorderColor(Platform* p) {
    switch (p->material) {
        case MATERIAL_STONE:
            return (Color){90, 85, 95, 255};
        case MATERIAL_METAL:
            return (Color){120, 140, 170, 255};
        case MATERIAL_ANCIENT:
            return (Color){120, 100, 80, 255};
        default:
            return (Color){100, 100, 110, 255};
    }
}

// Draw a single spike with glow effect (supports all directions)
static void DrawSpike(Spike* spike) {
    if (!spike->isActive) return;
    
    float x = spike->position.x;
    float y = spike->position.y;
    float w = spike->width;
    float h = spike->height;
    
    // Glow effect (pulsing)
    float glow = 0.5f + 0.5f * sinf(glowTimer * 2.0f);
    
    Color glowColor = (Color){255, 100, 50, (unsigned char)(40 + glow * 30)};
    Color spikeColor = (Color){200, 60, 40, 255};
    Color highlightColor = (Color){255, 120, 80, 255};
    
    Vector2 tip, base1, base2;
    Vector2 glowTip, glowBase1, glowBase2;
    
    switch (spike->direction) {
        case SPIKE_UP:  // Pointing upward (floor spike)
            tip = (Vector2){x, y - h};
            base1 = (Vector2){x - w/2, y};
            base2 = (Vector2){x + w/2, y};
            glowTip = (Vector2){x, y - h - 4};
            glowBase1 = (Vector2){x - w/2 - 4, y + 4};
            glowBase2 = (Vector2){x + w/2 + 4, y + 4};
            break;
        case SPIKE_DOWN:  // Pointing downward (ceiling spike)
            tip = (Vector2){x, y + h};
            base1 = (Vector2){x + w/2, y};
            base2 = (Vector2){x - w/2, y};
            glowTip = (Vector2){x, y + h + 4};
            glowBase1 = (Vector2){x + w/2 + 4, y - 4};
            glowBase2 = (Vector2){x - w/2 - 4, y - 4};
            break;
        case SPIKE_LEFT:  // Pointing left (wall spike on right side)
            tip = (Vector2){x - h, y};
            base1 = (Vector2){x, y - w/2};
            base2 = (Vector2){x, y + w/2};
            glowTip = (Vector2){x - h - 4, y};
            glowBase1 = (Vector2){x + 4, y - w/2 - 4};
            glowBase2 = (Vector2){x + 4, y + w/2 + 4};
            break;
        case SPIKE_RIGHT:  // Pointing right (wall spike on left side)
            tip = (Vector2){x + h, y};
            base1 = (Vector2){x, y + w/2};
            base2 = (Vector2){x, y - w/2};
            glowTip = (Vector2){x + h + 4, y};
            glowBase1 = (Vector2){x - 4, y + w/2 + 4};
            glowBase2 = (Vector2){x - 4, y - w/2 - 4};
            break;
    }
    
    // Outer glow
    DrawTriangle(glowTip, glowBase1, glowBase2, glowColor);
    
    // Main spike body
    DrawTriangle(tip, base1, base2, spikeColor);
    
    // Highlight edge
    DrawTriangleLines(tip, base1, base2, highlightColor);
}

void Level_Draw(void) {
    // Draw background gradient first
    Level_DrawBackground();
    
    // Draw disabled zones (background)
    for (int i = 0; i < currentLevel.disabledZoneCount; i++) {
        Color zoneColor = (Color){80, 25, 25, 80};
        DrawRectangleRec(currentLevel.disabledZones[i].bounds, zoneColor);
        
        // Draw diagonal warning lines
        Rectangle r = currentLevel.disabledZones[i].bounds;
        Color lineColor = (Color){120, 40, 40, 120};
        for (int offset = 0; offset < r.width + r.height; offset += 30) {
            float x1 = r.x + offset;
            float y1 = r.y;
            float x2 = r.x;
            float y2 = r.y + offset;
            
            if (x1 > r.x + r.width) {
                y1 += x1 - (r.x + r.width);
                x1 = r.x + r.width;
            }
            if (y2 > r.y + r.height) {
                x2 += y2 - (r.y + r.height);
                y2 = r.y + r.height;
            }
            
            if (x1 >= r.x && y1 <= r.y + r.height && x2 <= r.x + r.width && y2 >= r.y) {
                DrawLine((int)x1, (int)y1, (int)x2, (int)y2, lineColor);
            }
        }
        
        // Border
        DrawRectangleLinesEx(r, 2, (Color){150, 50, 50, 180});
    }
    
    // Draw spikes
    for (int i = 0; i < currentLevel.spikeCount; i++) {
        DrawSpike(&currentLevel.spikes[i]);
    }
    
    // Draw platforms
    for (int i = 0; i < currentLevel.platformCount; i++) {
        Platform* p = &currentLevel.platforms[i];
        
        Color platColor;
        Color borderColor;
        
        if (p->type == PLATFORM_ANCHOR_REACTIVE) {
            if (p->isActive) {
                platColor = (Color){40, 140, 70, 255};   // Green when active
                borderColor = (Color){80, 200, 100, 255};
            } else {
                // Ghost platform - draw outline only
                DrawRectangleLinesEx(p->bounds, 1, (Color){40, 120, 60, 80});
                continue;
            }
        } else if (p->type == PLATFORM_TIMED) {
            if (p->timedIsOn) {
                // Platform is ON - darker green, solid
                platColor = (Color){35, 100, 55, 255};
                borderColor = (Color){60, 160, 80, 255};
                
                // Warning blink in last 0.3s before switching OFF
                float timeRemaining = p->onDuration - p->timer;
                if (timeRemaining <= 0.3f && timeRemaining > 0.0f) {
                    // Blink effect - flash brighter
                    float blinkPhase = sinf(timeRemaining * 30.0f);
                    if (blinkPhase > 0) {
                        platColor = (Color){80, 180, 100, 255};
                        borderColor = (Color){120, 220, 140, 255};
                    }
                }
            } else {
                // Platform is OFF - draw faint outline only
                DrawRectangleLinesEx(p->bounds, 1, (Color){35, 80, 50, 100});
                
                // Show when it's coming back (last 0.5s of off duration)
                float timeRemaining = p->offDuration - p->timer;
                if (timeRemaining <= 0.5f && timeRemaining > 0.0f) {
                    float alpha = (0.5f - timeRemaining) / 0.5f * 60.0f;
                    DrawRectangleRec(p->bounds, (Color){35, 100, 55, (unsigned char)alpha});
                }
                continue;
            }
        } else if (p->type == PLATFORM_MOVING) {
            platColor = GetPlatformColor(p);
            borderColor = (Color){150, 160, 200, 255};  // Slight blue highlight for moving
        } else {
            platColor = GetPlatformColor(p);
            borderColor = GetPlatformBorderColor(p);
        }
        
        // Draw platform body
        DrawRectangleRec(p->bounds, platColor);
        
        // Top surface highlight
        Color topHighlight = (Color){
            (unsigned char)(platColor.r + 30),
            (unsigned char)(platColor.g + 30),
            (unsigned char)(platColor.b + 30),
            255
        };
        DrawRectangle((int)p->bounds.x, (int)p->bounds.y, (int)p->bounds.width, 3, topHighlight);
        
        // Border
        DrawRectangleLinesEx(p->bounds, 2, borderColor);
        
        // Moving platform indicator (small arrows)
        if (p->type == PLATFORM_MOVING) {
            float cx = p->bounds.x + p->bounds.width / 2;
            float cy = p->bounds.y + p->bounds.height / 2;
            Color arrowColor = (Color){180, 190, 220, 180};
            
            // Horizontal movement indicator
            if (fabsf(p->endPos.x - p->startPos.x) > 10) {
                DrawTriangle(
                    (Vector2){cx - 15, cy},
                    (Vector2){cx - 8, cy - 4},
                    (Vector2){cx - 8, cy + 4},
                    arrowColor
                );
                DrawTriangle(
                    (Vector2){cx + 15, cy},
                    (Vector2){cx + 8, cy + 4},
                    (Vector2){cx + 8, cy - 4},
                    arrowColor
                );
            }
            // Vertical movement indicator
            if (fabsf(p->endPos.y - p->startPos.y) > 10) {
                DrawTriangle(
                    (Vector2){cx, cy - 6},
                    (Vector2){cx - 4, cy},
                    (Vector2){cx + 4, cy},
                    arrowColor
                );
                DrawTriangle(
                    (Vector2){cx, cy + 6},
                    (Vector2){cx + 4, cy},
                    (Vector2){cx - 4, cy},
                    arrowColor
                );
            }
        }
    }
    
    // Draw goal with glow effect
    float glowIntensity = 0.5f + 0.5f * sinf(glowTimer * 1.5f);
    
    // Outer glow
    Rectangle goalGlow = currentLevel.goalArea;
    goalGlow.x -= 8;
    goalGlow.y -= 8;
    goalGlow.width += 16;
    goalGlow.height += 16;
    DrawRectangleRec(goalGlow, (Color){50, 200, 80, (unsigned char)(30 + glowIntensity * 40)});
    
    // Middle glow
    Rectangle goalMid = currentLevel.goalArea;
    goalMid.x -= 4;
    goalMid.y -= 4;
    goalMid.width += 8;
    goalMid.height += 8;
    DrawRectangleRec(goalMid, (Color){60, 220, 90, (unsigned char)(50 + glowIntensity * 50)});
    
    // Goal body
    Color goalColor = (Color){70, 200, 100, 230};
    DrawRectangleRec(currentLevel.goalArea, goalColor);
    
    // Goal text
    const char* goalText = "GOAL";
    int textWidth = MeasureText(goalText, 18);
    int textX = (int)(currentLevel.goalArea.x + currentLevel.goalArea.width/2 - textWidth/2);
    int textY = (int)(currentLevel.goalArea.y + currentLevel.goalArea.height/2 - 9);
    DrawText(goalText, textX, textY, 18, WHITE);
    
    // Goal border
    DrawRectangleLinesEx(currentLevel.goalArea, 2, (Color){100, 255, 130, 255});
}

Platform* Level_GetPlatforms(int* count) {
    *count = currentLevel.platformCount;
    return currentLevel.platforms;
}

Vector2 Level_GetSpawnPosition(void) {
    return currentLevel.spawnPosition;
}

bool Level_IsAnchorDisabledZone(Vector2 position) {
    for (int i = 0; i < currentLevel.disabledZoneCount; i++) {
        if (CheckCollisionPointRec(position, currentLevel.disabledZones[i].bounds)) {
            return true;
        }
    }
    return false;
}

bool Level_CheckGoalCollision(Rectangle playerBounds) {
    if (CheckCollisionRecs(playerBounds, currentLevel.goalArea)) {
        Level_Complete();
        return true;
    }
    return false;
}

bool Level_CheckSpikeCollision(Rectangle playerBounds) {
    for (int i = 0; i < currentLevel.spikeCount; i++) {
        Spike* spike = &currentLevel.spikes[i];
        if (!spike->isActive) continue;
        
        // Calculate spike collision rectangle based on direction
        Rectangle spikeRect;
        switch (spike->direction) {
            case SPIKE_UP:  // Floor spike pointing up
                spikeRect = (Rectangle){
                    spike->position.x - spike->width/2,
                    spike->position.y - spike->height,
                    spike->width,
                    spike->height
                };
                break;
            case SPIKE_DOWN:  // Ceiling spike pointing down
                spikeRect = (Rectangle){
                    spike->position.x - spike->width/2,
                    spike->position.y,
                    spike->width,
                    spike->height
                };
                break;
            case SPIKE_LEFT:  // Wall spike pointing left
                spikeRect = (Rectangle){
                    spike->position.x - spike->height,
                    spike->position.y - spike->width/2,
                    spike->height,
                    spike->width
                };
                break;
            case SPIKE_RIGHT:  // Wall spike pointing right
                spikeRect = (Rectangle){
                    spike->position.x,
                    spike->position.y - spike->width/2,
                    spike->height,
                    spike->width
                };
                break;
        }
        
        if (CheckCollisionRecs(playerBounds, spikeRect)) {
            return true;
        }
    }
    return false;
}

void Level_Complete(void) {
    currentLevel.isCompleted = true;
    levelCompletionStatus[currentLevel.levelIndex] = true;
    
    // Play level complete sound (once per completion)
    AudioManager_PlaySound(SOUND_LEVEL_COMPLETE);
    
    // Check if all levels complete
    bool allComplete = true;
    for (int i = 0; i < TOTAL_LEVELS; i++) {
        if (!levelCompletionStatus[i]) {
            allComplete = false;
            break;
        }
    }
    
    if (allComplete) {
        GameState_SetCurrent(STATE_VICTORY);
    } else {
        GameState_SetCurrent(STATE_LEVEL_COMPLETE);
    }
}

int Level_GetCurrentIndex(void) {
    return currentLevel.levelIndex;
}

bool Level_IsCompleted(int levelIndex) {
    if (levelIndex < 0 || levelIndex >= TOTAL_LEVELS) return false;
    return levelCompletionStatus[levelIndex];
}

// ============================================================================
// Level Data Definitions
// ============================================================================

static void Level_LoadLevel1(void) {
    // Level 1: First Anchor - Simple introduction
    currentLevel.spawnPosition = (Vector2){100, 500};
    
    // Ground platform (left side) - stone
    currentLevel.platforms[0] = CreatePlatform(
        (Rectangle){0, 600, 400, 40},
        PLATFORM_SOLID, MATERIAL_STONE
    );
    
    // Ground platform (right side - after gap) - stone
    currentLevel.platforms[1] = CreatePlatform(
        (Rectangle){600, 600, 680, 40},
        PLATFORM_SOLID, MATERIAL_STONE
    );
    
    // Goal platform - metal
    currentLevel.platforms[2] = CreatePlatform(
        (Rectangle){1100, 500, 150, 20},
        PLATFORM_SOLID, MATERIAL_METAL
    );
    
    currentLevel.platformCount = 3;
    
    // SPIKES: Full horizontal bottom coverage (entire screen width)
    float spikeY = 700.0f;
    float spikeWidth = 32.0f;
    float spikeHeight = 35.0f;
    float spikeSpacing = 36.0f;
    
    for (float x = spikeWidth/2; x < 1280; x += spikeSpacing) {
        AddSpike(x, spikeY, spikeWidth, spikeHeight);
    }
    
    // Goal area
    currentLevel.goalArea = (Rectangle){1150, 420, 80, 80};
}

static void Level_LoadLevel2(void) {
    // Level 2: Vertical Thinking - Climbing challenge
    currentLevel.spawnPosition = (Vector2){100, 600};
    
    // Ground - ancient stone
    currentLevel.platforms[0] = CreatePlatform(
        (Rectangle){0, 650, 300, 40},
        PLATFORM_SOLID, MATERIAL_ANCIENT
    );
    
    // Vertical climbing platforms - alternating materials
    currentLevel.platforms[1] = CreatePlatform(
        (Rectangle){200, 550, 120, 20},
        PLATFORM_SOLID, MATERIAL_STONE
    );
    
    currentLevel.platforms[2] = CreatePlatform(
        (Rectangle){350, 450, 120, 20},
        PLATFORM_SOLID, MATERIAL_METAL
    );
    
    currentLevel.platforms[3] = CreatePlatform(
        (Rectangle){200, 350, 120, 20},
        PLATFORM_SOLID, MATERIAL_STONE
    );
    
    currentLevel.platforms[4] = CreatePlatform(
        (Rectangle){350, 250, 120, 20},
        PLATFORM_SOLID, MATERIAL_METAL
    );
    
    currentLevel.platforms[5] = CreatePlatform(
        (Rectangle){200, 150, 200, 20},
        PLATFORM_SOLID, MATERIAL_ANCIENT
    );
    
    currentLevel.platformCount = 6;
    
    // SPIKES: Full horizontal bottom coverage (entire screen width)
    float spikeY = 720.0f;
    float spikeWidth = 32.0f;
    float spikeHeight = 35.0f;
    float spikeSpacing = 36.0f;
    
    for (float x = spikeWidth/2; x < 1280; x += spikeSpacing) {
        AddSpike(x, spikeY, spikeWidth, spikeHeight);
    }
    
    currentLevel.goalArea = (Rectangle){300, 70, 80, 80};
}

static void Level_LoadLevel3(void) {
    // Level 3: Commitment - Moving platforms and disabled zones
    currentLevel.spawnPosition = (Vector2){100, 550};
    
    // Starting ground - ancient
    currentLevel.platforms[0] = CreatePlatform(
        (Rectangle){0, 600, 250, 40},
        PLATFORM_SOLID, MATERIAL_ANCIENT
    );
    
    // Moving platform - metal
    currentLevel.platforms[1] = CreateMovingPlatform(
        (Rectangle){300, 550, 100, 20},
        (Vector2){300, 550}, (Vector2){500, 550},
        0.3f, MATERIAL_METAL
    );
    
    // Middle platform (in disabled zone) - stone
    currentLevel.platforms[2] = CreatePlatform(
        (Rectangle){550, 600, 200, 40},
        PLATFORM_SOLID, MATERIAL_STONE
    );
    
    // Another moving platform - metal
    currentLevel.platforms[3] = CreateMovingPlatform(
        (Rectangle){800, 500, 100, 20},
        (Vector2){800, 500}, (Vector2){800, 350},
        0.4f, MATERIAL_METAL
    );
    
    // Final platform - ancient
    currentLevel.platforms[4] = CreatePlatform(
        (Rectangle){950, 300, 200, 40},
        PLATFORM_SOLID, MATERIAL_ANCIENT
    );
    
    currentLevel.platformCount = 5;
    
    // Disabled zone over middle platform
    currentLevel.disabledZones[0] = (DisabledZone){
        .bounds = (Rectangle){500, 400, 300, 250}
    };
    currentLevel.disabledZoneCount = 1;
    
    // SPIKES inside red zone (disabled zone) - VERTICAL WALL SPIKES
    // These are always active and cause instant death on contact (same logic as floor spikes)
    // Red zone bounds: x=500, y=400, width=300, height=250 (so x extends to 800)
    // Place vertical spikes on the right inner wall of the red zone, pointing LEFT
    AddDirectionalSpike(790.0f, 500.0f, 32.0f, 35.0f, SPIKE_LEFT);  // Upper spike
    AddDirectionalSpike(790.0f, 560.0f, 32.0f, 35.0f, SPIKE_LEFT);  // Lower spike
    
    // SPIKES: Full horizontal bottom coverage (entire screen width)
    float spikeY = 700.0f;
    float spikeWidth = 32.0f;
    float spikeHeight = 35.0f;
    float spikeSpacing = 36.0f;
    
    for (float x = spikeWidth/2; x < 1280; x += spikeSpacing) {
        AddSpike(x, spikeY, spikeWidth, spikeHeight);
    }
    
    currentLevel.goalArea = (Rectangle){1050, 220, 80, 80};
}

static void Level_LoadLevel4(void) {
    // Level 4: Timed Platforms - TWO green platforms that toggle ON/OFF
    currentLevel.spawnPosition = (Vector2){100, 550};
    
    // Starting ground - stone
    currentLevel.platforms[0] = CreatePlatform(
        (Rectangle){0, 600, 300, 40},
        PLATFORM_SOLID, MATERIAL_STONE
    );
    
    // TIMED PLATFORM #1 (lower) - toggles ON (2.5s) / OFF (2.5s)
    currentLevel.platforms[1] = CreateTimedPlatform(
        (Rectangle){400, 550, 150, 20},
        2.5f,   // ON duration
        2.5f,   // OFF duration  
        true,   // Start ON
        MATERIAL_METAL
    );
    
    // Middle platform - ancient (safe landing zone)
    currentLevel.platforms[2] = CreatePlatform(
        (Rectangle){600, 500, 200, 40},
        PLATFORM_SOLID, MATERIAL_ANCIENT
    );
    
    // TIMED PLATFORM #2 (upper-right) - toggles ON (2.0s) / OFF (2.0s)
    // Offset timing: starts OFF so player must wait
    currentLevel.platforms[3] = CreateTimedPlatform(
        (Rectangle){850, 400, 150, 20},
        2.0f,   // ON duration
        2.0f,   // OFF duration  
        false,  // Start OFF - player must wait!
        MATERIAL_METAL
    );
    
    // Final platform - metal
    currentLevel.platforms[4] = CreatePlatform(
        (Rectangle){1050, 350, 200, 40},
        PLATFORM_SOLID, MATERIAL_METAL
    );
    
    currentLevel.platformCount = 5;
    
    // SPIKES: Full horizontal bottom coverage (entire screen width)
    float spikeY = 700.0f;
    float spikeWidth = 32.0f;
    float spikeHeight = 35.0f;
    float spikeSpacing = 36.0f;
    
    for (float x = spikeWidth/2; x < 1280; x += spikeSpacing) {
        AddSpike(x, spikeY, spikeWidth, spikeHeight);
    }
    
    currentLevel.goalArea = (Rectangle){1100, 270, 80, 80};
}

static void Level_LoadLevel5(void) {
    // Level 5: Final Loop - All mechanics combined with TIMED PLATFORMS
    currentLevel.spawnPosition = (Vector2){100, 550};
    
    // Starting area - ancient
    currentLevel.platforms[0] = CreatePlatform(
        (Rectangle){0, 600, 200, 40},
        PLATFORM_SOLID, MATERIAL_ANCIENT
    );
    
    // Moving platform over gap - metal
    currentLevel.platforms[1] = CreateMovingPlatform(
        (Rectangle){250, 580, 100, 20},
        (Vector2){250, 580}, (Vector2){400, 580},
        0.35f, MATERIAL_METAL
    );
    
    // Platform in disabled zone - stone
    currentLevel.platforms[2] = CreatePlatform(
        (Rectangle){450, 600, 150, 40},
        PLATFORM_SOLID, MATERIAL_STONE
    );
    
    // TIMED PLATFORM #1 - toggles ON (2.5s) / OFF (2.5s)
    currentLevel.platforms[3] = CreateTimedPlatform(
        (Rectangle){650, 520, 120, 20},
        2.5f,   // ON duration
        2.5f,   // OFF duration  
        true,   // Start ON
        MATERIAL_METAL
    );
    
    // Vertical section - stone
    currentLevel.platforms[4] = CreatePlatform(
        (Rectangle){800, 450, 100, 20},
        PLATFORM_SOLID, MATERIAL_STONE
    );
    
    // Moving platform - metal
    currentLevel.platforms[5] = CreateMovingPlatform(
        (Rectangle){700, 350, 100, 20},
        (Vector2){700, 350}, (Vector2){850, 350},
        0.4f, MATERIAL_METAL
    );
    
    // TIMED PLATFORM #2 - toggles ON (2.0s) / OFF (2.0s)
    // Offset timing: starts OFF
    currentLevel.platforms[6] = CreateTimedPlatform(
        (Rectangle){900, 280, 120, 20},
        2.0f,   // ON duration
        2.0f,   // OFF duration  
        false,  // Start OFF - player must wait
        MATERIAL_METAL
    );
    
    // Final platform - ancient
    currentLevel.platforms[7] = CreatePlatform(
        (Rectangle){1050, 200, 200, 40},
        PLATFORM_SOLID, MATERIAL_ANCIENT
    );
    
    currentLevel.platformCount = 8;
    
    // Disabled zones
    currentLevel.disabledZones[0] = (DisabledZone){
        .bounds = (Rectangle){420, 450, 200, 200}
    };
    currentLevel.disabledZoneCount = 1;
    
    // VERTICAL SPIKE: On the right edge of the gray platform inside the red zone
    // Platform is at x=450-600, y=600 (height 40)
    // Spike is flush against the right edge of the platform, at platform level
    // Points left (SPIKE_LEFT) - kills on any contact (same logic as floor spikes)
    AddDirectionalSpike(600.0f, 600.0f, 32.0f, 35.0f, SPIKE_LEFT);
    
    // SPIKES: Full horizontal bottom coverage (entire screen width)
    float spikeY = 700.0f;
    float spikeWidth = 32.0f;
    float spikeHeight = 35.0f;
    float spikeSpacing = 36.0f;
    
    for (float x = spikeWidth/2; x < 1280; x += spikeSpacing) {
        AddSpike(x, spikeY, spikeWidth, spikeHeight);
    }
    
    currentLevel.goalArea = (Rectangle){1100, 120, 80, 80};
}
