#include "mbed.h"
#include <cstdlib>

#define WRITEVAL 13

// Saw this as a method to create a pointer with a specific address
// https://stackoverflow.com/questions/3934775/how-to-initialize-a-pointer-to-a-specific-memory-address-in-c
uint32_t *testWrite = reinterpret_cast<uint32_t *>(0x1000);

void setup() {
    Serial.begin(115200);
    while (!Serial) {
    };
    Serial.println((uint32_t)testWrite);
    Serial.println(*testWrite);
    NRF_NVMC->CONFIG = 2;
    if (*testWrite != 0xFFFFFFFF && *testWrite != WRITEVAL)
        // If page was not empty earlier
        NRF_NVMC->ERASEPAGE = (uint32_t)testWrite;
    else
        Serial.println("Page is still not empty");

    // The first time the device gets turned on, this should write to flash and
    // then just spin lock. Defaults to 0xFFFFFFFF because the NVM is
    // initialized to all 1s. I think.
    if (*testWrite == 0xFFFFFFFF) {
        *testWrite = WRITEVAL;
        // Just waits until we are done with the write to continue
        while (!NRF_NVMC->READY) {
        };
        NRF_NVMC->CONFIG = 0;
    }
}

void loop() {
    // The conditional is probably unnecessary but I felt like using it
    if (*testWrite != 0xFFFFFFFF) {
        Serial.println(*testWrite);
    }
}
