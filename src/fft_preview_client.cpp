#include <raptorfft/fft_preview_client.h>
#include <malloc.h>
#include <cassert>
#include <algorithm>

#define RECREATE_BUFFER(buffer, type, length) if (buffer != nullptr) { free(buffer); } buffer = (type*)malloc(length * sizeof(type)); assert(buffer != nullptr);

raptor_fft_preview_client::raptor_fft_preview_client(int outputSize) :
	attack(0.2f),
	decay(0.3f),
	output_size(outputSize),
	first(true),
	power_buffer(nullptr),
	output_buffer(nullptr)
{
	configure();
}

raptor_fft_preview_client::~raptor_fft_preview_client() {
	//Free buffers
	if (power_buffer != nullptr) {
		free(power_buffer);
		power_buffer = nullptr;
	}
	if (output_buffer != nullptr) {
		free(output_buffer);
		output_buffer = nullptr;
	}
}

void raptor_fft_preview_client::configure() {
	//Create buffers
	RECREATE_BUFFER(power_buffer, float, output_size);
	RECREATE_BUFFER(output_buffer, float, output_size);

	//Set flag
	first = true;
}

void resize_fft(float* input, float* output, int inputLen, int outputLen) {
	int inputIndex = 0;
	int outputIndex = 0;
	float max = 0;
	bool resetMax = true;
	while (inputIndex < inputLen)
	{
		//Read value
		if (resetMax)
			max = input[inputIndex++];
		else
			max = std::max(input[inputIndex++], max);

		//Write to output
		int targetOutput = (inputIndex * outputLen) / inputLen;
		resetMax = (outputIndex < targetOutput);
		while (outputIndex < targetOutput)
			output[outputIndex++] = max;
	}
}

void apply_smoothening(float* buffer, float* incoming, int count, float attack, float decay)
{
	float ratio;
	for (int i = 0; i < count; i++)
	{
		ratio = buffer[i] < incoming[i] ? attack : decay;
		buffer[i] = buffer[i] * (1 - ratio) + incoming[i] * ratio;
	}
}

void raptor_fft_preview_client::preview_push(float* power, int count) {
	//Resize into our temporary buffer
	resize_fft(power, power_buffer, count, output_size);

	//Smoothen, or just copy if this is the first time
	if (first)
		memcpy(output_buffer, power_buffer, sizeof(float) * output_size);
	else
		apply_smoothening(output_buffer, power_buffer, output_size, attack, decay);
	first = false;

	//Push output
	preview_output(output_buffer, output_size);
}