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

#ifndef I_RADAR_SENSOR_HPP_
#define I_RADAR_SENSOR_HPP_

#include <cstdint>
#include <string>
#include <time.h>
#include <vector>

//--------------------------------------
//----- Enums --------------------------
//--------------------------------------

namespace radar_api {

// A list of possible status/return codes that API can report back.
enum ReturnCode {
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
};

// A list of possible power mode states for radar sensors.
enum RadarState {
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
};

// Defineis how an internal fifo buffer should behave in case of overflow.
enum FifoMode {
  // A default undefined value that should be used at initialization.
  RFIFO_UNDEFINED = 0,
  // A new burst will be ignored.
  RFIFO_DROP_NEW,
  // The oldest burst(s) will be dropped to release space for a new burst.
  RFIFO_DROP_OLD
};

//--------------------------------------
//----- Params -------------------------
//--------------------------------------

// A list of radar sensor parameters that define main characteristics.
// A configuration slot can hold only 1 value for each MainParam.
enum MainParam {
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
};

// A channel specific list of parameters.
enum ChannelParam {
  // A default undefined value that should be used at initialization.
  CHANNEL_PARAM_UNDEFINED = 0,
  // Variable Gain Amplifiers (VGA) in dB.
  CHANNEL_PARAM_VGA_DB,
  // High Phase (HP) filter gain in dB.
  CHANNEL_PARAM_HP_GAIN_DB,
  // High Phase (HP) cut off frequency in kHz.
  CHANNEL_PARAM_HP_CUTOFF_KHZ
};

enum LogLevel {
  // A default undefined value that should be used at initialization.
  RLOG_UNDEFINED = 0,
  // None of log messages are requested.
  RLOG_OFF,
  // Provide only log messages about occurred errors.
  RLOG_ERR,
  // Provide log messages same as for RLOG_ERR and warnings.
  RLOG_WRN,
  // Provide log messages same as for RLOG_WRN and informative changes.
  RLOG_INF,
  // Provide log messages same as for RLOG_INF and debugging info details.
  RLOG_DBG
};

//--------------------------------------
//----- Data types ---------------------
//--------------------------------------

// Forward declaration for a list of vensor specific parameters.
enum VendorParam : uint16_t;

// Describes a data format of burst data.
struct BurstFormat {
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
  bool is_channels_interlieved;
  bool is_big_endian;
  // CRC for the current burst data buffer.
  uint32_t burst_data_crc;
  // Timestamp when the burst was created since radar was turned on.
  uint32_t timestamp_ms;
};

// A semantic version holder.
struct Version {
  uint8_t major;
  uint8_t minor;
  uint8_t patch;
  uint8_t build;
};

// The general information about the radar sensor hardware and software.
struct SensorInfo {
  // Name of the radar sensor.
  std::string name;
  // Vendor name.
  std::string vendor;
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
};

/*
 * @brief A Radar data observer that allows application to be notified
 *        about it's events.
 */
class IRadarSensorObserver {
  public:
  /*
   * @brief An interface function declaration that will be invoked
   *        when a new burst is ready for read. Can be set using
   *        radarSetBurstReadyCb function.
   *
   * @return void
   */
  virtual void OnBurstReady(void) = 0;

  /*
   * @brief An interface function declaration that will be invoked
   *        when a new log message is available from the radar driver.
   *        The callback can be registered with radarSetLogCb function.
   *        Run time log level can be set using radarSetLogLevel function.
   *
   * @param level a level of the current log message.
   * @param file a source file name where the log message is generated.
   *        The value is set by a compiler and embedded into the code.
   * @param function a function name where the log message is generated.
   *        The value is set by a compiler and embedded into the code.
   * @param line a line number within a file where the log message is generated.
   * @param message a completely formed log message.
   *
   * @return void
   */
  virtual void OnLogMessage(int level, const char* file,
                            const char* function,
                            int line, const std::string& message) = 0;

