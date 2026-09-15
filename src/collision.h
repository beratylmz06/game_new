/**
 * collision.h - Collision detection systems
 */

#ifndef COLLISION_H
#define COLLISION_H

#include "raylib.h"

// Check if two rectangles overlap
bool Collision_CheckRectRect(Rectangle a, Rectangle b);

// Check if a point is inside a rectangle
bool Collision_CheckPointRect(Vector2 point, Rectangle rect);

// Update collision system (check player against level)
void Collision_Update(void);

// Resolve player collision with platforms
void Collision_ResolvePlayerPlatforms(void);

#endif // COLLISION_H
