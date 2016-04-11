#include <stdio.h>
#include <stdlib.h>
#include <fftw3.h>
#include <math.h>
#include "portaudio.h"

#define SAMPLE_RATE 44100
#define PA_SAMPLE_TYPE paFloat32
#define FRAMES_PER_BUFFER 64

#define C 32.7f
#define D 36.7f
#define E 41.2f
#define F 43.7f
#define G 49.0f
#define A 55.0f
#define B 61.7f

void checkError(PaError error);
static int callback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,PaStreamCallbackFlags statusFlags, void *userData);

const static float notes[] = {C, D, E, F, G, A, B};

int main(int argc, char const *argv[])
{
	PaStreamParameters inputParameters;
	PaStreamParameters outputParameters;
	PaStream *stream;
	PaError error;

	error = Pa_Initialize();
	checkError(error);

	inputParameters.device = Pa_GetDefaultInputDevice();
	if (inputParameters.device == paNoDevice) {
		printf("PortAudio Error: No default input device.\n");
		exit(-1);
	}

	inputParameters.channelCount = 1;
	inputParameters.sampleFormat = PA_SAMPLE_TYPE;
	inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
	inputParameters.hostApiSpecificStreamInfo = NULL;

	outputParameters.device = Pa_GetDefaultOutputDevice();
	if (outputParameters.device == paNoDevice) {
		printf("PortAudio Error: No default output device.\n");
		exit(-1);
	}

	outputParameters.channelCount = 1;
	outputParameters.sampleFormat = PA_SAMPLE_TYPE;
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;

	error = Pa_OpenStream(&stream,
		&inputParameters,
		&outputParameters,
		SAMPLE_RATE,
		FRAMES_PER_BUFFER,
		0,
		callback,
		NULL);
	checkError(error);

	error = Pa_StartStream(stream);
	checkError(error);

	printf("Press enter to stop...\n");
	getchar();

	error = Pa_CloseStream(stream);
	checkError(error);

	Pa_Terminate();

	return 0;
}

void checkError(PaError error)
{
	if (error != paNoError) {
		printf("PortAudio Error: %s\n", Pa_GetErrorText(error));
		Pa_Terminate();
		exit(-1);
	}
}

float cubicAmplifier(float input)
{
	float output;
	float temp;
	if (input < 0.0f) {
		temp = input + 1.0f;
		output = (temp * temp * temp) - 1.0f;
	} else {
		temp = input - 1.0f;
		output = (temp * temp * temp) + 1.0f;
	}

	return output;
}

void lowPassFilter(float *input, int length, float *output, int passes)
{
	for (int i = 0; i < length; ++i) {
		output[i] = input[i];
	}

	for (int i = 0; i < passes; ++i) {
		for (int i = 0; i < length; ++i) {
			int nextIndex = i + 1 > length - 1 ? 0 : i + 1;
			output[i] = (output[i] + output[nextIndex]) / 2.0;
		}
	}
}

static int callback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,PaStreamCallbackFlags statusFlags, void *userData)
{
	float *out = (float*)outputBuffer;
	const float *in = (const float*)inputBuffer;

	(void)timeInfo;
	(void)statusFlags;
	(void)userData;

	if (inputBuffer == NULL) {
		for (int i = 0; i < framesPerBuffer; ++i) {
			*out++ = 0.0f;
		}
	} else {
		float *filteredData = malloc(sizeof(float) * framesPerBuffer);
		lowPassFilter(in, framesPerBuffer, filteredData, 1);
		for (int i = 0; i < framesPerBuffer; ++i)
		{
			
			printf("%f\n", filteredData[i]);
		}
		for (int i = 0; i < framesPerBuffer; ++i) {
			// float tp = *in++;
			*out++ = filteredData[i]; // tp;
		}

		free(filteredData);

		// fftw_complex *fftIn = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * framesPerBuffer);
		// fftw_complex *fftOut = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * framesPerBuffer);
		// fftw_plan p = fftw_plan_dft_1d(framesPerBuffer, fftIn, fftOut, FFTW_FORWARD, FFTW_ESTIMATE);

		// for (int i = 0; i < framesPerBuffer; ++i) {
		// 	float in1 = *in++;
		// 	fftIn[i][0] = (double)in1;
		// 	fftIn[i][1] = 0.0;
		// }

		// fftw_execute(p);

		// double highestFrequency = 0.0;
		// for (int i = 0; i < framesPerBuffer; ++i) {
		// 	double com[] = {fftOut[i][0], fftOut[i][1]};
		// 	double freq = indexToFreq(i, SAMPLE_RATE, framesPerBuffer);
		// 	double fin = com[0] * freq;
		// 	if (fin > highestFrequency) {
		// 		highestFrequency = fin;
		// 	}
		// }

		// for (int i = 0; i < framesPerBuffer; ++i) {
		// 	double com[] = {fftOut[i][0], fftOut[i][1]};
		// 	*out++ = com[0];
		// }

		// fftw_destroy_plan(p);
		// fftw_free(fftIn);
		// fftw_free(fftOut);
	}

	return paContinue;
}
