// anchor.c - time anchor mechanic (place and teleport back)

#include "anchor.h"
#include "level.h"
#include "audio_manager.h"
#include <math.h>

static TimeAnchor anchor;

void Anchor_Init(void) {
    anchor.position = (Vector2){0, 0};
    anchor.isPlaced = false;
    anchor.pulseTimer = 0.0f;
    anchor.canPlaceHere = true;
}

void Anchor_Update(float dt) {
    if (!anchor.isPlaced) return;
    anchor.pulseTimer += dt * ANCHOR_PULSE_SPEED;
    if (anchor.pulseTimer > 2 * PI) anchor.pulseTimer -= 2 * PI;
}

void Anchor_Draw(void) {
    if (!anchor.isPlaced) return;
    
    float pulse = sinf(anchor.pulseTimer);
    float outerR = ANCHOR_RADIUS + pulse * 4;
    int x = (int)anchor.position.x;
    int y = (int)anchor.position.y;
    
    // glow ring
    DrawCircle(x, y, outerR + 8, (Color){255, 215, 0, 100});
    DrawCircle(x, y, outerR, (Color){255, 200, 50, 180});
    DrawCircle(x, y, ANCHOR_RADIUS - 4, (Color){255, 255, 200, 255});
    DrawCircle(x, y, 4, GOLD);
}

void Anchor_Place(Vector2 pos) {
    if (!Anchor_CanPlaceAt(pos)) return;
    
    anchor.position = pos;
    anchor.position.y += 24;  // offset to ground level
    anchor.isPlaced = true;
    anchor.pulseTimer = 0.0f;
}

void Anchor_Remove(void) { anchor.isPlaced = false; }

bool Anchor_IsPlaced(void) { return anchor.isPlaced; }

Vector2 Anchor_GetPosition(void) {
    Vector2 spawnPos = anchor.position;
    spawnPos.y -= 24;  // reverse ground offset
    return spawnPos;
}

bool Anchor_CanPlaceAt(Vector2 pos) {
    return !Level_IsAnchorDisabledZone(pos);
}
