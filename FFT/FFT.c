#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <float.h>
#include <complex.h>
#include <math.h>
#include <sndfile.h>
#include <portaudio.h>
#include <raylib.h>
#include <omp.h>
#include <errno.h>
#include <string.h>

// TODO:
// Fix bucket scaling
// Figure out why there's gaps in the buckets
// Speed up the calculation?
// Fix audio crashing on certain bucket amounts

// 1024 is consistently the amount of frames available to write
#define FRAMECOUNT 1024
#define HEIGHT 480
#define WIDTH 1024
// 256 is the highest multiple of 2 I can go to without crashing audio
// Maybe I should define this by the step rather than the number of buckets?
#define BUCKETS 256

int callback(const void *input, void *output, unsigned long frameCount,
             const PaStreamCallbackTimeInfo *timeInfo,
             PaStreamCallbackFlags statusFlags, void *userData);

// Because apparently you gotta do everything yourself around here
float max(float *array, int length) {
    float max = -1.0;
    for (int i = 0; i < length; i++) {
        if (array[i] > max)
            max = array[i];
    }
    return max;
}
float min(float *array, int length) {
    float min = FLT_MAX;
    for (int i = 0; i < length; i++) {
        if (array[i] < min)
            min = array[i];
    }
    return min;
}

// FFT algorithm from the Cooley-Tukey Wikipedia page following the pseudocode
// for the radix-2 case
// https://en.wikipedia.org/wiki/Cooley%E2%80%93Tukey_FFT_algorithm
void fft(float *input, float complex *output, int length) {
    if (length == 1) {
        output[0] = input[0];

    } else {
        float *rearranged = malloc(length * sizeof(float));
        // Rearrange the input function for recursion
        for (int i = 0; i < length / 2; i++) {
            rearranged[i] = input[2 * i];
            rearranged[length / 2 + i] = input[2 * i + 1];
        }
        fft(input, output, length / 2);
        fft(&input[length / 2], &output[length / 2], length / 2);

        for (int k = 0; k < length / 2; k++) {
            float complex even = output[k];
            float complex odd =
                cexpf(-2 * PI * I / length * k) * output[k + length / 2];
            output[k] = even + odd;
            output[k + length / 2] = even - odd;
        }
        free(rearranged);
    }
}

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

    if (child) {
        // Parent process displays the visualization
        close(pipes[1]);
        int index = 0;

        float complex **fft_output = malloc(2 * sizeof(float complex *));
        fft_output[0] = calloc(FRAMECOUNT, sizeof(float complex));
        fft_output[1] = calloc(FRAMECOUNT, sizeof(float complex));

        float **channels = malloc(2 * sizeof(float *));
        channels[0] = calloc(FRAMECOUNT, sizeof(float));
        channels[1] = calloc(FRAMECOUNT, sizeof(float));

        available = FRAMECOUNT;
        InitWindow(WIDTH, HEIGHT, "FFT Visualization");
        SetTargetFPS(60);
        while (!WindowShouldClose()) {
            BeginDrawing();
            ClearBackground(BLACK);
            // Should act as a sort of sync between the parent and child
            // assuming no errors
            ssize_t amount = 0;
            // do {
                amount = read(pipes[0], frames,
                              available * info.channels * sizeof(float));
            // } while (amount == 0 && errno == EAGAIN);

            if (amount) {
                for (int i = 0; i < FRAMECOUNT; i++) {
                    /*printf("%f %f\n", frames[2 * i], frames[2 * i + 1]);*/
                    channels[0][i] = frames[2 * i];
                    channels[1][i] = frames[2 * i + 1];
                }
                fft(channels[0], fft_output[0], FRAMECOUNT);
                fft(channels[1], fft_output[1], FRAMECOUNT);
                float height1[BUCKETS];
                float height2[BUCKETS];
                // How many to average before drawing a rectangle
                int step = WIDTH / (BUCKETS * 2);
                if (!step)
                    step = 1;
                int chunk = 200;
#pragma omp parallel for shared(height1, height2, chunk)                       \
    schedule(dynamic, chunk)
                for (int i = 0; i < BUCKETS; i++) {
                    height1[i] = cabsf(fft_output[0][FRAMECOUNT / 2 + step * i]);
                    height2[i] = cabsf(fft_output[1][FRAMECOUNT / 2 + step * i]);
                }
                // printf("Thread %d calculating\n", omp_get_thread_num());

                float boxH1, boxH2, max1, max2, min1, min2;
#pragma omp parallel
                {
                    max1 = max(height1, BUCKETS);
                    min1 = min(height1, BUCKETS);
                    max2 = max(height2, BUCKETS);
                    min2 = min(height2, BUCKETS);
                }

                for (int i = 0; i < BUCKETS; i++) {
                    boxH1 = (height1[i] - min1) / (max1 - min1) * HEIGHT / 2.0;
                    boxH2 = (height2[i] - min2) / (max2 - min2) * HEIGHT / 2.0;
                    DrawRectangle(step * i, HEIGHT / 2.0 - boxH1, step, boxH1,
                                  GREEN);
                    DrawRectangle(step * i, HEIGHT - boxH2, step, boxH2, BLUE);
                }
            }
            /*printf("Done\n");*/
            EndDrawing();
        }
        free(frames);
        free(fft_output[0]);
        free(fft_output[1]);
        free(fft_output);
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
        printf("Channels: %d\n", info.channels);
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

            // printf("Checking :%ld\n", available);
            if (available) {
                // printf("Running\n");
                count = sf_readf_float(music, frames, FRAMECOUNT);
                // printf("Count: %ld", count);
                Pa_WriteStream(stream, frames, FRAMECOUNT);
                write(pipes[1], frames,
                      FRAMECOUNT * info.channels * sizeof(float));
                if (count < FRAMECOUNT)
                    break;
            }
        }
        printf("Child Exiting\n");
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
