/*
 * Copyright 2021 Google LLC
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

#ifndef I_RADAR_SENSOR_H_
#define I_RADAR_SENSOR_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------
//----- Enums --------------------------
//--------------------------------------

// A list of possible status/return codes that API can report back.
typedef enum {
  // A default undefined value that should be used at initialization.
  RC_UNDEFINED = 0,
  // Operation completed successfully.
  RC_OK,
  // Operation failed and no more information can be provided.
  RC_ERROR,
  // Input parameters are invalid or out of supported range.
  RC_BAD_INPUT,
  // Operation timed out.
  RC_TIMEOUT,
  // Operation cannot be performed at the current state.
  RC_BAD_STATE,
  // Operation failed due to limited resources (memory, timers, mutexes, etc).
  RC_RES_LIMIT,
  // Operation is not supported.
  RC_UNSUPPORTED,
  // An internal system error that should never happen.
  RC_OOPS
} ReturnCode;

// A list of possible power mode states for radar sensors.
typedef enum {
  // A default undefined value that should be used at initialization.
  RSTATE_UNDEFINED = 0,
  // Active state when radar is emitting/collecting data started.
  RSTATE_ACTIVE,
  // Idle state when the radar is neither ACTIVE nor SLEEP nor OFF.
  RSTATE_IDLE,
  // Sleep state when configuration persists but power consumption reduced.
  RSTATE_SLEEP,
  // When radar is currently turned off and configuration is reset.
  RSTATE_OFF
} RadarState;

// Defineis how an internal fifo buffer should behave in case of overflow.
typedef enum {
  // A default undefined value that should be used at initialization.
  RFIFO_UNDEFINED = 0,
  // A new burst will be ignored.
  RFIFO_DROP_NEW,
  // The oldest burst(s) will be dropped to release space for a new burst.
  RFIFO_DROP_OLD
} RadarFifoMode;

//--------------------------------------
//----- Params -------------------------
//--------------------------------------

// A list of radar sensor parameters that define main characteristics.
// A configuration slot can hold only 1 value for each MainParam.
typedef enum {
  // A default undefined value that should be used at initialization.
  RADAR_PARAM_UNDEFINED = 0,
  // Power mode for after the burst period.
  RADAR_PARAM_AFTERBURST_POWER_MODE,
  // Power mode for the period between chirps.
  RADAR_PARAM_INTERCHIRP_POWER_MODE,
  // Duration between the start times of two consecutive bursts.
  RADAR_PARAM_BURST_PERIOD_US,
  // Duration between the start times of two consecutive chirps.
  RADAR_PARAM_CHIRP_PERIOD_US,
  // Amount of chirps within the burst.
  RADAR_PARAM_CHIRPS_PER_BURST,
  // The number of ADC sample values captured for each chirp.
  RADAR_PARAM_SAMPLES_PER_CHIRP,
  // The lower frequency at what TX antenna starts emitting the signal.
  RADAR_PARAM_LOWER_FREQ_MHZ,
  // The upper frequency at what TX antenna stops emitting the signal.
  RADAR_PARAM_UPPER_FREQ_MHZ,
  // Bit mask for enabled TX antennas.
  RADAR_PARAM_TX_ANTENNA_MASK,
  // Bit mask for enabled RX antennas.
  RADAR_PARAM_RX_ANTENNA_MASK,
  // Power for TX antennas.
  RADAR_PARAM_TX_POWER,
  // ADC sampling frequency.
  RADAR_PARAM_ADC_SAMPLING_HZ
} RadarMainParam;

// A channel specific list of parameters.
typedef enum {
  // A default undefined value that should be used at initialization.
  CHANNEL_PARAM_UNDEFINED = 0,
  // Variable Gain Amplifiers (VGA) in dB.
  CHANNEL_PARAM_VGA_DB,
  // High Phase (HP) filter gain in dB.
  CHANNEL_PARAM_HP_GAIN_DB,
  // High Phase (HP) cut off frequency in kHz.
  CHANNEL_PARAM_HP_CUTOFF_KHZ
} RadarChannelParam;

typedef enum {
  // A default undefined value that should be used at initialization.
  RLOG_UNDEFINED = 0,
  // None of log messages are requested.
  RLOG_OFF,
  // Provide only log messages about occurred errors.
  RLOG_ERRR,
  // Provide log messages same as for RLOG_ERR and warnings.
  RLOG_WARN,
  // Provide log messages same as for RLOG_WRN and informative changes.
  RLOG_INFO,
  // Provide log messages same as for RLOG_INF and debugging info details.
  RLOG_DBG
} RadarLogLevel;

//--------------------------------------
//----- Data types ---------------------
//--------------------------------------

// Forward declaration for a custom radar driver implementation.
typedef struct RadarHandlerImpl RadarHandler;

// Forward declaration for a list of vensor specific parameters.
typedef enum RadarVendorParamImpl RadarVendorParam;

// Describes a data format of burst data.
typedef struct BurstFormat_s {
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
} BurstFormat;

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

/*
 * @brief A callback function declaration that will be invoked
 *        when a new burst is ready for read. Can be set using
 *        radarSetBurstReadyCb function.
 *
 * @param user_data a pointer to a custom user data that is passed to
 *        radarSetBurstReadyCb function.
 *
 * @return void
 */
