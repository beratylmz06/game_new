// player.h - player struct and movement constants

#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include <stdbool.h>

#define PLAYER_WIDTH  32
#define PLAYER_HEIGHT 48

// movement tuning
#define PLAYER_SPEED         360.0f
#define PLAYER_ACCELERATION  1800.0f
#define PLAYER_FRICTION      1300.0f
#define PLAYER_JUMP_VELOCITY -520.0f
#define PLAYER_GRAVITY       1100.0f
#define PLAYER_MAX_FALL_SPEED 750.0f

// jump helpers - makes platforming feel better
#define COYOTE_TIME   0.10f   // can still jump shortly after leaving ground
#define JUMP_BUFFER   0.10f   // pressing jump slightly early still works
#define JUMP_CUT_MULT 0.45f   // releasing jump early = shorter jump

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float width;
    float height;
    bool isGrounded;
    bool isJumping;
    bool facingRight;
    float coyoteTimer;
    float jumpBufferTimer;
} Player;

void Player_Init(void);
void Player_Update(float deltaTime);
void Player_Draw(void);
Vector2 Player_GetPosition(void);
void Player_SetPosition(Vector2 pos);
void Player_ResetVelocity(void);
Rectangle Player_GetBounds(void);
bool Player_IsGrounded(void);
void Player_SetGrounded(bool grounded);
void Player_Respawn(void);

#endif
