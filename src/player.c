// player.c - handles player movement and physics

#include "player.h"
#include "audio_manager.h"
#include <math.h>

static Player gPlayer;
static Vector2 gSpawnPos = { 100.0f, 500.0f };

// smoothly move current towards target
static float Approach(float current, float target, float step)
{
    float diff = target - current;
    if (fabsf(diff) <= step) return target;
    return current + (diff > 0 ? step : -step);
}

void Player_Init(void)
{
    gPlayer.width  = PLAYER_WIDTH;
    gPlayer.height = PLAYER_HEIGHT;
    gPlayer.position = gSpawnPos;
    gPlayer.velocity = (Vector2){0};
    gPlayer.isGrounded = false;
    gPlayer.isJumping  = false;
    gPlayer.facingRight = true;
    gPlayer.coyoteTimer = 0.0f;
    gPlayer.jumpBufferTimer = 0.0f;
}

void Player_Respawn(void)
{
    gPlayer.position = gSpawnPos;
    gPlayer.velocity = (Vector2){0};
    gPlayer.isGrounded = false;
    gPlayer.isJumping = false;
    gPlayer.coyoteTimer = 0.0f;
    gPlayer.jumpBufferTimer = 0.0f;
}

void Player_Update(float dt)
{
    if (dt <= 0.0f) return;
    if (dt > 0.05f) dt = 0.05f;  // clamp big frame spikes

    // horizontal input
    float inputX = 0.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  inputX -= 1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) inputX += 1.0f;

    if (inputX != 0.0f) gPlayer.facingRight = (inputX > 0.0f);

    float targetVx = inputX * PLAYER_SPEED;
    float acc = (inputX != 0.0f) ? PLAYER_ACCELERATION : PLAYER_FRICTION;
    float goal = (inputX != 0.0f) ? targetVx : 0.0f;
    gPlayer.velocity.x = Approach(gPlayer.velocity.x, goal, acc * dt);

    // coyote timer lets you jump briefly after leaving ground
    if (gPlayer.isGrounded) 
        gPlayer.coyoteTimer = COYOTE_TIME;
    else 
        gPlayer.coyoteTimer = fmaxf(0.0f, gPlayer.coyoteTimer - dt);

    // jump buffer stores jump input for a short time
    bool wantJump = IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W);
    if (wantJump) 
        gPlayer.jumpBufferTimer = JUMP_BUFFER;
    else 
        gPlayer.jumpBufferTimer = fmaxf(0.0f, gPlayer.jumpBufferTimer - dt);

    // jump executes if buffer and coyote are both active
    if (gPlayer.jumpBufferTimer > 0.0f && gPlayer.coyoteTimer > 0.0f) {
        gPlayer.velocity.y = PLAYER_JUMP_VELOCITY;
        gPlayer.isGrounded = false;
        gPlayer.isJumping = true;
        gPlayer.coyoteTimer = 0.0f;
        gPlayer.jumpBufferTimer = 0.0f;
        AudioManager_PlaySound(SOUND_JUMP);
    }

    // variable jump: release early = shorter jump
    bool holdingJump = IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_W);
    if (!holdingJump && gPlayer.velocity.y < 0.0f) {
        gPlayer.velocity.y *= JUMP_CUT_MULT;
    }

    // gravity
    gPlayer.velocity.y += PLAYER_GRAVITY * dt;
    if (gPlayer.velocity.y > PLAYER_MAX_FALL_SPEED) 
        gPlayer.velocity.y = PLAYER_MAX_FALL_SPEED;

    // update position (collision handled separately)
    gPlayer.position.x += gPlayer.velocity.x * dt;
    gPlayer.position.y += gPlayer.velocity.y * dt;

    if (gPlayer.isGrounded) gPlayer.isJumping = false;
}

void Player_Draw(void)
{
    Rectangle r = Player_GetBounds();
    DrawRectangleRec(r, (Color){ 255, 220, 50, 255 });

    // small triangle shows facing direction
    float cx = r.x + r.width/2.0f;
    float cy = r.y + r.height/2.0f;
    float dirX = gPlayer.facingRight ? 10.0f : -10.0f;
    DrawTriangle(
        (Vector2){ cx + dirX, cy },
        (Vector2){ cx, cy - 6.0f },
        (Vector2){ cx, cy + 6.0f },
        (Color){ 255, 255, 255, 200 }
    );
}

Vector2 Player_GetPosition(void) { return gPlayer.position; }

void Player_SetPosition(Vector2 pos) { gPlayer.position = pos; }

void Player_ResetVelocity(void) { gPlayer.velocity = (Vector2){0}; }

Rectangle Player_GetBounds(void)
{
    return (Rectangle){
        gPlayer.position.x,
        gPlayer.position.y,
        gPlayer.width,
        gPlayer.height
    };
}

bool Player_IsGrounded(void) { return gPlayer.isGrounded; }

void Player_SetGrounded(bool grounded)
{
    gPlayer.isGrounded = grounded;
    // stop downward velocity when landing
    if (grounded && gPlayer.velocity.y > 0.0f) {
        gPlayer.velocity.y = 0.0f;
    }
}
