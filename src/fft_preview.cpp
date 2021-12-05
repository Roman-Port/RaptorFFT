#include <raptorfft/fft_preview.h>
#include <cstring>
#include <algorithm>
#include <cassert>
#include <malloc.h>
#include <volk/volk.h>
#include <raptordsp/filter/window.h>

raptor_fft_preview::raptor_fft_preview(int fft_size, raptor_fft_preview_client_base* output) :
    fft_size(fft_size),
    output(output),
    fps(0),
    sample_rate(0),
    block_size(0),
    buffer_usage(0),
    fft_in((raptor_complex*)malloc(fft_size * sizeof(raptor_complex))),
    fft_out((raptor_complex*)malloc(fft_size * sizeof(raptor_complex))),
    power((float*)malloc(fft_size * sizeof(float))),
    window(nullptr)
{
    //Validate
    assert(fft_in != nullptr);
    assert(fft_out != nullptr);
    assert(power != nullptr);

    //Clear
    memset(fft_in, 0, fft_size * sizeof(raptor_complex));

    //Create plan
    plan = fftwf_plan_dft_1d(fft_size, (fftwf_complex*)fft_in, (fftwf_complex*)fft_out, FFTW_FORWARD, FFTW_ESTIMATE);

    //Create the FFT window
    window = raptor_window_build(win_type::WIN_BLACKMAN_HARRIS, fft_size, 0, true);
}

raptor_fft_preview::~raptor_fft_preview() {
    //Destroy buffers
    free(fft_in);
    free(fft_out);
    free(window);
    free(power);

    //Destroy plan
    fftwf_destroy_plan(plan);
}

void raptor_fft_preview::input(raptor_complex* input, int count) {
    //If the block size is 0, this is invalid; refuse
    if (block_size == 0)
        return;

    //Enter loop
    while (count > 0) {
        //If our buffer position is less than 0, consume but don't write until we reach 0
        if (buffer_usage < 0) {
            //Consume
            int add = std::min(count, -buffer_usage);
            buffer_usage += add;
            input += add;
            count -= add;

            //If our buffer usage is still less than zero, do nothing
            if (buffer_usage < 0)
                return;
        }

        //Get the number we'll consume to not read too much AND read up to a block boundry
        int consumed = std::min(fft_size - buffer_usage, count);

        //Write to the buffer and consume
        memcpy(&fft_in[buffer_usage], input, consumed * sizeof(raptor_complex));
        buffer_usage += consumed;
        input += consumed;
        count -= consumed;

        //Check if it's time to submit a block
        if (buffer_usage == fft_size) {
            //Push
            push_block();

            //Update; this may or may not make it negative
            buffer_usage -= block_size;

            //Move everything in the buffer block_size samples down, keeping buffer_usage
            if (buffer_usage > 0) {
                memcpy(fft_in, &fft_in[block_size], sizeof(raptor_complex) * buffer_usage);
            }
        }
    }
}

void raptor_fft_preview::configure() {
    //Set
    if (sample_rate > 0 && fps > 0)
        block_size = (int)(sample_rate / fps);
    else
        block_size = 0;

    //Reset
    buffer_usage = fft_size - block_size;
}

void offset_spectrum(raptor_complex* buffer, int count) {
    count /= 2;
    raptor_complex* left = buffer;
    raptor_complex* right = buffer + count;
    raptor_complex temp;
    for (int i = 0; i < count; i++)
    {
        temp = *left;
        *left++ = *right;
        *right++ = temp;
    }
}

void apply_window(raptor_complex* in, float* taps, int count) {
    float* dst = (float*)in;
    for (int i = 0; i < count; i++) {
        *dst++ *= *taps;
        *dst++ *= *taps++;
    }
}

void raptor_fft_preview::push_block() {
    //Apply windowing function
    apply_window(fft_in, window, fft_size);

    //Compute the FFT
    fftwf_execute(plan);

    //Offset the spectrum to center
    offset_spectrum(fft_out, fft_size);

    //Calculate magnitude squared
    volk_32fc_magnitude_squared_32f(power, fft_out, fft_size);

    //Scale to dB
    //https://github.com/gnuradio/gnuradio/blob/1a0be2e6b54496a8136a64d86e372ab219c6559b/gr-utils/plot_tools/plot_fft_base.py#L88
    for (int i = 0; i < fft_size; i++)
        power[i] = 20 * log10(abs((power[i] + 1e-15f) / fft_size));

    //Send out
    output->preview_push(power, fft_size);
}