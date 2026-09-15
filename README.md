# Time Anchor

2D puzzle platformer game for SENG479 Game Programming course.

## What is this?

Its a platformer game where you can place an "anchor" and teleport back to it. The idea is you explore dangerous areas and if you fall, you come back to where you placed the anchor.

I used Raylib for graphics and wrote everything in C.

## How to Play

**Movement:**
- A/D or arrow keys to move
- Space to jump (also W and up arrow work)

**Anchor mechanic:**
- Press E to place anchor at your feet
- Press Q to teleport back to anchor
- You can only have one anchor at a time

**Other:**
- ESC to pause
- Complete all 5 levels to win

## How to Run

Just double click `run.bat` in the main folder. If that doesnt work:

1. Open the project in Visual Studio
2. Build it (Ctrl+Shift+B)
3. Run it (F5)

You need raylib installed for building from source.

## Files

```
src/
  main.c         - game loop
  player.c/h     - movement and jumping
  collision.c/h  - platform collision
  anchor.c/h     - the anchor mechanic
  level.c/h      - level data
  game_state.c/h - menus
  audio_manager.c/h - sounds

assets/
  sounds/        - wav files for sfx

build/
  TimeAnchor.exe - the game
```

## What I Implemented

- Player movement with coyote time and jump buffering
- Anchor place/teleport system
- 5 levels with increasing difficulty
- Spikes that kill you
- Timed platforms (they blink on/off)
- Menu and level select
- Sound effects

## Known Issues

- Sometimes the player gets stuck on platform edges (rare)
- No music yet, only sound effects

## Credits

Made by:
- Ahmet Serdar Erünsal
- Elif Uğur
- Berat Yılmaz
- Ayçe Çoban
- Hüseyin Emir Macit

SENG479 Game Programming - Fall 2025-2026

Sound effects from Mixkit: https://mixkit.co/
Raylib library: https://www.raylib.com/
