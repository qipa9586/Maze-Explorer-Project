#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#define SAMPLE_RATE 48000

void writeWav(const char *filename, float *samples, int count) {
    FILE *f = fopen(filename, "wb");
    int dataSize = count * 2;  // 16-bit = 2 bytes per sample

    // WAV header
    fwrite("RIFF", 1, 4, f);
    int32_t riffSize = 36 + dataSize;
    fwrite(&riffSize, 4, 1, f);
    fwrite("WAVE", 1, 4, f);

    fwrite("fmt ", 1, 4, f);
    int32_t fmtSize = 16;
    fwrite(&fmtSize, 4, 1, f);
    int16_t audioFormat = 1;       // PCM
    int16_t channels = 1;          // mono
    int32_t sampleRate = SAMPLE_RATE;
    int32_t byteRate = SAMPLE_RATE * 2;
    int16_t blockAlign = 2;
    int16_t bitsPerSample = 16;
    fwrite(&audioFormat, 2, 1, f);
    fwrite(&channels, 2, 1, f);
    fwrite(&sampleRate, 4, 1, f);
    fwrite(&byteRate, 4, 1, f);
    fwrite(&blockAlign, 2, 1, f);
    fwrite(&bitsPerSample, 2, 1, f);

    fwrite("data", 1, 4, f);
    fwrite(&dataSize, 4, 1, f);

    for (int i = 0; i < count; i++) {
        float val = samples[i];
        if (val > 1.0f) val = 1.0f;
        if (val < -1.0f) val = -1.0f;
        int16_t pcm = (int16_t)(val * 32767);
        fwrite(&pcm, 2, 1, f);
    }
    fclose(f);
}

int main() {
    int len;

    // walk.wav — 短触地声 (100ms ~ 2205 samples)
    {
        len = (int)(0.1 * SAMPLE_RATE);
        float *buf = (float *)malloc(len * sizeof(float));
        for (int i = 0; i < len; i++) {
            float t = (float)i / SAMPLE_RATE;
            buf[i] = 0.6f * sinf(6.2831f * 800 * t) * (1.0f - (float)i / len);
        }
        writeWav("assets/walk.wav", buf, len);
        free(buf);
        printf("walk.wav done\n");
    }

    // wall.wav — 闷撞声 (120ms, 低音衰减)
    {
        len = (int)(0.12 * SAMPLE_RATE);
        float *buf = (float *)malloc(len * sizeof(float));
        for (int i = 0; i < len; i++) {
            float t = (float)i / SAMPLE_RATE;
            buf[i] = 0.5f * sinf(6.2831f * 150 * t) * (1.0f - (float)i / len);
        }
        writeWav("assets/wall.wav", buf, len);
        free(buf);
        printf("wall.wav done\n");
    }

    // collect.wav — 叮 (150ms, 1200Hz 纯音 + 衰减)
    {
        len = (int)(0.15 * SAMPLE_RATE);
        float *buf = (float *)malloc(len * sizeof(float));
        for (int i = 0; i < len; i++) {
            float t = (float)i / SAMPLE_RATE;
            float env = (1.0f - (float)i / len);
            buf[i] = 0.6f * sinf(6.2831f * 1200 * t) * env * env;
        }
        writeWav("assets/collect.wav", buf, len);
        free(buf);
        printf("collect.wav done\n");
    }

    // all_collected.wav — 三连音上行 (400ms, C-E-G 三音符)
    {
        len = (int)(0.4 * SAMPLE_RATE);
        float *buf = (float *)malloc(len * sizeof(float));
        for (int i = 0; i < len; i++) buf[i] = 0;
        float notes[] = {523.25f, 659.25f, 783.99f};  // C5, E5, G5
        for (int n = 0; n < 3; n++) {
            int start = n * len / 3;
            int noteLen = len / 3;
            for (int i = 0; i < noteLen; i++) {
                int idx = start + i;
                if (idx >= len) break;
                float t = (float)i / SAMPLE_RATE;
                float env = (float)(noteLen - i) / noteLen;
                buf[idx] += 0.4f * sinf(6.2831f * notes[n] * t) * env * env;
            }
        }
        writeWav("assets/all_collected.wav", buf, len);
        free(buf);
        printf("all_collected.wav done\n");
    }

    // win.wav — 胜利号角 (2.5s, 上行大三和弦 + 长高潮 + 漫长尾音)
    {
        float totalSec = 2.5f;
        len = (int)(totalSec * SAMPLE_RATE);
        float *buf = (float *)malloc(len * sizeof(float));
        for (int i = 0; i < len; i++) buf[i] = 0;

        float notes[] = {523.25f, 659.25f, 783.99f, 1046.5f};  // C5, E5, G5, C6
        float dur[]   = {0.3f,    0.25f,   0.25f,   1.0f};
        int pos = 0;
        for (int n = 0; n < 4; n++) {
            int noteLen = (int)(dur[n] * SAMPLE_RATE);
            for (int i = 0; i < noteLen && pos + i < len; i++) {
                float t = (float)i / SAMPLE_RATE;
                float env = expf(-t * 2.0f);
                buf[pos + i] += 0.5f * sinf(6.2831f * notes[n] * t) * env;
                buf[pos + i] += 0.25f * sinf(6.2831f * notes[n] * 0.5f * t) * env;
            }
            pos += noteLen;
        }

        // 尾音回荡
        float tailStart = (float)pos / SAMPLE_RATE;
        float tailSec = totalSec - tailStart;
        for (int i = 0; i < (int)(tailSec * SAMPLE_RATE) && pos + i < len; i++) {
            float t = (float)i / SAMPLE_RATE;
            float env = expf(-t * 2.5f);
            buf[pos + i] += 0.35f * sinf(6.2831f * 1046.5f * t) * env;
        }

        writeWav("assets/win.wav", buf, len);
        free(buf);
        printf("win.wav done\n");
    }

    printf("All sounds generated.\n");
    return 0;
}
