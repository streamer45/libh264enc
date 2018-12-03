#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "libh264enc.h"

#define ERROR(fmt, ...) \
  { \
    fprintf(stderr, "E: %s@%s:%d ", __func__, __FILE__, __LINE__); \
    fprintf(stderr, fmt, ##__VA_ARGS__); \
    fprintf(stderr, "\n"); \
  }

int test() {
  int ret;
  int fd;
  h264enc_t *h264;
  h264enc_config_t config;
  h264_out_t out;

  memset(&config, 0, sizeof(config));

  config.width = 640;
  config.height = 480;
  config.bitrate = 500;
  config.fps = 60;
  config.threads = 2;
  config.gop = 60;
  config.level = 31;
  config.profile = H264_PROFILE_BASELINE;
  config.preset = H264_PRESET_ULTRAFAST;
  config.tune = H264_TUNE_ZEROLATENCY;
  config.fmt = H264_IMG_FMT_I420;

  h264 = h264enc_create(&config);

  if (!h264) return -1;

  fd = open("assets/src.yuv", O_RDONLY);

  if (fd < 0) return -2;

  int len = config.width * config.height * 1.5;

  uint8_t *data = malloc(len);

  for (;;) {
    ret = read(fd, data, len);
    if (ret == 0) break;
    if (ret != len) return -3;
    ret = h264enc_encode(h264, data, len, &out);
    if (ret != 0) return -4;
    for (int i = 0; i < out.n; ++i) {
      write(1, out.nals[i].data, out.nals[i].len);
    }
  }

  free(data);
  close(fd);

  ret = h264enc_destroy(h264);

  if (ret != 0) return -5;

  return 0;
}

int main(int argc, char const *argv[]) {
  int ret;
  ret = test();
  if (ret != 0) {
    ERROR("Failure! %d", ret);
    return ret;
  }
  return 0;
}
