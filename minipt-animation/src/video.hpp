#ifndef _VIDEO_HPP_
#define _VIDEO_HPP_

#include <bits/stdc++.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
};

#include "image.hpp"

class VideoEncoder
{
    const AVCodec *codec;
    AVCodecContext *c;
    int frame_count, ret, x, y, got_output;
    FILE *f;
    AVFrame *frame;
    AVPacket pkt;
    uint8_t endcode[4];
    const char *filename;
    const char *codec_name;
    int video_w;
    int video_h;
    int bitrate;
    int fps;
    int gop;

    void encode_frame(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt,
                      FILE *outfile)
    {
        int ret;
        if (frame)
            printf("Send frame %3" PRId64 "\n", frame->pts);
        ret = avcodec_send_frame(enc_ctx, frame);
        if (ret < 0)
        {
            fprintf(stderr, "Error sending a frame for encoding\n");
            exit(1);
        }
        while (ret >= 0)
        {
            ret = avcodec_receive_packet(enc_ctx, pkt);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                return;
            else if (ret < 0)
            {
                fprintf(stderr, "Error during encoding\n");
                exit(1);
            }
            printf("Write packet %3" PRId64 " (size=%5d)\n", pkt->pts, pkt->size);
            fwrite(pkt->data, 1, pkt->size, outfile);
            av_packet_unref(pkt);
        }
    }

public:
    VideoEncoder(const char *filename, const char *codec_name, int video_w, int video_h, int bitrate, int fps, int gop)
        : filename(filename), codec_name(codec_name), video_w(video_w), video_h(video_h), bitrate(bitrate), fps(fps), gop(gop)
    {
        endcode[0] = 0;
        endcode[1] = 0;
        endcode[2] = 1;
        endcode[3] = 0xb7;
        c = NULL;

        avcodec_register_all();

        codec = avcodec_find_encoder_by_name(codec_name);
        if (!codec)
        {
            fprintf(stderr, "Codec not found\n");
            exit(1);
        }
        c = avcodec_alloc_context3(codec);
        if (!c)
        {
            fprintf(stderr, "Could not allocate video codec context\n");
            exit(1);
        }

        c->bit_rate = bitrate;
        c->width = video_w;
        c->height = video_h;
        c->time_base = (AVRational){1, fps};
        c->framerate = (AVRational){fps, 1};

        c->gop_size = gop;
        c->max_b_frames = 1;
        c->pix_fmt = AV_PIX_FMT_YUV420P;

        if (codec->id == AV_CODEC_ID_H264)
            av_opt_set(c->priv_data, "preset", "slow", 0);

        if (avcodec_open2(c, codec, NULL) < 0)
        {
            fprintf(stderr, "Could not open codec\n");
            exit(1);
        }

        f = fopen(filename, "wb");
        if (!f)
        {
            fprintf(stderr, "Could not open %s\n", filename);
            exit(1);
        }

        frame = av_frame_alloc();
        if (!frame)
        {
            fprintf(stderr, "Could not allocate video frame\n");
            exit(1);
        }
        frame->format = c->pix_fmt;
        frame->width = c->width;
        frame->height = c->height;

        ret = av_frame_get_buffer(frame, 32);
        if (ret < 0)
        {
            fprintf(stderr, "Could not allocate the video frame data\n");
            exit(1);
        }

        av_init_packet(&pkt);
        pkt.data = NULL;
        pkt.size = 0;
        fflush(stdout);

        ret = av_frame_make_writable(frame);
        if (ret < 0)
            exit(1);

        frame_count = 0;
    }

    void NewFrame(Image_RGB888 &image)
    {
        frame_count++;

        for (y = 0; y < c->height; y++)
        {
            for (x = 0; x < c->width; x++)
            {
                vec3_uint8 color = image.Get(x, y);
                uint8_t Y, U, V;
                rgb2yuv(color, Y, U, V);
                frame->data[0][y * frame->linesize[0] + x] = Y;
            }
        }

        for (y = 0; y + y < c->height; y++)
        {
            for (x = 0; x + x < c->width; x++)
            {
                vec3_uint8 color = image.Get(2 * x, 2 * y) / 4 + image.Get(2 * x + 1, 2 * y) / 4 + image.Get(2 * x, 2 * y + 1) / 4 + image.Get(2 * x + 1, 2 * y + 1) / 4;
                uint8_t Y, U, V;
                rgb2yuv(color, Y, U, V);
                frame->data[1][y * frame->linesize[1] + x] = U;
                frame->data[2][y * frame->linesize[2] + x] = V;
            }
        }

        frame->pts = frame_count;
        encode_frame(c, frame, &pkt, f);
    }

    void NewFrame(Image &image)
    {
        frame_count++;

        for (y = 0; y < c->height; y++)
        {
            for (x = 0; x < c->width; x++)
            {
                vec3 color = image.Get(x, y);
                uint8_t Y, U, V;
                rgb2yuv({color.x * 255, color.y * 255, color.z * 255}, Y, U, V);
                frame->data[0][y * frame->linesize[0] + x] = Y;
            }
        }

        for (y = 0; y + y < c->height; y++)
        {
            for (x = 0; x + x < c->width; x++)
            {
                vec3 color = image.Get(2 * x, 2 * y) / 4 + image.Get(2 * x + 1, 2 * y) / 4 + image.Get(2 * x, 2 * y + 1) / 4 + image.Get(2 * x + 1, 2 * y + 1) / 4;
                uint8_t Y, U, V;
                rgb2yuv({color.x * 255, color.y * 255, color.z * 255}, Y, U, V);
                frame->data[1][y * frame->linesize[1] + x] = U;
                frame->data[2][y * frame->linesize[2] + x] = V;
            }
        }

        frame->pts = frame_count;
        encode_frame(c, frame, &pkt, f);
    }

    void End()
    {
        encode_frame(c, frame, &pkt, f);
        fwrite(endcode, 1, sizeof(endcode), f);
        fclose(f);

        avcodec_free_context(&c);
        av_frame_free(&frame);
    }
};

#endif