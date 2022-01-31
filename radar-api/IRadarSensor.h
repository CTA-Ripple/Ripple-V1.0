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

#ifndef I_RADAR_SENSOR_H_
#define I_RADAR_SENSOR_H_

#include "RadarCommon.h"

#include <inttypes.h>
#include <stdbool.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------
//----- Data types ---------------------
//--------------------------------------

// Forward declaration for a custom radar driver implementation.
typedef struct RadarHandleImpl RadarHandle;

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
RadarReturnCode radarInit(void);
/*
 * @brief De-initialize a radar module.
 *
 * @note This function should be called the most last of all radar API.
 */
RadarReturnCode radarDeinit(void);

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
RadarReturnCode radarDestroy(RadarHandle* handle);

// Power management.

/*
 * @brief Get the current power state.
 *
 * @param handle a handler for the radar instance to use.
 * @param state a pointer to the state that will be set.
 */
RadarReturnCode radarGetState(RadarHandle* handle, RadarState* state);

/*
 * @brief Turn on the radar.
 *
 * @param handle a handler for the radar instance to use.
 */
RadarReturnCode radarTurnOn(RadarHandle* handle);

/*
 * @brief Turn off the radar.
 *
 * @param handle a handler for the radar instance to use.
 */
RadarReturnCode radarTurnOff(RadarHandle* handle);

/*
 * @brief Put the radar to sleep and preserve configuration.
 *
 * @param handle a handler for the radar instance to use.
 */
RadarReturnCode radarGoSleep(RadarHandle* handle);

/*
 * @brief Wake up the radar.
 *
 * @param handle a handler for the radar instance to use.
 */
RadarReturnCode radarWakeUp(RadarHandle* handle);

// Configuration.

/*
 * @brief Set mode of the internal FIFO that holds radar bursts.
 *
 * @param handle a handler for the radar instance to use.
 * @param mode a new fifo mode for the internal buffer.
 */
RadarReturnCode radarSetFifoMode(RadarHandle* handle, RadarFifoMode mode);

/*
 * Get the total available configuration slots.
 *
 * @param handle a handler for the radar instance to use.
 * @param num_slots a pointer to where the number of config slots to write.
 */
RadarReturnCode radarGetNumConfigSlots(RadarHandle* handle, int8_t* num_slots);

/*
 * @brief Activate a specified configuration slot. Does not start the radar.
 *
 * @param handle a handler for the radar instance to use.
 * @param slot_id a configuration slot ID to activate.
 *
 * @note This function will perform the final configuration check for
 *       compatibility before activating.
 */
RadarReturnCode radarActivateConfig(RadarHandle* handle, int8_t slot_id);

/*
 * @brief Deactivate a specified configuration slot.
 *
 * @param handle a handler for the radar instance to use.
 * @param slot_id a configuration slot ID to deactivate.
 */
RadarReturnCode radarDeactivateConfig(RadarHandle* handle, int8_t slot_id);

/*
 * @brief Check if the configuration slot is active.
 *
 * @param handle a handler for the radar instance to use.
 * @param slot_id a configuration slot ID to check if active.
 * @param is_active a pointer to where the result will be written into.
 */
RadarReturnCode radarIsActiveConfig(RadarHandle* handle, int8_t slot_id,
    bool* is_active);

/*
 * @brief Get a main radar parameter.
 *
 * @param handle a handler for the radar instance to use.
 * @param slot_id a configuration slot ID where to read the parameter value.
 * @param id a parameter ID to be read.
 * @param value a pointer to where a parameter value will be written into.
 */
RadarReturnCode radarGetMainParam(RadarHandle* handle, uint32_t slot_id,
    RadarMainParam id, uint32_t* value);
/*
 * @brief Get a main radar parameter.
 *
 * @param handle a handler for the radar instance to use.
 * @param slot_id a configuration slot ID where to set a new parameter value.
 * @param id a parameter ID to be set.
 * @param value a new value for the parameter.
 */
RadarReturnCode radarSetMainParam(RadarHandle* handle, uint32_t slot_id,
    RadarMainParam id, uint32_t value);

/*
 * @brief Get a main radar parameter range of acceptable values.
 *
 * @param handle a handler for the radar instance to use.
 * @param id a parameter ID which range of values to read.
 * @param min_value a pointer where a minimum parameter value will be set.
 * @param max_value a pointer where a maximum parameter value will be set.
 */