  /*
   * @brief An interface function declaration that will be invoked
   *        when the radar driver sets a new value for the sensor chip register.
   *
   * @param address a register's address.
   * @param value a new value to be set in the register.
   *
   * @return void.
   */
  virtual void OnRegisterSet(uint32_t address, uint32_t value) = 0;
};

//--------------------------------------
//----- API ----------------------------
//--------------------------------------

class IRadarSensor {
 public:
  // Feedback

  /*
   * @brief Add a new observer object that will get notified about
   * the Radar sensor activity.
   *
   * @param observer pointer to the implmenetation of the interface to add.
   */
  virtual ReturnCode AddObserver(IRadarSensorObserver* observer) = 0;

  /*
   * @brief Remove the previously registered observer from subscribers list.
   *
   * @param observer pointer to the implmenetation of the interface to remove.
   */
  virtual ReturnCode RemoveObserver(IRadarSensorObserver* observer) = 0;


  // Power management.

  /*
   * @brief Get the current power state.
   *
   * @param state a pointer to the state that will be set.
   */
  virtual ReturnCode GetRadarState(RadarState& state) = 0;

  /*
   * @brief Turn on the radar.
   */
  virtual ReturnCode TurnOn(void) = 0;

  /*
   * @brief Turn off the radar.
   */
  virtual ReturnCode TurnOff(void) = 0;

  /*
   * @brief Put the radar to sleep and preserve configuration.
   */
  virtual ReturnCode GoSleep(void) = 0;

  /*
   * @brief Wake up the radar.
   */
  virtual ReturnCode WakeUp(void) = 0;

  // Configuration.

  /*
   * @brief Set mode of the internal FIFO that holds radar bursts.
   *
   * @param mode a new fifo mode for the internal buffer.
   */
  virtual ReturnCode SetFifoMode(FifoMode mode) = 0;

  /*
   * Get the total available configuration slots.
   *
   * @param num_slots where the number of config slots to write.
   */
  virtual ReturnCode GetNumConfigSlots(int8_t& num_slots) = 0;

  /*
   * @brief Activate a specified configuration slot. Does not start the radar.
   *
   * @param slot_id a configuration slot ID to activate.
   *
   * @note This function will perform the final configuration check for
   *       compatibility before activating.
   */
  virtual ReturnCode ActivateConfig(uint8_t slot_id) = 0;

  /*
   * @brief Deactivate a specified configuration slot.
   *
   * @param slot_id a configuration slot ID to deactivate.
   */
  virtual ReturnCode DeactivateConfig(uint8_t slot_id) = 0;

  /*
   * @brief Check if the configuration slot is active.
   *
   * @param slot_ids a vector to be filled with IDs of
   *        active config slots.
   */
  virtual ReturnCode GetActiveConfigs(std::vector<uint8_t>& slot_ids) = 0;

  /*
   * @brief Get a main radar parameter.
   *
   * @param slot_id a configuration slot ID where to read the parameter value.
   * @param id a parameter ID to be read.
   * @param value where a parameter value will be written into.
   */
  virtual ReturnCode GetMainParam(uint32_t slot_id, MainParam id,
                                  uint32_t& value) = 0;

  /*
   * @brief Get a main radar parameter.
   *
   * @param slot_id a configuration slot ID where to set a new parameter value.
   * @param id a parameter ID to be set.
   * @param value a new value for the parameter.
   */
  virtual ReturnCode SetMainParam(uint32_t slot_id, MainParam id,
                                  uint32_t value) = 0;

  /*
   * @brief Get a main radar parameter range of acceptable values.
   *
   * @param id a parameter ID which range of values to read.
   * @param min_value where a minimum parameter value will be set.
   * @param max_value where a maximum parameter value will be set.
   */
  virtual ReturnCode GetMainParamRange(MainParam id, uint32_t& min_value,
                                       uint32_t& max_value) = 0;

  /*
   * @brief Get a channel specific parameter.
   *
   * @param slot_id a configuration slot ID where to read the parameter value.
   * @param channel_id a channel ID which parameter needs to be read.
   * @param id a parameter ID to read.
   * @param value to where a parameter value will be written into.
   */
  virtual ReturnCode GetChannelParam(uint32_t slot_id, uint8_t channel_id,
                                     ChannelParam id, uint32_t& value) = 0;

