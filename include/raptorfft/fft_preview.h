#pragma once

#include <raptordsp/defines.h>
#include <fftw3.h>

class raptor_fft_preview_client_base {

public:
	virtual void preview_push(float* power, int count) = 0;

};

class raptor_fft_preview {

public:
	raptor_fft_preview(int fft_size, raptor_fft_preview_client_base* output);
	~raptor_fft_preview();

	void set_fps(float fps) { this->fps = fps; configure(); }
	void set_sample_rate(float sample_rate) { this->sample_rate = sample_rate; configure(); }

	void input(raptor_complex* input, int count);

private:
	void push_block();

private:
	raptor_fft_preview_client_base* output;
	int fft_size;
	int block_size;
	float fps;
	float sample_rate;

	float* power;
	float* window;
	raptor_complex* fft_in;
	raptor_complex* fft_out;
	int buffer_usage;

	fftwf_plan plan;

	void configure();

};