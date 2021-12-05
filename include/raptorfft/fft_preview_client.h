#pragma once

#include <raptorfft/fft_preview.h>

class raptor_fft_preview_client : public raptor_fft_preview_client_base {

public:
	raptor_fft_preview_client(int outputSize);
	~raptor_fft_preview_client();

	float get_attack() { return attack; }
	void set_attack(float value) { if (value > 1) { value = 1; } if (value < 0) { value = 0; } attack = value; }
	float get_decay() { return decay; }
	void set_decay(float value) { if (value > 1) { value = 1; } if (value < 0) { value = 0; } decay = value; }
	int get_output_size() { return output_size; }
	void set_output_size(int size) { output_size = size; configure(); }

	virtual void preview_push(float* power, int count) override;

protected:
	virtual void preview_output(float* power, int count) = 0;

private:
	int output_size;
	float attack;
	float decay;

	bool first;

	float* power_buffer;
	float* output_buffer;

	void configure();

};