typedef void (*RadarBurstReadyCB)(void* user_data);

/*
 * @brief A callback function declaration that will be invoked
 *        when a new log message is available from the radar driver.
 *        The callback can be registered with radarSetLogCb function.
 *        Run time log level can be set using radarSetLogLevel function.
 *
 * @param level a level of the current log message.
 * @param file a source file name where the log message is generated.
 * @param function a function name where the log message is generated.
 * @param line a line number within a file where the log message is generated.
 * @param user_data a pointer to a custom user data that is passed to
 *        radarSetLogCb function.
 * @param message the actuall log message.
 *
 * @return void
 */
typedef void (*RadarLogCB)(RadarLogLevel level, const char* file,
                           const char* function, int line,
                           void* user_data, const char* message);

/*
 * @brief A callback function declaration that will be invoked
 *        when the radar driver sets a new value for the sensor chip register.
 *
 * @param address a register's address.
 * @param value a new value to be set in the register.
 * @param user_data a pointer to a custom user data that is passed to
 *        radarSetRegisterSetCb function.
 *
 * @return void.
 */
typedef void (*RadarRegisterSetCB)(uint32_t address, uint32_t value,
                                   void* user_data);

//--------------------------------------
//----- API ----------------------------
//--------------------------------------

// Lifecycle.

/*
 * @brief Initialize a radar module.
 *
 * @note This function should be called the most first of all radar API.
 */
ReturnCode radarInit(void);
/*
 * @brief De-initialize a radar module.
 *
 * @note This function should be called the most last of all radar API.
 */
ReturnCode radarDeinit(void);

/*
 * @brief Create a radar module instance.
 *
 * @param id a unique identifier of the radar chip.
 *        Can be used to differentiate if multiple radar available at the
 *        same time.
 *
 * @return A handler for a newly created radar instance.
 */
RadarHandle* radarCreate(int32_t id);

/*
 * @brief Destroy a radar module instance.
 *
 * @param handle the handle of the radar instance to be destroyed.
 */
ReturnCode radarDestroy(RadarHandle* handle);

// Power management.

/*
 * @brief Get the current power state.
 *
 * @param handle a handler for the radar instance to use.
 * @param state a pointer to the state that will be set.
 */
ReturnCode radarGetState(RadarHandle* handle, RadarState* state);

/*
 * @brief Turn on the radar.
 *
 * @param handle a handler for the radar instance to use.
 */
ReturnCode radarTurnOn(RadarHandle* handle);

/*
 * @brief Turn off the radar.
 *
 * @param handle a handler for the radar instance to use.
 */
ReturnCode radarTurnOff(RadarHandle* handle);

/*
 * @brief Put the radar to sleep and preserve configuration.
 *
 * @param handle a handler for the radar instance to use.
 */
ReturnCode radarGoSleep(RadarHandle* handle);

/*
 * @brief Wake up the radar.
 *
 * @param handle a handler for the radar instance to use.
 */
ReturnCode radarWakeUp(RadarHandle* handle);

// Configuration.

/*
 * @brief Set mode of the internal FIFO that holds radar bursts.
 *
 * @param handle a handler for the radar instance to use.
 * @param mode a new fifo mode for the internal buffer.
 */
ReturnCode radarSetFifoMode(RadarHandle* handle, RadarFifoMode mode);

/*
 * Get the total available configuration slots.
 *
 * @param handle a handler for the radar instance to use.
 * @param num_slots a pointer to where the number of config slots to write.
 */
ReturnCode radarGetNumConfigSlots(RadarHandle* handle, int8_t* num_slots);

