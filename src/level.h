// level.h - level data structures and function declarations

#ifndef LEVEL_H
#define LEVEL_H

#include "raylib.h"
#include <stdbool.h>

#define MAX_PLATFORMS 64
#define MAX_DISABLED_ZONES 16
#define MAX_SPIKES 64

typedef enum {
    MATERIAL_STONE,
    MATERIAL_METAL,
    MATERIAL_ANCIENT
} PlatformMaterial;

typedef enum {
    PLATFORM_SOLID,
    PLATFORM_MOVING,
    PLATFORM_ANCHOR_REACTIVE,  // appears when anchor placed
    PLATFORM_TIMED             // toggles on/off
} PlatformType;

typedef struct {
    Rectangle bounds;
    PlatformType type;
    PlatformMaterial material;
    bool isActive;
    Vector2 startPos;
    Vector2 endPos;
    float speed;
    float progress;
    float timer;
    float onDuration;
    float offDuration;
    bool timedIsOn;
} Platform;

typedef struct {
    Rectangle bounds;
} DisabledZone;

typedef enum {
    SPIKE_UP,
    SPIKE_LEFT,
    SPIKE_RIGHT,
    SPIKE_DOWN
} SpikeDirection;

typedef struct {
    Vector2 position;
    float width;
    float height;
    bool isActive;
    SpikeDirection direction;
} Spike;

typedef struct {
    int levelIndex;
    Platform platforms[MAX_PLATFORMS];
    int platformCount;
    DisabledZone disabledZones[MAX_DISABLED_ZONES];
    int disabledZoneCount;
    Spike spikes[MAX_SPIKES];
    int spikeCount;
    Vector2 spawnPosition;
    Rectangle goalArea;
    bool isCompleted;
} Level;

void Level_Init(void);
void Level_Load(int levelIndex);
void Level_Update(float deltaTime);
void Level_Draw(void);
Platform* Level_GetPlatforms(int* count);
Vector2 Level_GetSpawnPosition(void);
bool Level_IsAnchorDisabledZone(Vector2 position);
bool Level_CheckGoalCollision(Rectangle playerBounds);
void Level_Complete(void);
int Level_GetCurrentIndex(void);
bool Level_IsCompleted(int levelIndex);
bool Level_CheckSpikeCollision(Rectangle playerBounds);
void Level_DrawBackground(void);

#endif
