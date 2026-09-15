// audio_manager.c - loads and plays sound effects

#include "audio_manager.h"
#include "raylib.h"
#include <stdio.h>

static Sound sounds[SOUND_COUNT];
static bool loaded[SOUND_COUNT] = {false};
static float sfxVol = 0.7f;
static float musicVol = 0.5f;
static bool initialized = false;

static bool TryLoad(SoundType type, const char* path) {
    if (FileExists(path)) {
        sounds[type] = LoadSound(path);
        loaded[type] = true;
        return true;
    }
    loaded[type] = false;
    return false;
}

void AudioManager_Init(void) {
    TryLoad(SOUND_JUMP, "assets/sounds/jump.wav");
    TryLoad(SOUND_SPIKE_DEATH, "assets/sounds/spike_death.wav");
    TryLoad(SOUND_LEVEL_COMPLETE, "assets/sounds/level_complete.wav");
    TryLoad(SOUND_MENU_MOVE, "assets/sounds/menu_move.wav");
    TryLoad(SOUND_MENU_SELECT, "assets/sounds/menu_select.wav");
    TryLoad(SOUND_LEVEL_TRANSITION, "assets/sounds/level_transition.wav");
    TryLoad(SOUND_ANCHOR_PLACE, "assets/sounds/anchor_place.wav");
    TryLoad(SOUND_ANCHOR_TELEPORT, "assets/sounds/anchor_teleport.wav");
    TryLoad(SOUND_ERROR, "assets/sounds/error.wav");
    initialized = true;
}

void AudioManager_Cleanup(void) {
    if (!initialized) return;
    for (int i = 0; i < SOUND_COUNT; i++) {
        if (loaded[i]) {
            UnloadSound(sounds[i]);
            loaded[i] = false;
        }
    }
    initialized = false;
}

void AudioManager_PlaySound(SoundType type) {
    if (!initialized) return;
    if (type < 0 || type >= SOUND_COUNT) return;
    if (!loaded[type]) return;
    SetSoundVolume(sounds[type], sfxVol);
    PlaySound(sounds[type]);
}

void AudioManager_SetSoundVolume(float vol) {
    if (vol < 0.0f) vol = 0.0f;
    if (vol > 1.0f) vol = 1.0f;
    sfxVol = vol;
}

void AudioManager_SetMusicVolume(float vol) {
    if (vol < 0.0f) vol = 0.0f;
    if (vol > 1.0f) vol = 1.0f;
    musicVol = vol;
}