/*
 * @brief Activate a specified configuration slot. Does not start the radar.
 *
 * @param handle a handler for the radar instance to use.
 * @param slot_id a configuration slot ID to activate.
 *
 * @note This function will perform the final configuration check for
 *       compatibility before activating.
 */
ReturnCode radarActivateConfig(RadarHandle* handle, int8_t slot_id);

/*
 * @brief Deactivate a specified configuration slot.
 *
 * @param handle a handler for the radar instance to use.
 * @param slot_id a configuration slot ID to deactivate.
 */
ReturnCode radarDeactivateConfig(RadarHandle* handle, int8_t slot_id);

/*
 * @brief Check if the configuration slot is active.
 *
 * @param handle a handler for the radar instance to use.
 * @param slot_id a configuration slot ID to check if active.
 * @param is_active a pointer to where the result will be written into.
 */
ReturnCode radarIsActiveConfig(RadarHandle* handle, int8_t slot_id,
                               bool* is_active);

/*
 * @brief Get a main radar parameter.
 *
 * @param handle a handler for the radar instance to use.
 * @param slot_id a configuration slot ID where to read the parameter value.
 * @param id a parameter ID to be read.
 * @param value a pointer to where a parameter value will be written into.
 */
ReturnCode radarGetMainParam(RadarHandle* handle, uint32_t slot_id,
                             RadarMainParam id, uint32_t* value);
/*
 * @brief Get a main radar parameter.
 *
 * @param handle a handler for the radar instance to use.
 * @param slot_id a configuration slot ID where to set a new parameter value.
 * @param id a parameter ID to be set.
 * @param value a new value for the parameter.
 */
ReturnCode radarSetMainParam(RadarHandle* handle, uint32_t slot_id,
                             RadarMainParam id, uint32_t value);

/*
 * @brief Get a main radar parameter range of acceptable values.
 *
 * @param handle a handler for the radar instance to use.
 * @param id a parameter ID which range of values to read.
 * @param min_value a pointer where a minimum parameter value will be set.
 * @param max_value a pointer where a maximum parameter value will be set.
 */
ReturnCode radarGetMainParamRange(RadarHandle* handle, RadarMainParam id,
                                  uint32_t* min_value, uint32_t* max_value);

/*
 * @brief Get a channel specific parameter.
 *
 * @param handle a handler for the radar instance to use.
 * @param slot_id a configuration slot ID where to read the parameter value.
 * @param channel_id a channel ID which parameter needs to be read.
 * @param id a parameter ID to read.
 * @param value a pointer to where a parameter value will be written into.
 */
ReturnCode radarGetChannelParam(RadarHandle* handle, uint32_t slot_id,
                                uint8_t channel_id, RadarChannelParam id,
                                uint32_t* value);

/*
 * @brief Set a channel specific parameter.
 *
 * @param handle a handler for the radar instance to use.
 * @param slot_id a configuration slot ID where to set a new parameter value.
 * @param channel_id a channel ID which parameter needs to be set.
 * @param id a parameter ID to set.
 * @param value a new value for the parameter.
 */
ReturnCode radarSetChannelParam(RadarHandle* handle, uint32_t slot_id,
                                uint8_t channel_id, RadarChannelParam id,
                                uint32_t value);

/*
 * @brief Get a channel pamaeter range of acceptable values.
 *
 * @param handle a handler for the radar instance to use.
 * @param id a parameter ID which range of values to read.
 * @param min_value a pointer where a minimum parameter value will be set.
 * @param max_value a pointer where a maximum parameter value will be set.
 */
ReturnCode radarGetChannelParamRange(RadarHandle* handle, RadarChannelParam id,
                                     uint32_t* min_value, uint32_t* max_value);

/*
 * @brief Get a vendor specific parameter.
 *
 * @param handle a handler for the radar instance to use.
 * @param slot_id a configuration slot ID where to read a parameter value.
 * @param id a vendor specific parameter ID to read.
 * @param value a pointer to where a parameter value will be written into.
 */
ReturnCode radarGetVendorParam(RadarHandle* handle, uint32_t slot_id,
                               RadarVendorParam id, uint32_t* value);

/*
 * @brief Set a vendor specific parameter.
 *
 * @param handle a handler for the radar instance to use.
 * @param slot_id a configuration slot ID where to set a new parameter value.
 * @param id a parameter ID to set.
 * @param value a new value for the parameter.
 */
ReturnCode radarSetVendorParam(RadarHandle* handle, uint32_t slot_id,
                               RadarVendorParam id, uint32_t value);

