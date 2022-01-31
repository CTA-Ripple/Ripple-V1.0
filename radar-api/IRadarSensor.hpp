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

#ifndef I_RADAR_SENSOR_HPP_
#define I_RADAR_SENSOR_HPP_

#include "RadarCommon.h"

#include <cstdint>
#include <string>
#include <time.h>
#include <vector>

//--------------------------------------
//----- Enums --------------------------
//--------------------------------------

namespace radar_api {

//--------------------------------------
//----- Data types ---------------------
//--------------------------------------

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
  virtual RadarReturnCode AddObserver(IRadarSensorObserver* observer) = 0;

  /*
   * @brief Remove the previously registered observer from subscribers list.
   *
   * @param observer pointer to the implmenetation of the interface to remove.
   */
  virtual RadarReturnCode RemoveObserver(IRadarSensorObserver* observer) = 0;


  // Power management.

  /*
   * @brief Get the current power state.
   *
   * @param state a pointer to the state that will be set.
   */
  virtual RadarReturnCode GetRadarState(RadarState& state) = 0;

  /*
   * @brief Turn on the radar.
   */
  virtual RadarReturnCode TurnOn(void) = 0;

  /*
   * @brief Turn off the radar.
   */
  virtual RadarReturnCode TurnOff(void) = 0;

  /*
   * @brief Put the radar to sleep and preserve configuration.
   */
  virtual RadarReturnCode GoSleep(void) = 0;

  /*
   * @brief Wake up the radar.
   */
  virtual RadarReturnCode WakeUp(void) = 0;

  // Configuration.

  /*
   * @brief Set mode of the internal FIFO that holds radar bursts.
   *
   * @param mode a new fifo mode for the internal buffer.
   */
  virtual RadarReturnCode SetFifoMode(RadarFifoMode mode) = 0;

  /*
   * Get the total available configuration slots.
   *
   * @param num_slots where the number of config slots to write.
   */
  virtual RadarReturnCode GetNumConfigSlots(int8_t& num_slots) = 0;

  /*
   * @brief Activate a specified configuration slot. Does not start the radar.
   *
   * @param slot_id a configuration slot ID to activate.
   *
   * @note This function will perform the final configuration check for
   *       compatibility before activating.
   */
  virtual RadarReturnCode ActivateConfig(uint8_t slot_id) = 0;

  /*
   * @brief Deactivate a specified configuration slot.
   *
   * @param slot_id a configuration slot ID to deactivate.
   */
  virtual RadarReturnCode DeactivateConfig(uint8_t slot_id) = 0;

  /*
   * @brief Check if the configuration slot is active.
   *
   * @param slot_ids a vector to be filled with IDs of
   *        active config slots.
   */
  virtual RadarReturnCode GetActiveConfigs(std::vector<uint8_t>& slot_ids) = 0;

  /*
   * @brief Get a main radar parameter.
   *
   * @param slot_id a configuration slot ID where to read the parameter value.
   * @param id a parameter ID to be read.
   * @param value where a parameter value will be written into.
   */
  virtual RadarReturnCode GetMainParam(uint32_t slot_id, RadarMainParam id,
      uint32_t& value) = 0;

  /*
   * @brief Get a main radar parameter.
   *
   * @param slot_id a configuration slot ID where to set a new parameter value.
   * @param id a parameter ID to be set.
   * @param value a new value for the parameter.
   */
  virtual RadarReturnCode SetMainParam(uint32_t slot_id, RadarMainParam id,
      uint32_t value) = 0;

  /*
   * @brief Get a main radar parameter range of acceptable values.
   *
   * @param id a parameter ID which range of values to read.
   * @param min_value where a minimum parameter value will be set.
   * @param max_value where a maximum parameter value will be set.
   */
  virtual RadarReturnCode GetMainParamRange(RadarMainParam id,
      uint32_t& min_value, uint32_t& max_value) = 0;

  /*
   * @brief Get a channel specific parameter.
   *
   * @param slot_id a configuration slot ID where to read the parameter value.
   * @param channel_id a channel ID which parameter needs to be read.
   * @param id a parameter ID to read.
   * @param value to where a parameter value will be written into.
   */
  virtual RadarReturnCode GetChannelParam(uint32_t slot_id, uint8_t channel_id,
      RadarChannelParam id, uint32_t& value) = 0;

