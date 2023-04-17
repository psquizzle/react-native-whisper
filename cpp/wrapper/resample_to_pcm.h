

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include <cmath>
#include <cstdint>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
}

using namespace std;

bool is_pcm_s16le_wav(const std::string &filename)
{

    // Open the input file and read its format
    AVFormatContext *format_ctx = nullptr;
    if (avformat_open_input(&format_ctx, filename.c_str(), nullptr, nullptr) != 0)
    {
        return false;
    }

    // Retrieve stream information
    if (avformat_find_stream_info(format_ctx, nullptr) < 0)
    {
        avformat_close_input(&format_ctx);
        return false;
    }

    // Check if the format is WAV and the codec is pcm_s16le
    bool is_pcm_s16le = false;
    if (format_ctx->iformat->name && strcmp(format_ctx->iformat->name, "wav") == 0)
    {
        for (unsigned int i = 0; i < format_ctx->nb_streams; ++i)
        {
            AVCodecParameters *codec_params = format_ctx->streams[i]->codecpar;
            if (codec_params->codec_type == AVMEDIA_TYPE_AUDIO &&
                codec_params->codec_id == AV_CODEC_ID_PCM_S16LE)
            {
                is_pcm_s16le = true;
                break;
            }
        }
    }

    // Clean up and return the result
    avformat_close_input(&format_ctx);
    return is_pcm_s16le;
}

std::vector<uint8_t> convert_audio(const std::string &input_filepath)
{
    std::vector<uint8_t> output_samples;
    // Register all codecs, formats, and protocols

    AVFormatContext *input_format_ctx = nullptr;
    if (avformat_open_input(&input_format_ctx, input_filepath.c_str(), nullptr, nullptr) < 0)
    {
        std::cerr << "Error opening input file." << std::endl;
        return output_samples;
    }

    if (avformat_find_stream_info(input_format_ctx, nullptr) < 0)
    {
        std::cerr << "Error finding stream information." << std::endl;
        return output_samples;
    }

    int audio_stream_idx = av_find_best_stream(input_format_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (audio_stream_idx < 0)
    {
        std::cerr << "Error finding audio stream." << std::endl;
        return output_samples;
    }

    AVStream *input_audio_stream = input_format_ctx->streams[audio_stream_idx];
    AVCodecParameters *input_codec_params = input_audio_stream->codecpar;
    const AVCodec *input_codec = avcodec_find_decoder(input_codec_params->codec_id);

    if (!input_codec)
    {
        std::cerr << "Error finding input codec." << std::endl;
        return output_samples;
    }

    AVCodecContext *input_codec_ctx = avcodec_alloc_context3(input_codec);
    if (!input_codec_ctx)
    {
        std::cerr << "Error allocating input codec context." << std::endl;
        return output_samples;
    }

    if (avcodec_parameters_to_context(input_codec_ctx, input_codec_params) < 0)
    {
        std::cerr << "Error setting parameters for input codec context." << std::endl;
        return output_samples;
    }

    if (avcodec_open2(input_codec_ctx, input_codec, nullptr) < 0)
    {
        std::cerr << "Error opening input codec." << std::endl;
        return output_samples;
    }

    // Setup resampler
    SwrContext *swr_ctx = swr_alloc_set_opts(
        nullptr,
        AV_CH_LAYOUT_MONO,
        AV_SAMPLE_FMT_S16,
        16000,
        input_codec_ctx->channel_layout,
        static_cast<AVSampleFormat>(input_codec_ctx->sample_fmt),
        input_codec_ctx->sample_rate,
        0,
        nullptr);

    if (!swr_ctx)
    {
        std::cerr << "Error allocating resampler context." << std::endl;
        return output_samples;
    }

    if (swr_init(swr_ctx) < 0)
    {
        std::cerr << "Error initializing resampler context." << std::endl;
        return output_samples;
    }

    // Read and decode the input audio stream
    AVPacket packet;
    AVFrame *decoded_frame = av_frame_alloc();

    while (av_read_frame(input_format_ctx, &packet) >= 0)
    {
        if (packet.stream_index == audio_stream_idx)
        {
            if (avcodec_send_packet(input_codec_ctx, &packet) >= 0)
            {
                while (avcodec_receive_frame(input_codec_ctx, decoded_frame) >= 0)
                {
                    // Convert the frame using the resampler
                    int max_out_samples = av_rescale_rnd(
                        swr_get_delay(swr_ctx, input_codec_ctx->sample_rate) + decoded_frame->nb_samples,
                        16000,
                        input_codec_ctx->sample_rate,
                        AV_ROUND_UP);

                    uint8_t *out_buffer = nullptr;
                    av_samples_alloc(&out_buffer, nullptr, 1, max_out_samples, AV_SAMPLE_FMT_S16, 0);

                    int out_samples = swr_convert(
                        swr_ctx,
                        &out_buffer,
                        max_out_samples,
                        (const uint8_t **)decoded_frame->data,
                        decoded_frame->nb_samples);
                    output_samples.insert(output_samples.end(), out_buffer, out_buffer + out_samples * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16));

                    av_freep(&out_buffer);
                }
            }
        }
        av_packet_unref(&packet);
    }

    // Free all resources
    av_frame_free(&decoded_frame);
    swr_free(&swr_ctx);
    avcodec_free_context(&input_codec_ctx);
    avformat_close_input(&input_format_ctx);

    return output_samples;
}