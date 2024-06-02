#pragma once

#include "framework/extra/bass.h"
#include "includes.h"
#include "framework.h"

#include <map>

class Audio {
private:

	// Handler for BASS_SampleLoad
	HSAMPLE sample;

	// Load sample from disk
	bool load(const std::string& filename, uint8_t flags = 0);

	// Play audio and return the channel
	HCHANNEL play(float volume = 1.0f);

	// Map to store audios loaded
	static std::map<std::string, Audio*> sAudiosLoaded;

public:

	Audio();
	~Audio();

	// Initialize BASS
	static bool Init();

	// Close BASS
	static void Destroy();

	// Get from manager map
	static Audio* Get(const std::string& filename, uint8_t flags = 0);

	// Play Manager API
	static HCHANNEL Play(const std::string& filename, float volume = 1.0f, uint8_t flags = 0);
	static HCHANNEL Play3D(const std::string& filename, Vector3 position, float volume = 1.0f);

	// Stop sounds
	static bool Stop(HCHANNEL channel);
};