#include "Player.h"

Player::Player() {
    position = { 100, 300 };
    velocity = { 0, 0 };
    isGrounded = false;
}

void Player::Update(float deltaTime, std::vector<Platform>& platforms, int currentWorld) {
   
    if (IsKeyDown(KEY_RIGHT)) velocity.x = speed;
    else if (IsKeyDown(KEY_LEFT)) velocity.x = -speed;
    else velocity.x = 0;

    
    if (isGrounded && IsKeyPressed(KEY_UP)) {
        velocity.y = -jumpForce;
        isGrounded = false;
    }

    
    velocity.y += gravity * deltaTime;

    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;

    isGrounded = false; 

    
    Rectangle playerRect = { position.x, position.y - 40, 40, 40 };

    for (Platform& p : platforms) {
       
        if (p.type != PLATFORM_BOTH && p.type != currentWorld) continue;

       
        if (CheckCollisionRecs(playerRect, p.rect)) {
            
            if (velocity.y > 0 && (position.y - 40) < p.rect.y) {
                velocity.y = 0;
                position.y = p.rect.y; 
                isGrounded = true;
            }
        }
    }

    
    if (position.y > 800){
        position = { 100, 300 }; 
        velocity = { 0, 0 };
    }
}

void Player::Draw() {
    DrawRectangle(position.x, position.y - 40, 40, 40, RAYWHITE); 
}