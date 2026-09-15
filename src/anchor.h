/**
 * anchor.h - Time Anchor mechanic
 */

#ifndef ANCHOR_H
#define ANCHOR_H

#include "raylib.h"

// Anchor visual settings
#define ANCHOR_RADIUS 16
#define ANCHOR_PULSE_SPEED 3.0f

typedef struct {
    Vector2 position;
    bool isPlaced;
    float pulseTimer;     // For visual effect
    bool canPlaceHere;    // Check for disabled zones
} TimeAnchor;

// Initialize anchor system
void Anchor_Init(void);

// Update anchor (visual effects)
void Anchor_Update(float deltaTime);

// Draw anchor marker
void Anchor_Draw(void);

// Place anchor at position
void Anchor_Place(Vector2 position);

// Remove anchor
void Anchor_Remove(void);

// Check if anchor is placed
bool Anchor_IsPlaced(void);

// Get anchor position
Vector2 Anchor_GetPosition(void);

// Check if anchor can be placed at position (not in disabled zone)
bool Anchor_CanPlaceAt(Vector2 position);

#endif // ANCHOR_H
