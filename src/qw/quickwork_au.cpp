#include "quickwork_au.h"
#include <alc.h>
#include <efx.h>
#include <efx-creative.h>
#include <efx-presets.h>
#include <alext.h>
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisenc.h>
#include <vorbis/vorbisfile.h>

#include <iostream>
#include <fstream>

using namespace qwau;

ALCdevice *qwau::device = nullptr;
ALCcontext *qwau::context = nullptr;

std::map<std::string, unsigned int> sound_cache = std::map<std::string, unsigned int>();

int qwau::start() {
	ALint attribs[4] = {0};
	device = alcOpenDevice(NULL);
	if(!device)
		return -1;
	if(alcIsExtensionPresent(device, "ALC_EXT_EFX") == AL_FALSE)
		return -1;
	attribs[0] = ALC_MAX_AUXILIARY_SENDS;
	attribs[1] = 4;

	context = alcCreateContext(device, attribs);
	if(!context) {
		alcCloseDevice(device);
		return -1;
	}
	alcMakeContextCurrent(context);

	ALint sends;
	alcGetIntegerv(device, ALC_MAX_AUXILIARY_SENDS, 1, &sends);
	std::cout << "Supports " << sends << " sends" << std::endl;

	alGenEffects = (LPALGENEFFECTS)
		alGetProcAddress("alGenEffects");
	alDeleteEffects = (LPALDELETEEFFECTS)
		alGetProcAddress("alDeleteEffects");
	alIsEffect = (LPALISEFFECT)
		alGetProcAddress("alIsEffect");
	if (!(alGenEffects && alDeleteEffects && alIsEffect))
		return -1;
	return 0;
}

unsigned int qwau::load_sound(const char* filepath) {
	ALuint source, buffer;
	alGenSources(1, &source);
	FILE* file = 0;
	file = fopen(filepath, "rb");
	if (file == 0) {
		std::cout << "Error: Unable to open ogg file \"" << filepath << "\"" << std::endl;
		return -1;
	}
	alGenBuffers(1, &buffer);
	OggVorbis_File vorbis_file;
	ov_open_callbacks(file, &vorbis_file, NULL, 0, OV_CALLBACKS_NOCLOSE);
	vorbis_info* info = ov_info(&vorbis_file, -1);
	
	ALenum format = info->channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
	size_t data_len = ov_pcm_total(&vorbis_file, -1) * info->channels * 2;
	int16_t *pcmout = (int16_t*)malloc(data_len);

	for (size_t size = 0, offset = 0, sel = 0;
		(size = ov_read(&vorbis_file, (char*)pcmout + offset, 4096, 0, 2, 1, (int*)&sel)) != 0; offset += size) {
		if (size < 0) {
			std::cout << "Error: Bad OGG file! \"" << filepath << "\"" << std::endl;
			free(pcmout);
			fclose(file);
			ov_clear(&vorbis_file);
			return -1;
		}
	}

	alBufferData(buffer, format, pcmout, data_len, info->rate);
	free(pcmout);
	fclose(file);
	ov_clear(&vorbis_file);
	alSourcei(source, AL_BUFFER, buffer);
	return source;
}

unsigned int qwau::get_loaded_sound(const char* name) {
	return sound_cache[std::string(name)];
}

void qwau::load_sound_as(const char* path, const char* name) {
	sound_cache[std::string(name)] = load_sound(path);
}