// Running.

/*
 * @brief Start running the radar with active configuration.
 *
 * @param handle a handler for the radar instance to use.
 */
ReturnCode radarStartDataStreaming(RadarHandle* handle);

/*
 * @brief Stop running the radar.
 *
 * @param handle a handler for the radar instance to use.
 */
ReturnCode radarStopDataStreaming(RadarHandle* handle);

/*
 * @brief Check if the radar has a new burst ready to read.
 *
 * @param handle a handler for the radar instance to use.
 * @param is_ready a pointer where the result will be set.
 */
ReturnCode radarIsBurstReady(RadarHandle* handle, bool* is_ready);

/*
 * @brief Initiate reading a new burst.
 *
 * @param handle a handler for the radar instance to use.
 * @param format a pointer where a new burst format will be written into.
 * @param buffer a pointer where a burst data to write.
 * @param read_bytes a pointer where the maximum buffer size is set.
 *        When function finishes, the pointer will have the amount of bytes
 *        have been read.
 * @param timeout the maximum time to wait if the burst frame is not ready.
 */
ReturnCode radarReadBurst(RadarHandle* handle, RadarBurstFormat* format,
                          uint8_t* buffer, uint32_t* read_bytes,
                          timespec_t timeout);

// Feedback.

/*
 * @brief Set a callback that will be invoked when a new burst is ready to read.
 *
 * @param handle a handler for the radar instance to use.
 * @param cb a callback function.
 * @param user_data a pointer to user_data that will be passed to the callback.
 */
ReturnCode radarSetBurstReadyCb(RadarHandle* handle, RadarBurstReadyCB cb,
                                void* user_data);

/*
 * @biref Set a callback that will be invoked with a log message from
 *        the radar API impl.
 *
 * @param handle a handler for the radar instance to use.
 * @param cb a callback function.
 * @param user_data a pointer to user_data that will be passed to the callback.
 */
ReturnCode radarSetLogCb(RadarHandle* handle, RadarLogCB cb, void* user_data);

/*
 * @brief Set a callback that will be invoked when a radar’s register is set.
 *
 * @param handle a handler for the radar instance to use.
 * @param cb a callback function.
 * @param user_data a pointer to user_data that will be passed to the callback.
 */
ReturnCode radarSetRegisterSetCb(RadarHandle* handle, RadarRegisterSetCB cb,
                                 void* user_data);

// Miscellaneous.

/*
 * @brief Set country code. If local regulations do not allow current sensor
 *        to operate, it should be turned off or faile to turn on.
 *
 * @param handle a handler for the radar instance to use.
 * @param country_code a ISO 3166-1 alpha-2 country code.
 */
ReturnCode radarSetCountryCode(RadarHandle* handle, const char* country_code);

/*
 * @brief Get radar sensor info.
 *
 * @param handle a handler for the radar instance to use.
 * @param info a pointer to the sensor info to be filled in.
 */
ReturnCode radarGetSensorInfo(RadarHandle* handle, SensorInfo* info);

/*
 * @brief Set a run time log level for radar API impl.
 *
 * @param handle a handler for the radar instance to use.
 * @param level new log level.
 */
ReturnCode radarSetLogLevel(RadarHandle* handle, RadarLogLevel level);

/*
 * @brief Get all register values from the radar sensor.
 *
 * @param handle a handler for the radar instance to use.
 * @param addresses a pointer where register addresses will be written into.
 * @param values a pointer where register values will be written into.
 * @param count a pointer where the maximum amount of elemnets for register
 *        addresses and values can set. When function returns, the value
 *        under the pointer will have the number of address-value pairs
 *        have been set.
 */
ReturnCode radarGetAllRegisters(RadarHandle* handle, uint32_t* addresses,
                                uint32_t* values, uint32_t* count);

/*
 * @brief Get a register value directly from the sensor.
 *
 * @param handle a handler for the radar instance to use.
 * @param address an address to read.
 * @param value a pointer where the register's value will be written into.
 */
ReturnCode radarGetRegister(RadarHandle* handle, uint32_t address,
                            uint32_t* value);

/*
 * @brief Set a register value directly to the sensor.
 *
 * @param handle a handler for the radar instance to use.
 * @param address an address of the register to set.
 * @param value a new value to set.
 */
ReturnCode radarSetRegister(RadarHandle* handle, uint32_t address, uint32_t value);

#ifdef __cplusplus
}
#endif

#endif // I_RADAR_SENSOR_H_
