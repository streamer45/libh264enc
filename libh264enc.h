#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define ERROR(fmt, ...) \
  { \
    fprintf(stderr, "E: %s@%s:%d ", __func__, __FILE__, __LINE__); \
    fprintf(stderr, fmt, ##__VA_ARGS__); \
    fprintf(stderr, "\n"); \
  }

typedef struct h264enc h264enc_t;

typedef enum {
  H264_PROFILE_NONE = 0,
  H264_PROFILE_BASELINE,
  H264_PROFILE_MAIN,
  H264_PROFILE_HIGH
} h264_profile;

typedef enum {
  H264_PRESET_NONE = 0,
  H264_PRESET_ULTRAFAST,
  H264_PRESET_FAST
} h264_preset;

typedef enum {
  H264_TUNE_NONE = 0,
  H264_TUNE_ZEROLATENCY
} h264_tune;

typedef enum {
  H264_IMG_FMT_NONE = 0,
  H264_IMG_FMT_I420
} h264_img_fmt;

typedef struct {
  uint width;
  uint height;
  uint bitrate;
  uint fps;
  uint level;
  uint threads;
  uint gop;
  h264_profile profile;
  h264_preset preset;
  h264_tune tune;
  h264_img_fmt fmt;
} h264enc_config_t;

typedef struct {
  int type;
  uint8_t *data;
  int len;
  int padding;
} h264_nal_t;

typedef struct {
  h264_nal_t *nals;
  int n;
} h264_out_t;

h264enc_t *h264enc_create(h264enc_config_t *cfg);
int h264enc_destroy(h264enc_t *h264);
int h264enc_encode(h264enc_t *h264, uint8_t *in, size_t len, h264_out_t *out);
