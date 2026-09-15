/**
 * audio_manager.h - Audio system management
 */

#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

typedef enum {
    SOUND_ANCHOR_PLACE,
    SOUND_ANCHOR_TELEPORT,
    SOUND_JUMP,
    SOUND_SPIKE_DEATH,
    SOUND_LEVEL_COMPLETE,
    SOUND_MENU_MOVE,
    SOUND_MENU_SELECT,
    SOUND_LEVEL_TRANSITION,
    SOUND_ERROR,
    SOUND_COUNT
} SoundType;

// Initialize audio manager
void AudioManager_Init(void);

// Cleanup audio manager
void AudioManager_Cleanup(void);

// Play a sound effect
void AudioManager_PlaySound(SoundType type);

// Set sound volume (0.0 to 1.0)
void AudioManager_SetSoundVolume(float volume);

// Set music volume (0.0 to 1.0)
void AudioManager_SetMusicVolume(float volume);

#endif // AUDIO_MANAGER_H
