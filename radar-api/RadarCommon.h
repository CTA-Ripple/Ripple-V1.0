/*
 * Copyright 2022 CTA Radar API Technical Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef RADAR_API_RADAR_COMMON_H_
#define RADAR_API_RADAR_COMMON_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------
//----- Constants ----------------------
//--------------------------------------

// A list of possible status/return codes that API can report back.
typedef uint16_t RadarReturnCode;

// A default undefined value that should be used at initialization.
#define RC_UNDEFINED        0
// Operation completed successfully.
#define RC_OK               1
// Operation failed and no more information can be provided.
#define RC_ERROR            2
// Input parameters are invalid or out of supported range.
#define RC_BAD_INPUT        3
// Operation timed out.
#define RC_TIMEOUT          4
// Operation cannot be performed at the current state.
#define RC_BAD_STATE        5
// Operation failed due to limited resources (memory, timers, mutexes, etc).
#define RC_RES_LIMIT        6
// Operation is not supported.
#define RC_UNSUPPORTED      7
// An internal system error that should never happen.
#define RC_OOPS             8

// A list of possible power mode states for radar sensors.
typedef uint16_t RadarState;

// A default undefined value that should be used at initialization.
#define RSTATE_UNDEFINED    0
// Active state when radar is emitting/collecting data started.
#define RSTATE_ACTIVE       1
// Idle state when the radar is neither ACTIVE nor SLEEP nor OFF.
#define RSTATE_IDLE         2
// Sleep state when configuration persists but power consumption reduced.
#define RSTATE_SLEEP        3
// When radar is currently turned off and configuration is reset.
#define RSTATE_OFF          4

// Defineis how an internal fifo buffer should behave in case of overflow.
typedef uint16_t RadarFifoMode;

// A default undefined value that should be used at initialization.
#define RFIFO_UNDEFINED     0
// A new burst will be ignored.
#define RFIFO_DROP_NEW      1
// The oldest burst(s) will be dropped to release space for a new burst.
#define RFIFO_DROP_OLD      2

// Log level
typedef uint32_t RadarLogLevel;

// A default undefined value that should be used at initialization.
#define RLOG_UNDEFINED      0
// None of log messages are requested.
#define RLOG_OFF            1
// Provide only log messages about occurred errors.
#define RLOG_ERR            2
// Provide log messages same as for RLOG_ERR and warnings.
#define RLOG_WRN            3
// Provide log messages same as for RLOG_WRN and informative changes.
#define RLOG_INF            4
// Provide log messages same as for RLOG_INF and debugging info details.
#define RLOG_DBG            5


//--------------------------------------
//----- Params -------------------------
//--------------------------------------

// A list of radar sensor parameters that define main characteristics.
// A configuration slot can hold only 1 value for each MainParam.
typedef uint32_t RadarMainParam;

// A default undefined value that should be used at initialization.
#define RADAR_PARAM_UNDEFINED               0
// Power mode for after the burst period.
#define RADAR_PARAM_AFTERBURST_POWER_MODE   1
// Power mode for the period between chirps.
#define RADAR_PARAM_INTERCHIRP_POWER_MODE   2
// Duration between the start times of two consecutive bursts.
#define RADAR_PARAM_BURST_PERIOD_US         3
// Duration between the start times of two consecutive chirps.
#define RADAR_PARAM_CHIRP_PERIOD_US         4
// Amount of chirps within the burst.
#define RADAR_PARAM_CHIRPS_PER_BURST        5
// The number of ADC sample values captured for each chirp.
#define RADAR_PARAM_SAMPLES_PER_CHIRP       6
// The lower frequency at what TX antenna starts emitting the signal.
#define RADAR_PARAM_LOWER_FREQ_MHZ          7
// The upper frequency at what TX antenna stops emitting the signal.
#define RADAR_PARAM_UPPER_FREQ_MHZ          8
// Bit mask for enabled TX antennas.
#define RADAR_PARAM_TX_ANTENNA_MASK         9
// Bit mask for enabled RX antennas.
#define RADAR_PARAM_RX_ANTENNA_MASK        10
// Power for TX antennas.
#define RADAR_PARAM_TX_POWER               11
// ADC sampling frequency.
#define RADAR_PARAM_ADC_SAMPLING_HZ        12

// A channel specific list of parameters.
typedef uint32_t RadarChannelParam;

// A default undefined value that should be used at initialization.
#define CHANNEL_PARAM_UNDEFINED             0
// Variable Gain Amplifiers (VGA) in dB.
#define CHANNEL_PARAM_VGA_DB                1
// High Phase (HP) filter gain in dB.
#define CHANNEL_PARAM_HP_GAIN_DB            2
// High Phase (HP) cut off frequency in kHz.
#define CHANNEL_PARAM_HP_CUTOFF_KHZ         3


// Forward declaration for a list of vensor specific parameters.
typedef uint32_t RadarVendorParam;

//--------------------------------------
//----- Data types ---------------------
//--------------------------------------

// Describes a data format of burst data.
typedef struct RadarBurstFormat_s {
  // Sequence number for the current burst.
  uint32_t sequence_number;
  // Maximum value ADC sampler produces.
  uint32_t max_sample_value;
  // Amount of bits per single sample.
  uint8_t bits_per_sample;
  // Amount of samples per single chirp.
  uint16_t samples_per_chirp;
  // Amount of active channels in the current burst.
  uint8_t channels_count;
  // Amount of chirps in the current burst.
  uint8_t chirps_per_burst;
  // Config slot ID used to generate the current burst.
  uint8_t config_id;
  union {
    struct {
      // True/1 if channels and samples are interleaved.
      uint16_t is_channels_interlieved: 1;
      // True/1 if a word is in big endian.
      uint16_t is_big_endian: 1;
      // Reserved for future use.
      uint16_t reserved: 14;
    };
    uint16_t flags;
  };
  // CRC for the current burst data buffer.
  uint32_t burst_data_crc;
  // Timestamp when the burst was created since radar was turned on.
  uint32_t timestamp_ms;
} RadarBurstFormat;

// A semantic version holder.
typedef struct Version_s {
  uint8_t major;
  uint8_t minor;
  uint8_t patch;
  uint8_t build;
} Version;

#define MAX_SENSOR_NAME_LEN 32
#define MAX_VENDOR_NAME_LEN 32

// The general information about the radar sensor hardware and software.
typedef struct SensorInfo_s {
  // Name of the radar sensor.
  const char name[MAX_SENSOR_NAME_LEN];
  // Vendor name.
  const char vendor[MAX_VENDOR_NAME_LEN];
  // ID that identifies this sensor.
  uint32_t device_id;
  // The radar driver version.
  Version driver_version;
  // The Radar API version that the current driver supports.
  Version api_version;
  // Maximum ADC sampling rate in hertz.
  uint64_t max_sampling_rate_hz;
  // Current sensor state
  RadarState state;
} SensorInfo;

#ifdef __cplusplus
}
#endif

#endif // RADAR_API_RADAR_COMMON_H_
