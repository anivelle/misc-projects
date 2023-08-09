#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>
#include <sndfile.h>
#include <portaudio.h>
#include <raylib.h>

#define LENGTH 200

// 1024 is consistently the amount of frames available to write
#define FRAMECOUNT 1024
#define SEQ_STEP 0.1
int callback(const void *input, void *output, unsigned long frameCount,
             const PaStreamCallbackTimeInfo *timeInfo,
             PaStreamCallbackFlags statusFlags, void *userData);

int ended = 0;
char *file = "libera.mp3";

int main(int argc, char *argv[]) {
    PaErrorCode err = Pa_Initialize();
    if (err != paNoError) {
        printf("Issue starting PortAudio.\n");
        return 1;
    }

    SF_INFO info;
    info.format = 0;

    // Take command-line input to select a file for now
    if (argc > 1)
        file = argv[1];

    // Open audio file for reading
    SNDFILE *music = sf_open(file, SFM_READ, &info);
    PaStream *stream;

    // Synchronous playback - will use this so I have easier access to the data
    // that is being read.
    err = Pa_OpenDefaultStream(&stream, 0, info.channels, paFloat32,
                               info.samplerate, 512, NULL, music);

    // Asynchronous callback - cool but I'm not sure how the timing will work
    // when trying to capture a specific frame of data to read
    /* err = Pa_OpenDefaultStream(&stream, 0, info.channels, paFloat32, */
    /*                            info.samplerate, 512, &callback, music); */

    if (err != paNoError) {
        printf("Error opening stream\n");
        return 1;
    }

    err = Pa_StartStream(stream);
    sf_count_t count = -1;
    signed long available;

    float *frames = malloc(FRAMECOUNT * info.channels);

    while (true) {
        available = Pa_GetStreamWriteAvailable(stream);
        if (available) {
            /* printf("%ld\n", available); */
            count = sf_readf_float(music, frames, available);
            /* printf("%ld\n", count); */
            Pa_WriteStream(stream, frames, available);
            if (count < available)
                break;
        }
    };
    // err = Pa_StopStream(stream);

    // Just for funsies

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

// No longer in use
/* int callback(const void *input, void *output, unsigned long frameCount, */
/*              const PaStreamCallbackTimeInfo *timeInfo, */
/*              PaStreamCallbackFlags statusFlags, void *userData) { */
/*     SNDFILE *music = (SNDFILE *)userData; // Take in the audio file */
/*     float *out = (float *)output;         // Cast output buffer */
/*     (void)input; */

/*     // https://github.com/hosackm/wavplayer/blob/master/src/wavplay.c */
/*     // Realized I had to multiply the frameCount by the number of channels in
 */
/*     // the audio from this page. Also helped with stream starting */
/*     // */
/*     // Reads audio from the input file straight into the output buffer */
/*     sf_count_t count = sf_read_float(music, out, frameCount * 2); */

/*     // Same way file reading works. If we read less than expected, we've
 * reached */
/*     // the end */
/*     if (count == frameCount * 2) */
/*         return paContinue; */

/*     ended = 1; // Mark the ending of the song */
/*     return paComplete; */
/* } */