  /*
   * @brief Set a channel specific parameter.
   *
   * @param slot_id a configuration slot ID where to set a new parameter value.
   * @param channel_id a channel ID which parameter needs to be set.
   * @param id a parameter ID to set.
   * @param value a new value for the parameter.
   */
  virtual ReturnCode SetChannelParam(uint32_t slot_id, uint8_t channel_id,
                                     ChannelParam id, uint32_t value) = 0;

  /*
   * @brief Get a channel pamaeter range of acceptable values.
   *
   * @param id a parameter ID which range of values to read.
   * @param min_value where a minimum parameter value will be set.
   * @param max_value where a maximum parameter value will be set.
   */
  virtual ReturnCode GetChannelParamRange(ChannelParam id, uint32_t& min_value,
                                          uint32_t& max_value) = 0;

  /*
   * @brief Get a vendor specific parameter.
   *
   * @param slot_id a configuration slot ID where to read a parameter value.
   * @param id a vendor specific parameter ID to read.
   * @param value to where a parameter value will be written into.
   */
  virtual ReturnCode GetVendorParam(uint32_t slot_id, VendorParam id, uint32_t& value) = 0;

  /*
   * @brief Set a vendor specific parameter.
   *
   * @param slot_id a configuration slot ID where to set a new parameter value.
   * @param id a parameter ID to set.
   * @param value a new value for the parameter.
   */
  virtual ReturnCode SetVendorParam(uint32_t slot_id, VendorParam id, uint32_t value) = 0;

  // Running.

  /*
   * @brief Start running the radar with active configuration.
   */
  virtual ReturnCode StartDataStreaming(void) = 0;

  /*
   * @brief Stop running the radar.
   */
  virtual ReturnCode StopDataStreaming(void) = 0;

  /*
   * @brief Check if the radar has a new burst ready to read.
   *
   * @param is_ready where the result will be set.
   */
  virtual ReturnCode IsBurstReady(bool& is_ready) = 0;

  /*
   * @brief Initiate reading a new burst.
   *
   * @param format where a new burst format will be written into.
   * @param raw_radar_data where a burst data to be written.
   * @param timeout the maximum time to wait if the burst frame is not ready.
   */
  virtual ReturnCode ReadBurst(BurstFormat& format,
                               std::vector<uint8_t>& raw_radar_data,
                               timespec timeout) = 0;

  // Miscellaneous.

  /*
   * @brief Set country code. If local regulations do not allow current sensor
   *        to operate, it should be turned off or faile to turn on.
   *
   * @param country_code a ISO 3166-1 alpha-2 country code.
   */
  virtual ReturnCode SetCountryCode(const std::string& country_code) = 0;

  /*
   * @brief Get radar sensor info.
   *
   * @param info where the sensor info to be filled in.
   */
  virtual ReturnCode GetSensorInfo(SensorInfo& info) = 0;

  /*
   * @brief Set a run time log level for radar API impl.
   *
   * @param level new log level.
   */
  virtual ReturnCode SetLogLevel(LogLevel level) = 0;

  /*
   * @brief Get all register values from the radar sensor.
   *
   * @param registers where the pair of register's address and value
   *        to be written.
   */
  virtual ReturnCode GetAllRegisters(
      std::vector<std::pair<uint32_t, uint32_t>>& registers) = 0;

  /*
   * @brief Get a register value directly from the sensor.
   *
   * @param address an address to read.
   * @param value where the register's value will be written into.
   */
  virtual ReturnCode GetRegister(uint32_t address, uint32_t& value) = 0;
  /*
   * @brief Set a register value directly to the sensor.
   *
   * @param address an address of the register to set.
   * @param value a new value to set.
   */
  virtual ReturnCode SetRegister(uint32_t address, uint32_t value) = 0;
};

// Return the intsance of the RadarSensor implementation.
IRadarSensor* GetRadarSensorImpl(void);

} // namespace radar_api

#endif // I_RADAR_SENSOR_HPP_
