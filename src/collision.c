// collision.c - checks and resolves player vs platform collisions

#include <math.h>
#include "collision.h"
#include "player.h"
#include "level.h"
#include "game_state.h"

bool Collision_CheckRectRect(Rectangle a, Rectangle b) {
    return CheckCollisionRecs(a, b);
}

bool Collision_CheckPointRect(Vector2 point, Rectangle rect) {
    return CheckCollisionPointRec(point, rect);
}

void Collision_Update(void) {
    Collision_ResolvePlayerPlatforms();
    
    Rectangle pBounds = Player_GetBounds();
    
    if (Level_CheckSpikeCollision(pBounds)) {
        GameState_TriggerFail();
        return;
    }
    
    Level_CheckGoalCollision(pBounds);
}

void Collision_ResolvePlayerPlatforms(void) {
    Rectangle pBounds = Player_GetBounds();
    Vector2 pPos = Player_GetPosition();
    
    int count;
    Platform* platforms = Level_GetPlatforms(&count);
    
    bool onGround = false;
    
    for (int i = 0; i < count; i++) {
        Platform* plat = &platforms[i];
        
        if (!plat->isActive) continue;
        
        if (!Collision_CheckRectRect(pBounds, plat->bounds)) continue;
        
        // find overlap on each side
        float oLeft = (pBounds.x + pBounds.width) - plat->bounds.x;
        float oRight = (plat->bounds.x + plat->bounds.width) - pBounds.x;
        float oTop = (pBounds.y + pBounds.height) - plat->bounds.y;
        float oBottom = (plat->bounds.y + plat->bounds.height) - pBounds.y;
        
        // pick smallest overlap to resolve
        float minX = (oLeft < oRight) ? -oLeft : oRight;
        float minY = (oTop < oBottom) ? -oTop : oBottom;
        
        if (fabsf(minX) < fabsf(minY)) {
            pPos.x += minX;
        } else {
            pPos.y += minY;
            if (minY < 0) onGround = true;  // landed on top
        }
        
        Player_SetPosition(pPos);
        pBounds = Player_GetBounds();
    }
    
    Player_SetGrounded(onGround);
}
