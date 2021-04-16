
#define _CRT_SECURE_NO_WARNINGS
#include "AudioInstance.h"
#include "AudioEngine.h"
#include <string>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring> 
#include <vector>
#include <memory>

// Source: https://gist.github.com/mudream4869/34541dfbd12a747b027e

ALfloat listenerPos[] = { 0.0,0.0,4.0 };
ALfloat listenerVel[] = { 0.0,0.0,0.0 };
ALfloat listenerOri[] = { 0.0,0.0,1.0, 0.0,1.0,0.0 };

ALfloat source0Pos[] = { -2.0, 0.0, 0.0 };
ALfloat source0Vel[] = { 0.0, 0.0, 0.0 };

/*
 * Struct to hold fmt subchunk data for WAVE files.
 */
struct WAVE_Format {
    char subChunkID[4];
    int subChunkSize;
    short audioFormat;
    short numChannels;
    int sampleRate;
    int byteRate;
    short blockAlign;
    short bitsPerSample;
};

/*
 * Struct to hold the data of the wave file
 */
struct WAVE_Data {
    char subChunkID[4]; //should contain the word data
    int subChunk2Size; //Stores the size of the data block
};

/*
 * Struct that holds the RIFF data of the Wave file.
 * The RIFF data is the meta data information that holds,
 * the ID, size and format of the wave file
 */
struct RIFF_Header {
    char chunkID[4];
    int chunkSize;//size not including chunkSize or chunkID
    char format[4];
};

