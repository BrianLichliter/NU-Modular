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
 
#include "mbed.h"
#include "BLEDevice.h"
#include "BatteryService.h"
#include "CounterService.h"
#include "DeviceInformationService.h"
#include "mlx90614.h"


/**
 * For this demo application, populate the beacon advertisement payload
 * with 2 AD structures: FLAG and MSD (manufacturer specific data).
 *
 * Reference:
 *  Bluetooth Core Specification 4.0 (Vol. 3), Part C, Section 11, 18
 */

BLEDevice ble;
DigitalOut myled(LED1); //displays I2C wait
I2C i2c(p19,p18);   //sda,scl
Serial pc(USBTX,USBRX);  //serial usb config

MLX90614 IR_thermometer(&i2c);
//setup an MLX90614 using MLX90614 library from
// http://mbed.org/users/aquahika/libraries/MLX90614/lsixz6
 
float temp; //temperature in degrees C

const static char     DEVICE_NAME[]        = "NU Modular Watch";
static const uint16_t uuid16_list[]        = {GattService::UUID_COUNTER_SERVICE,
                                              GattService::UUID_BATTERY_SERVICE,
                                              GattService::UUID_DEVICE_INFORMATION_SERVICE};
static volatile bool  triggerSensorPolling = false;
                                              
void disconnectionCallback(Gap::Handle_t handle, Gap::DisconnectionReason_t reason)
{
    ble.startAdvertising(); // restart advertising
}

void onConnectionCallback(Gap::Handle_t handle, Gap::addr_type_t peerAddrType, const Gap::address_t peerAddr, const Gap::ConnectionParams_t *params)
{
    #if UPDATE_PARAMS_FOR_LONGER_CONNECTION_INTERVAL
    /* Updating connection parameters can be attempted only after a connection has been
     * established. Please note that the ble-Central is still the final arbiter for
     * the effective parameters; the peripheral can only hope that the request is
     * honored. Please also be mindful of the constraints that might be enforced by
     * the BLE stack on the underlying controller.*/
    #define MIN_CONN_INTERVAL 250  /**< Minimum connection interval (250 ms) */
    #define MAX_CONN_INTERVAL 350  /**< Maximum connection interval (350 ms). */
    #define CONN_SUP_TIMEOUT  6000 /**< Connection supervisory timeout (6 seconds). */
    #define SLAVE_LATENCY     4

        Gap::ConnectionParams_t gap_conn_params;
        gap_conn_params.minConnectionInterval        = Gap::MSEC_TO_GAP_DURATION_UNITS(MIN_CONN_INTERVAL);
        gap_conn_params.maxConnectionInterval        = Gap::MSEC_TO_GAP_DURATION_UNITS(MAX_CONN_INTERVAL);
        gap_conn_params.connectionSupervisionTimeout = Gap::MSEC_TO_GAP_DURATION_UNITS(CONN_SUP_TIMEOUT);
        gap_conn_params.slaveLatency                 = SLAVE_LATENCY;
        ble.updateConnectionParams(handle, &gap_conn_params);
    #endif /* #if UPDATE_PARAMS_FOR_LONGER_CONNECTION_INTERVAL */
}

void periodicCallback(void)
{
    /* Note that the periodicCallback() executes in interrupt context, so it is safer to do
     * heavy-weight sensor polling from the main thread. */
    triggerSensorPolling = true;
} 

int main(void)
{    
    Ticker ticker;
    ticker.attach(periodicCallback, 1);

    ble.init();
    ble.onDisconnection(disconnectionCallback);
#if UPDATE_PARAMS_FOR_LONGER_CONNECTION_INTERVAL
    ble.onConnection(onConnectionCallback);
#endif /* #if UPDATE_PARAMS_FOR_LONGER_CONNECTION_INTERVAL */

    /* Setup auxiliary services. */
    CounterService           count(ble);
    BatteryService           battery(ble);
    DeviceInformationService deviceInfo(ble, "ARM", "Model1", "SN1", "hw-rev1", "fw-rev1", "soft-rev1");
    
    /* Setup advertising. */
    ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)uuid16_list, sizeof(uuid16_list));
    ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));
 
    /* Set advertising interval. Longer interval = longer battery life */
    ble.setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.setAdvertisingInterval(Gap::MSEC_TO_ADVERTISEMENT_DURATION_UNITS(5000));
    ble.startAdvertising();

    int counter = 0;

    while (true) {
        if (triggerSensorPolling && ble.getGapState().connected) {
            triggerSensorPolling = false;
            myled = 1;
            //printf("polling!\r\n");
            if (IR_thermometer.getTemp(&temp)) {
                //gets temperature from sensor via I2C bus
                myled=0;
                //print temperature on PC
                printf("Temperature is %5.1F degrees C\r\n",temp);
            }
            counter++;        
            count.updateCount(temp);
            
        } else {
            ble.waitForEvent();
        }
    }
}
