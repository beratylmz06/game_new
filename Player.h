#pragma once
#include "raylib.h"
#include <vector> 


enum PlatformType {
    PLATFORM_BOTH = 0, 
    PLATFORM_RED = 1,  
    PLATFORM_BLUE = 2  
};


struct Platform {
    Rectangle rect;
    PlatformType type;
    Color color;
};

class Player {
public:
    Vector2 position;
    Vector2 velocity;
    bool isGrounded;

    float speed = 300.0f;
    float gravity = 1000.0f;
    float jumpForce = 550.0f;

    Player();
    
    void Update(float deltaTime, std::vector<Platform>& platforms, int currentWorld);
    void Draw();
};