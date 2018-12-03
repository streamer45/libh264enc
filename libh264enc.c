#include "libh264enc.h"
#include <x264.h>

struct h264enc {
  x264_t *enc;
  x264_param_t param;
  h264enc_config_t config;
  h264_nal_t nals[32];
};

static const char *profiles[] = {
  "none",
  "baseline",
  "main",
  "high"
};

static const char *presets[] = {
  "none",
  "ultrafast",
  "fast"
};

static const char *tunings[] = {
  "none",
  "zerolatency"
};

static const int formats[] = {
  0,
  X264_CSP_I420
};

static int h264enc_load_param(h264enc_t *enc) {
  x264_param_t *param;
  h264enc_config_t *cfg;

  param = &enc->param;
  cfg = &enc->config;

  x264_param_default(param);
  x264_param_default_preset(param, presets[cfg->preset], tunings[cfg->tune]);
  x264_param_apply_profile(param, profiles[cfg->profile]);

  param->i_width = cfg->width;
  param->i_height = cfg->height;
  param->rc.i_bitrate = cfg->bitrate;
  param->rc.i_rc_method = X264_RC_ABR;
  param->i_threads = cfg->threads;
  param->i_fps_num = cfg->fps;
  param->i_fps_den = 1;
  // param->i_timebase_num = 60;
  // param->i_timebase_den = 1;
  param->i_level_idc = cfg->level;
  param->i_keyint_max = cfg->gop;
  param->b_intra_refresh = 1;

  param->b_repeat_headers = 1;
  param->b_annexb = 1;
  param->i_csp = formats[cfg->fmt];

  return 0;
}

h264enc_t *h264enc_create(h264enc_config_t *cfg) {
  h264enc_t *h264;

  if (!cfg) return NULL;

  h264 = calloc(1, sizeof(*h264));

  h264->config = *cfg;
  h264enc_load_param(h264);
  h264->enc = x264_encoder_open(&h264->param);

  if (!h264->enc) {
    free(h264);
    return NULL;
  }

  return h264;
}

int h264enc_destroy(h264enc_t *h264) {
  if (!h264) return -1;
  x264_encoder_close(h264->enc);
  free(h264);
  return 0;
}

int h264enc_encode(h264enc_t *h264, uint8_t *in, size_t len, h264_out_t *out) {
  int i;
  int ret;
  int nnals;
  x264_nal_t *nals;
  x264_picture_t in_pic;
  x264_picture_t out_pic;

  if (!h264 || !in || !len || !out) return -1;
  if (len < h264->config.width * h264->config.height * 1.5) return -2;

  x264_picture_init(&in_pic);
  x264_picture_init(&out_pic);

  out_pic.param = &h264->param;
  in_pic.param = &h264->param;
  in_pic.img.i_csp = h264->param.i_csp;

  if (h264->config.fmt == H264_IMG_FMT_I420) {
    in_pic.img.i_plane = 3;
    in_pic.img.i_stride[0] = h264->config.width;
    in_pic.img.i_stride[1] = h264->config.width / 2;
    in_pic.img.i_stride[2] = h264->config.width / 2;
    in_pic.img.plane[0] = in;
    in_pic.img.plane[1] = in + (h264->config.width
     * h264->config.height);
    in_pic.img.plane[2] = in_pic.img.plane[1] + (h264->config.width
     * h264->config.height / 4);
  } else return -45;

  ret = x264_encoder_encode(h264->enc, &nals, &nnals, &in_pic, &out_pic);

  if (ret < 0) return -3;

  for (i = 0; i < nnals; ++i) {
    h264->nals[i].data = nals[i].p_payload;
    h264->nals[i].len = nals[i].i_payload;
    h264->nals[i].padding = nals[i].i_padding;
    h264->nals[i].type = nals[i].i_type;
  }

  out->nals = (h264_nal_t *)&h264->nals;
  out->n = nnals;

  return 0;
}