  /*
   * @brief Set a channel specific parameter.
   *
   * @param slot_id a configuration slot ID where to set a new parameter value.
   * @param channel_id a channel ID which parameter needs to be set.
   * @param id a parameter ID to set.
   * @param value a new value for the parameter.
   */
  virtual RadarReturnCode SetChannelParam(uint32_t slot_id, uint8_t channel_id,
      RadarChannelParam id, uint32_t value) = 0;

  /*
   * @brief Get a channel pamaeter range of acceptable values.
   *
   * @param id a parameter ID which range of values to read.
   * @param min_value where a minimum parameter value will be set.
   * @param max_value where a maximum parameter value will be set.
   */
  virtual RadarReturnCode GetChannelParamRange(RadarChannelParam id,
      uint32_t& min_value, uint32_t& max_value) = 0;

  /*
   * @brief Get a vendor specific parameter.
   *
   * @param slot_id a configuration slot ID where to read a parameter value.
   * @param id a vendor specific parameter ID to read.
   * @param value to where a parameter value will be written into.
   */
  virtual RadarReturnCode GetVendorParam(uint32_t slot_id, RadarVendorParam id,
      uint32_t& value) = 0;

  /*
   * @brief Set a vendor specific parameter.
   *
   * @param slot_id a configuration slot ID where to set a new parameter value.
   * @param id a parameter ID to set.
   * @param value a new value for the parameter.
   */
  virtual RadarReturnCode SetVendorParam(uint32_t slot_id, RadarVendorParam id,
      uint32_t value) = 0;

  // Running.

  /*
   * @brief Start running the radar with active configuration.
   */
  virtual RadarReturnCode StartDataStreaming(void) = 0;

  /*
   * @brief Stop running the radar.
   */
  virtual RadarReturnCode StopDataStreaming(void) = 0;

  /*
   * @brief Check if the radar has a new burst ready to read.
   *
   * @param is_ready where the result will be set.
   */
  virtual RadarReturnCode IsBurstReady(bool& is_ready) = 0;

  /*
   * @brief Initiate reading a new burst.
   *
   * @param format where a new burst format will be written into.
   * @param raw_radar_data where a burst data to be written.
   * @param timeout the maximum time to wait if the burst frame is not ready.
   */
  virtual RadarReturnCode ReadBurst(RadarBurstFormat& format,
      std::vector<uint8_t>& raw_radar_data, timespec timeout) = 0;

  // Miscellaneous.

  /*
   * @brief Set country code. If local regulations do not allow current sensor
   *        to operate, it should be turned off or faile to turn on.
   *
   * @param country_code a ISO 3166-1 alpha-2 country code.
   */
  virtual RadarReturnCode SetCountryCode(const std::string& country_code) = 0;

  /*
   * @brief Get radar sensor info.
   *
   * @param info where the sensor info to be filled in.
   */
  virtual RadarReturnCode GetSensorInfo(SensorInfo& info) = 0;

  /*
   * @brief Set a run time log level for radar API impl.
   *
   * @param level new log level.
   */
  virtual RadarReturnCode SetLogLevel(RadarLogLevel level) = 0;

  /*
   * @brief Get all register values from the radar sensor.
   *
   * @param registers where the pair of register's address and value
   *        to be written.
   */
  virtual RadarReturnCode GetAllRegisters(
      std::vector<std::pair<uint32_t, uint32_t>>& registers) = 0;

  /*
   * @brief Get a register value directly from the sensor.
   *
   * @param address an address to read.
   * @param value where the register's value will be written into.
   */
  virtual RadarReturnCode GetRegister(uint32_t address, uint32_t& value) = 0;
  /*
   * @brief Set a register value directly to the sensor.
   *
   * @param address an address of the register to set.
   * @param value a new value to set.
   */
  virtual RadarReturnCode SetRegister(uint32_t address, uint32_t value) = 0;
};

// Return the intsance of the RadarSensor implementation.
IRadarSensor* GetRadarSensorImpl(void);

} // namespace radar_api

#endif // I_RADAR_SENSOR_HPP_