AudioEngine::AudioEngine() {}
AudioEngine::~AudioEngine() {}
	void AudioEngine::initialize() {
        
        device = alcOpenDevice(NULL);
        context = alcCreateContext(device, NULL);
        alcMakeContextCurrent(context);

        alListenerfv(AL_POSITION, listenerPos);
        alListenerfv(AL_VELOCITY, listenerVel);
        alListenerfv(AL_ORIENTATION, listenerOri);

    }
	void AudioEngine::initializeBuffers(bool engineOnly) {

        // Generate buffers for sound
        alGenSources(NUM_TOTAL, sources);
        CheckError();
    
        loadWavFile("audio_files/engine.wav", bufferArray);
        CheckError();

        alSourcef(sources[0], AL_PITCH, 1.0f);
        alSourcef(sources[0], AL_GAIN, 1.0f);
        alSourcefv(sources[0], AL_POSITION, source0Pos);
        alSourcefv(sources[0], AL_VELOCITY, source0Vel);
        alSourcei(sources[0], AL_BUFFER, bufferArray[0]);

        if (!engineOnly) {

            loadWavFile("audio_files/track.wav", bufferArray + 1);
            CheckError();

            alSourcef(sources[1], AL_PITCH, 1.0f);
            alSourcef(sources[1], AL_GAIN, 0.25f);
            alSourcefv(sources[1], AL_POSITION, source0Pos);
            alSourcefv(sources[1], AL_VELOCITY, source0Vel);
            alSourcei(sources[1], AL_BUFFER, bufferArray[1]);

            loadWavFile("audio_files/collision.wav", bufferArray + 2);
            CheckError();

            alSourcef(sources[2], AL_PITCH, 1.0f);
            alSourcef(sources[2], AL_GAIN, 1.0f);
            alSourcefv(sources[2], AL_POSITION, source0Pos);
            alSourcefv(sources[2], AL_VELOCITY, source0Vel);
            alSourcei(sources[2], AL_BUFFER, bufferArray[2]);

            loadWavFile("audio_files/projectile.wav", bufferArray + 3);
            CheckError();

            alSourcef(sources[3], AL_PITCH, 1.0f);
            alSourcef(sources[3], AL_GAIN, 3.5f);
            alSourcefv(sources[3], AL_POSITION, source0Pos);
            alSourcefv(sources[3], AL_VELOCITY, source0Vel);
            alSourcei(sources[3], AL_BUFFER, bufferArray[3]);

            loadWavFile("audio_files/flag_pickup.wav", bufferArray + 4);
            CheckError();

            alSourcef(sources[4], AL_PITCH, 1.0f);
            alSourcef(sources[4], AL_GAIN, 1.0f);
            alSourcefv(sources[4], AL_POSITION, source0Pos);
            alSourcefv(sources[4], AL_VELOCITY, source0Vel);
            alSourcei(sources[4], AL_BUFFER, bufferArray[4]);

            loadWavFile("audio_files/projectile_pickup.wav", bufferArray + 5);
            CheckError();

            alSourcef(sources[5], AL_PITCH, 1.0f);
            alSourcef(sources[5], AL_GAIN, 0.4f);
            alSourcefv(sources[5], AL_POSITION, source0Pos);
            alSourcefv(sources[5], AL_VELOCITY, source0Vel);
            alSourcei(sources[5], AL_BUFFER, bufferArray[5]);

            loadWavFile("audio_files/spike_trap_pickup.wav", bufferArray + 6);
            CheckError();

            alSourcef(sources[6], AL_PITCH, 1.0f);
            alSourcef(sources[6], AL_GAIN, 0.4f);
            alSourcefv(sources[6], AL_POSITION, source0Pos);
            alSourcefv(sources[6], AL_VELOCITY, source0Vel);
            alSourcei(sources[6], AL_BUFFER, bufferArray[6]);

            loadWavFile("audio_files/speed_boost_pickup.wav", bufferArray + 7);
            CheckError();

            alSourcef(sources[7], AL_PITCH, 1.0f);
            alSourcef(sources[7], AL_GAIN, 0.4f);
            alSourcefv(sources[7], AL_POSITION, source0Pos);
            alSourcefv(sources[7], AL_VELOCITY, source0Vel);
            alSourcei(sources[7], AL_BUFFER, bufferArray[7]);

            loadWavFile("audio_files/flag_return.wav", bufferArray + 8);
            CheckError();

            alSourcef(sources[8], AL_PITCH, 1.0f);
            alSourcef(sources[8], AL_GAIN, 1.0f);
            alSourcefv(sources[8], AL_POSITION, source0Pos);
            alSourcefv(sources[8], AL_VELOCITY, source0Vel);
            alSourcei(sources[8], AL_BUFFER, bufferArray[8]);

            loadWavFile("audio_files/speed_boost.wav", bufferArray + 9);
            CheckError();

            alSourcef(sources[9], AL_PITCH, 0.6f);
            alSourcef(sources[9], AL_GAIN, 0.8f);
            alSourcefv(sources[9], AL_POSITION, source0Pos);
            alSourcefv(sources[9], AL_VELOCITY, source0Vel);
            alSourcei(sources[9], AL_BUFFER, bufferArray[9]);

            loadWavFile("audio_files/car_crash.wav", bufferArray + 10);
            CheckError();

            alSourcef(sources[10], AL_PITCH, 1.0f);
            alSourcef(sources[10], AL_GAIN, 1.5f);
            alSourcefv(sources[10], AL_POSITION, source0Pos);
            alSourcefv(sources[10], AL_VELOCITY, source0Vel);
            alSourcei(sources[10], AL_BUFFER, bufferArray[10]);

            loadWavFile("audio_files/projectile_explosion.wav", bufferArray + 11);
            CheckError();

            alSourcef(sources[11], AL_PITCH, 1.0f);
            alSourcef(sources[11], AL_GAIN, 0.8f);
            alSourcefv(sources[11], AL_POSITION, source0Pos);
            alSourcefv(sources[11], AL_VELOCITY, source0Vel);
            alSourcei(sources[11], AL_BUFFER, bufferArray[11]);

            loadWavFile("audio_files/flag_lost.wav", bufferArray + 12);
            CheckError();

            alSourcef(sources[12], AL_PITCH, 1.0f);
            alSourcef(sources[12], AL_GAIN, 1.0f);
            alSourcefv(sources[12], AL_POSITION, source0Pos);
            alSourcefv(sources[12], AL_VELOCITY, source0Vel);
            alSourcei(sources[12], AL_BUFFER, bufferArray[12]);

            loadWavFile("audio_files/door_switch.wav", bufferArray + 13);
            CheckError();

            alSourcef(sources[13], AL_PITCH, 1.0f);
            alSourcef(sources[13], AL_GAIN, 3.0f);
            alSourcefv(sources[13], AL_POSITION, source0Pos);
            alSourcefv(sources[13], AL_VELOCITY, source0Vel);
            alSourcei(sources[13], AL_BUFFER, bufferArray[13]);

            loadWavFile("audio_files/engine.wav", bufferArray + 14);
            CheckError();

            alSourcef(sources[14], AL_PITCH, 1.0f);
            alSourcef(sources[14], AL_GAIN, 1.0f);
            alSourcefv(sources[14], AL_POSITION, source0Pos);
            alSourcefv(sources[14], AL_VELOCITY, source0Vel);
            alSourcei(sources[14], AL_BUFFER, bufferArray[14]);

            loadWavFile("audio_files/engine.wav", bufferArray + 15);
            CheckError();

            alSourcef(sources[15], AL_PITCH, 1.0f);
            alSourcef(sources[15], AL_GAIN, 1.0f);
            alSourcefv(sources[15], AL_POSITION, source0Pos);
            alSourcefv(sources[15], AL_VELOCITY, source0Vel);
            alSourcei(sources[15], AL_BUFFER, bufferArray[15]);

            loadWavFile("audio_files/engine.wav", bufferArray + 16);
            CheckError();

            alSourcef(sources[16], AL_PITCH, 1.0f);
            alSourcef(sources[16], AL_GAIN, 1.0f);
            alSourcefv(sources[16], AL_POSITION, source0Pos);
            alSourcefv(sources[16], AL_VELOCITY, source0Vel);
            alSourcei(sources[16], AL_BUFFER, bufferArray[16]);

            loadWavFile("audio_files/caught.wav", bufferArray + 17);
            CheckError();

            alSourcef(sources[17], AL_PITCH, 1.0f);
            alSourcef(sources[17], AL_GAIN, 3.0f);
            alSourcefv(sources[17], AL_POSITION, source0Pos);
            alSourcefv(sources[17], AL_VELOCITY, source0Vel);
            alSourcei(sources[17], AL_BUFFER, bufferArray[17]);

        }
        
    }
	AudioInstance& AudioEngine::createInstance(int soundFile) {

        ALuint* track = &sources[soundFile]; 

        AudioInstance mainTrack = AudioInstance(track);
        mainTrack.initialize();
        return mainTrack;
    }
	void AudioEngine::killSources() {
        
        alcMakeContextCurrent(NULL);
        alcDestroyContext(context);
        alcCloseDevice(device);
    }

	void AudioEngine::CheckError() {
        
        // passed as arguments in boilerplate but never actually changed, so setting them should be fine

        int err;
        err = alGetError(); // clear any error messages

        if (err != AL_NO_ERROR) {
            if (err == AL_INVALID_NAME)
                fprintf(stderr, "Error : Invalid Name\n");
            else if (err == AL_INVALID_ENUM)
                fprintf(stderr, "Error : Invalid Enum\n");
            else if (err == AL_INVALID_VALUE)
                fprintf(stderr, "Error : Invalid Value\n");
            else if (err == AL_INVALID_OPERATION)
                fprintf(stderr, "Error : Invalid Operation\n");
            else if (err == AL_OUT_OF_MEMORY)
                fprintf(stderr, "Error : Out of Memory\n");
            exit(1);
        }
        return;
    }
	bool AudioEngine::_strcmp(const char* base, const char* cp) {
        for (int lx = 0; base[lx] != 0;lx++) {
            if (cp[lx] != base[lx])
                return false;
        }
        return true;
    }

	// Source: https://gist.github.com/mudream4869/34541dfbd12a747b027e
	bool AudioEngine::loadWavFile(const char* filename, ALuint* buffer) {

        //Local Declarations
        fprintf(stderr, "[1] filename = %s\n", filename);
        FILE* soundFile = NULL;
        WAVE_Format wave_format;
        RIFF_Header riff_header;
        WAVE_Data wave_data;
        unsigned char* data = 0;

        try {
            soundFile = fopen(filename, "rb");
            if (!soundFile)
                throw (filename);

            // Read in the first chunk into the struct
            fread(&riff_header, sizeof(RIFF_Header), 1, soundFile);

            //check for RIFF and WAVE tag in memeory
            if (_strcmp("RIFF", riff_header.chunkID) == false or
                _strcmp("WAVE", riff_header.format) == false)
                throw ("Invalid RIFF or WAVE Header");

            //Read in the 2nd chunk for the wave info
            fread(&wave_format, sizeof(WAVE_Format), 1, soundFile);
            //check for fmt tag in memory
            if (_strcmp("fmt ", wave_format.subChunkID) == false)
                throw ("Invalid Wave Format");

            //check for extra parameters;
            if (wave_format.subChunkSize > 16)
                fseek(soundFile, sizeof(short), SEEK_CUR);

            //Read in the the last byte of data before the sound file
            fread(&wave_data, sizeof(WAVE_Data), 1, soundFile);
            //check for data tag in memory
            if (_strcmp("data", wave_data.subChunkID) == false)
                throw ("Invalid data header");

            //Allocate memory for data
            data = new unsigned char[wave_data.subChunk2Size];

            // Read in the sound data into the soundData variable
            if (!fread(data, wave_data.subChunk2Size, 1, soundFile))
                throw ("error loading WAVE data into struct!");

            //Now we set the variables that we passed in with the
            //data from the structs
            ALsizei size = wave_data.subChunk2Size;
            ALsizei frequency = wave_format.sampleRate;
            ALenum format;

            //The format is worked out by looking at the number of
            //channels and the bits per sample.
            if (wave_format.numChannels == 1) {
                if (wave_format.bitsPerSample == 8)
                    format = AL_FORMAT_MONO8;
                else if (wave_format.bitsPerSample == 16)
                    format = AL_FORMAT_MONO16;
            }
            else if (wave_format.numChannels == 2) {
                if (wave_format.bitsPerSample == 8)
                    format = AL_FORMAT_STEREO8;
                else if (wave_format.bitsPerSample == 16)
                    format = AL_FORMAT_STEREO16;
            }
            //create our openAL buffer and check for success
            CheckError();
            alGenBuffers(1, buffer);
            CheckError();
            //now we put our data into the openAL buffer and
            //check for success
            alBufferData(*buffer, format, (void*)data, size, frequency);
            CheckError();
            //clean up and return true if successful
            fclose(soundFile);
            fprintf(stderr, "load ok\n");
            delete[] data;
            return true;
        }
        catch (std::string error) {
            //our catch statement for if we throw a string
            std::cout << error << std::endl;
            //clean up memory if wave loading fails
            if (soundFile != NULL)
                fclose(soundFile);
            if (data)
                delete[] data;
            //return false to indicate the failure to load wave
            return false;
        }
	}

