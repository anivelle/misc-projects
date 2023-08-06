#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>
#include <sndfile.h>
#include <portaudio.h>
#include <raylib.h>

#define LENGTH 200
#define SEQ_STEP 0.1
int callback(const void *input, void *output, unsigned long frameCount,
             const PaStreamCallbackTimeInfo *timeInfo,
             PaStreamCallbackFlags statusFlags, void *userData);

int ended = 0;

int main(int argc, char *argv[]) {
    PaErrorCode err = Pa_Initialize();
    if (err != paNoError) {
        printf("Issue starting PortAudio.\n");
        return 1;
    }
    const PaHostApiInfo *host = Pa_GetHostApiInfo(Pa_GetDefaultHostApi());
    const PaDeviceInfo *indev = Pa_GetDeviceInfo(host->defaultInputDevice);
    const PaDeviceInfo *outdev = Pa_GetDeviceInfo(host->defaultOutputDevice);

    SF_INFO info;
    info.format = 0;

    SNDFILE *music = sf_open("libera.mp3", SFM_READ, &info);
    PaStream *stream;

    err = Pa_OpenDefaultStream(&stream, 0, info.channels, paFloat32,
                               info.samplerate, 512, &callback, music);

    if (err != paNoError) {
        printf("Error opening stream\n");
        return 1;
    }

    err = Pa_StartStream(stream);

    while (!ended) {
    };
    err = Pa_StopStream(stream);
    float sequence[LENGTH];
    float complex output[LENGTH];
    for (int i = 0; i < LENGTH; i++) {
        sequence[i] = cosf(i * SEQ_STEP);
        for (float j = -2; j < sequence[i] * 2; j += 0.05) {
            printf(" ");
        }
        printf("*\n");
    }
    sf_close(music);
    Pa_Terminate();
    return 0;
}

int callback(const void *input, void *output, unsigned long frameCount,
             const PaStreamCallbackTimeInfo *timeInfo,
             PaStreamCallbackFlags statusFlags, void *userData) {
    SNDFILE *music = (SNDFILE *)userData;
    float *out = (float *)output;
    (void)input;

    // https://github.com/hosackm/wavplayer/blob/master/src/wavplay.c
    // Realized I had to multiply the frameCount by the number of channels in
    // the audio from this page. Also helped with stream starting
    sf_count_t count = sf_read_float(music, out, frameCount * 2);
    if (count == frameCount * 2)
        return paContinue;

    ended = 1;
    return paComplete;
}
