#pragma once

#include <util/ansiTypes.h>
#include <util/builtins.h>
typedef int16 FMSampleType;
typedef int16 PCMSampleType;
static const uint8 config_dac_bits = 14;
static const int16 config_psg_preamp = 100;
static const int32 config_fm_preamp = 100;
static const uint8 config_overscan = 0;//3;
static const uint8 config_render = 0;
static const uint8 config_hq_fm = 1; // TODO: non-hq mode causes random seg-faults
static const uint8 config_filter = 0;
static const uint8 config_clipSound = 0;
static const uint8 config_psgBoostNoise = 0;
static const int16 config_lp_range = 50;
static const int16 config_low_freq = 880;
static const int16 config_high_freq = 5000;
static const int16 config_lg = 1;
static const int16 config_mg = 1;
static const int16 config_hg = 1;
static const SysDDec config_rolloff = 0.990;
extern uint config_ym2413_enabled;
static const int16 config_ym2612_clip = 1;
static const uint8 config_force_dtack = 0;
static const uint8 config_addr_error = 1;

#define MAX_INPUTS 8

typedef struct
{
  unsigned char padtype;
} t_input_config;

typedef struct 
{
  uint8 region_detect;
  uint8 tmss;
  uint8 lock_on;
  uint8 hot_swap;
  uint8 romtype;
  t_input_config input[MAX_INPUTS];
} t_config;
