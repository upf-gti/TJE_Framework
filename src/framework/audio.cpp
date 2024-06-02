#include "audio.h"

std::map<std::string, Audio*> Audio::sAudiosLoaded;

Audio::Audio()
{
	sample = 0;
}

Audio::~Audio()
{
	BASS_SampleFree(sample);
}

// Use "BASS_SAMPLE_LOOP" in flags to play audio as a loop
bool Audio::load(const std::string& filename, uint8_t flags)
{
	sample = BASS_SampleLoad(
		false,  				// From internal memory
		filename.c_str(),		// Filepath
		0,						// Offset
		0,						// Length
		3,						// Max playbacks
		flags					// Flags
	);

	return sample != 0;
}

HCHANNEL Audio::play(float volume)
{
	// Store sample channel in handler
	HCHANNEL channel = BASS_SampleGetChannel(sample, false);

	// Change channel volume
	BASS_ChannelSetAttribute(channel, BASS_ATTRIB_VOL, volume);

	// Play channel
	BASS_ChannelPlay(channel, true);

	return channel;
}

bool Audio::Init()
{
	if (BASS_Init(-1, 44100, 0, 0, NULL) == false) {
		// Error with sound device
		return false;
	}

	return true;
}

void Audio::Destroy()
{
	BASS_Free();
}

Audio* Audio::Get(const std::string& filename, uint8_t flags)
{
	std::map<std::string, Audio*>::iterator it = sAudiosLoaded.find(filename);
	if (it != sAudiosLoaded.end())
		return it->second;

	Audio* audio = new Audio();
	if (!audio->load(filename, flags))
		return nullptr;

	sAudiosLoaded[filename] = audio;

	return audio;
}

HCHANNEL Audio::Play(const std::string& filename, float volume, uint8_t flags)
{
	Audio* audio = Audio::Get(filename, flags);

	if (!audio) {
		return 0;
	}

	return audio->play(volume);
}

// Make sure the sample has been loaded using "BASS_SAMPLE_3D" flag
HCHANNEL Audio::Play3D(const std::string& filename, Vector3 pos, float volume)
{
	Audio* audio = Audio::Get(filename, BASS_SAMPLE_3D);

	if (!audio) {
		return 0;
	}

	HCHANNEL channel = audio->play(volume);

	// Set audio position in space
	BASS_3DVECTOR bass_position = { pos.x, pos.y, pos.z };
	BASS_ChannelSet3DPosition(channel, &bass_position, NULL, NULL);

	// Apply changes to 3D system
	BASS_Apply3D();

	return channel;
}

bool Audio::Stop(HCHANNEL channel)
{
	return BASS_ChannelStop(channel);
}