#include <stdio.h>
#include <signal.h>
#include <unistd.h>
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

SNDFILE *music;
PaStream *stream;
float *frames;
int pipes[2];

// For the child only
static void term_catch(int signo) {
    Pa_StopStream(stream);
    sf_close(music);
    Pa_Terminate();
    free(frames);
    close(pipes[1]);
    exit(EXIT_SUCCESS);
}

int ended = 0;
char *file = "libera.mp3";

int main(int argc, char *argv[]) {
    PaErrorCode err;

    SF_INFO info;
    info.format = 0;
    // signal(SIGKILL, term_catch);

    // Take command-line input to select a file for now
    if (argc > 1)
        file = argv[1];

    // Open audio file for reading

    pipe(pipes);

    pid_t child = fork();

    frames = calloc(FRAMECOUNT * info.channels, sizeof(float));
    if (!frames) {
        printf("Issue allocating frame array\n");
        return -1;
    }
    signed long available;
    sf_count_t count = -1;

    if (child) {
        // Parent process displays the visualization
        close(pipes[1]);
        int index = 0;

        // Why does this work but float complex *fft[2] doesn't???
        float complex **fft = malloc(2 * sizeof(float complex *));
        fft[0] = calloc(FRAMECOUNT, sizeof(float complex));
        fft[1] = calloc(FRAMECOUNT, sizeof(float complex));

        float **channels = malloc(2 * sizeof(float *));
        channels[0] = calloc(FRAMECOUNT, sizeof(float));
        channels[1] = calloc(FRAMECOUNT, sizeof(float));

        available = 1024;
        InitWindow(640, 480, "FFT Visualization");
        SetTargetFPS(60);
        while (!WindowShouldClose()) {
            BeginDrawing();
            ClearBackground(BLACK);
            // Should act as a sort of sync between the parent and child
            // assuming no errors
            ssize_t amount = read(pipes[0], frames,
                                  available * info.channels * sizeof(float));
            /*printf("%d\n", (int)amount);*/
            for (int i = 0; i < FRAMECOUNT; i++) {
                /*printf("%f %f\n", frames[2 * i], frames[2 * i + 1]);*/
                channels[0][i] = frames[2 * i];
                channels[1][i] = frames[2 * i + 1];
            }
            int height = index / (float)64 * 480;
            DrawRectangle(10 * index, 480 - height, 10, height, BLUE);
            index = (index + 1) % 64;
            EndDrawing();
        }
        free(frames);
        free(fft[0]);
        free(fft[1]);
        free(fft);
        free(channels[0]);
        free(channels[1]);
        free(channels);
        if (!kill(child, 0))
            // Makes sure the music stops with the window if it hasn't already
            kill(child, SIGTERM);
        CloseWindow();
        close(pipes[0]);
    } else {
        // Child process plays audio and passes the data to the parent
        err = Pa_Initialize();
        if (err != paNoError) {
            printf("Issue starting PortAudio.\n");
            return 1;
        }
        signal(SIGTERM, term_catch);
        music = sf_open(file, SFM_READ, &info);
        close(pipes[0]);
        err = Pa_OpenDefaultStream(&stream, 0, info.channels, paFloat32,
                                   info.samplerate, FRAMECOUNT, NULL, music);
        if (err != paNoError) {
            printf("Error opening stream\n");
            return 1;
        }

        err = Pa_StartStream(stream);
        sf_count_t count = -1;

        while (true) {
            available = Pa_GetStreamWriteAvailable(stream);
            if (available) {
                // printf("Running\n");
                count = sf_readf_float(music, frames, available);
                Pa_WriteStream(stream, frames, available);
                write(pipes[1], frames,
                      available * info.channels * sizeof(float));
                if (count < available)
                    break;
            }
        }
        err = Pa_StopStream(stream);
        sf_close(music);
        Pa_Terminate();
        free(frames);
        close(pipes[1]);
    }
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
