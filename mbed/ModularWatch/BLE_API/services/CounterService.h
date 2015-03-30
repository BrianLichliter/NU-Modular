/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __BLE_COUNTER_SERVICE_H__
#define __BLE_COUNTER_SERVICE_H__

#include "BLEDevice.h"

/**
* @class BatteryService
* @brief BLE Battery Service. This service displays the battery level from 0%->100% represented as a 8bit number.<br>
* Service:  https://developer.bluetooth.org/gatt/services/Pages/ServiceViewer.aspx?u=org.bluetooth.service.battery_service.xml <br>
* Battery Level Char:  https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.battery_level.xml
*/
class CounterService {
public:
    /**
    * @param[ref] _ble
    *               BLEDevice object for the underlying controller.
    * @param[in] level
    *               Test counting service
    */
    CounterService(BLEDevice &_ble, uint8_t start = 1) :
        ble(_ble),
        count(start),
        countCharacteristic(GattCharacteristic::UUID_COUNT_CHAR, &count, sizeof(count), sizeof(count),
                                   GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY) {

        static bool serviceAdded = false; /* We should only ever need to add the service once. */
        if (serviceAdded) {
            return;
        }

        GattCharacteristic *charTable[] = {&countCharacteristic};
        GattService         countService(GattService::UUID_COUNTER_SERVICE, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));

        ble.addService(countService);
        serviceAdded = true;
    }

    /**
     * @brief Update the battery level with a new value. Valid values range from
     * 0..100. Anything outside this range will be ignored.
     *
     * @param newLevel
     *              update to battery level.
     */
    void updateCount(uint8_t newCount) {
        count = newCount;
        ble.updateCharacteristicValue(countCharacteristic.getValueAttribute().getHandle(), &count, 1);
    }

private:
    BLEDevice          &ble;
    uint8_t             count;
    GattCharacteristic  countCharacteristic;
};

#endif /* #ifndef __BLE_BATTERY_SERVICE_H__*/