RadarReturnCode radarGetMainParamRange(RadarHandle* handle, RadarMainParam id,
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
RadarReturnCode radarGetChannelParam(RadarHandle* handle, uint32_t slot_id,
    uint8_t channel_id, RadarChannelParam id, uint32_t* value);

/*
 * @brief Set a channel specific parameter.
 *
 * @param handle a handler for the radar instance to use.
 * @param slot_id a configuration slot ID where to set a new parameter value.
 * @param channel_id a channel ID which parameter needs to be set.
 * @param id a parameter ID to set.
 * @param value a new value for the parameter.
 */
RadarReturnCode radarSetChannelParam(RadarHandle* handle, uint32_t slot_id,
    uint8_t channel_id, RadarChannelParam id, uint32_t value);

/*
 * @brief Get a channel pamaeter range of acceptable values.
 *
 * @param handle a handler for the radar instance to use.
 * @param id a parameter ID which range of values to read.
 * @param min_value a pointer where a minimum parameter value will be set.
 * @param max_value a pointer where a maximum parameter value will be set.
 */
RadarReturnCode radarGetChannelParamRange(RadarHandle* handle,
    RadarChannelParam id, uint32_t* min_value, uint32_t* max_value);

/*
 * @brief Get a vendor specific parameter.
 *
 * @param handle a handler for the radar instance to use.
 * @param slot_id a configuration slot ID where to read a parameter value.
 * @param id a vendor specific parameter ID to read.
 * @param value a pointer to where a parameter value will be written into.
 */
RadarReturnCode radarGetVendorParam(RadarHandle* handle, uint32_t slot_id,
    RadarVendorParam id, uint32_t* value);

/*
 * @brief Set a vendor specific parameter.
 *
 * @param handle a handler for the radar instance to use.
 * @param slot_id a configuration slot ID where to set a new parameter value.
 * @param id a parameter ID to set.
 * @param value a new value for the parameter.
 */
RadarReturnCode radarSetVendorParam(RadarHandle* handle, uint32_t slot_id,
    RadarVendorParam id, uint32_t value);

// Running.

/*
 * @brief Start running the radar with active configuration.
 *
 * @param handle a handler for the radar instance to use.
 */
RadarReturnCode radarStartDataStreaming(RadarHandle* handle);

/*
 * @brief Stop running the radar.
 *
 * @param handle a handler for the radar instance to use.
 */
RadarReturnCode radarStopDataStreaming(RadarHandle* handle);

/*
 * @brief Check if the radar has a new burst ready to read.
 *
 * @param handle a handler for the radar instance to use.
 * @param is_ready a pointer where the result will be set.
 */
RadarReturnCode radarIsBurstReady(RadarHandle* handle, bool* is_ready);

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
RadarReturnCode radarReadBurst(RadarHandle* handle, RadarBurstFormat* format,
    uint8_t* buffer, uint32_t* read_bytes, struct timespec timeout);

// Feedback.

/*
 * @brief Set a callback that will be invoked when a new burst is ready to read.
 *
 * @param handle a handler for the radar instance to use.
 * @param cb a callback function.
 * @param user_data a pointer to user_data that will be passed to the callback.
 */
RadarReturnCode radarSetBurstReadyCb(RadarHandle* handle, RadarBurstReadyCB cb,
    void* user_data);

/*
 * @biref Set a callback that will be invoked with a log message from
 *        the radar API impl.
 *
 * @param handle a handler for the radar instance to use.
 * @param cb a callback function.
 * @param user_data a pointer to user_data that will be passed to the callback.
 */
RadarReturnCode radarSetLogCb(RadarHandle* handle, RadarLogCB cb,
    void* user_data);

/*
 * @brief Set a callback that will be invoked when a radar’s register is set.
 *
 * @param handle a handler for the radar instance to use.
 * @param cb a callback function.
 * @param user_data a pointer to user_data that will be passed to the callback.
 */
RadarReturnCode radarSetRegisterSetCb(RadarHandle* handle,
    RadarRegisterSetCB cb, void* user_data);

// Miscellaneous.

/*
 * @brief Set country code. If local regulations do not allow current sensor
 *        to operate, it should be turned off or faile to turn on.
 *
 * @param handle a handler for the radar instance to use.
 * @param country_code a ISO 3166-1 alpha-2 country code.
 */
RadarReturnCode radarSetCountryCode(RadarHandle* handle,
    const char* country_code);

/*
 * @brief Get radar sensor info.
 *
 * @param handle a handler for the radar instance to use.
 * @param info a pointer to the sensor info to be filled in.
 */
RadarReturnCode radarGetSensorInfo(RadarHandle* handle, SensorInfo* info);

/*
 * @brief Set a run time log level for radar API impl.
 *
 * @param handle a handler for the radar instance to use.
 * @param level new log level.
 */
RadarReturnCode radarSetLogLevel(RadarHandle* handle, RadarLogLevel level);

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
RadarReturnCode radarGetAllRegisters(RadarHandle* handle, uint32_t* addresses,
    uint32_t* values, uint32_t* count);

/*
 * @brief Get a register value directly from the sensor.
 *
 * @param handle a handler for the radar instance to use.
 * @param address an address to read.
 * @param value a pointer where the register's value will be written into.
 */
RadarReturnCode radarGetRegister(RadarHandle* handle, uint32_t address,
    uint32_t* value);

/*
 * @brief Set a register value directly to the sensor.
 *
 * @param handle a handler for the radar instance to use.
 * @param address an address of the register to set.
 * @param value a new value to set.
 */
RadarReturnCode radarSetRegister(RadarHandle* handle, uint32_t address,
    uint32_t value);

#ifdef __cplusplus
}
#endif

#endif // I_RADAR_SENSOR_H_
