#include <Arduino.h>
#include <nvm.h>
#include <system.h>
#include <sam.h>
#include <eeprom.h>

static bool eeprom_configure_fuses() {
    nvm_fusebits fb;

    if (nvm_get_fuses(&fb) == STATUS_OK) {
        if (fb.eeprom_size != NVM_EEPROM_EMULATOR_SIZE_1024) {
            fb.eeprom_size = NVM_EEPROM_EMULATOR_SIZE_1024;
            if (nvm_set_fuses(&fb) == STATUS_OK) {
                Serial.println("Changed EEPROM Size:");
                Serial.println(fb.eeprom_size);
                return true;
            }
            else {
                Serial.println("Unable to set fuses.");
            }

        }
        else {
            Serial.println("EEPROM Size:");
            Serial.println(fb.eeprom_size);
            return true;
        }
    }
    else {
        Serial.println("Unable to get fuses.");
    }

    return false;
}

static void eeprom_emulation_configure(void) {
    enum status_code error_code = eeprom_emulator_init();
    if (error_code == STATUS_ERR_NO_MEMORY) {
        if (eeprom_configure_fuses()) {
            NVIC_SystemReset();
        }
        while (true) {
        }
    }
    else if (error_code != STATUS_OK) {
        /* Erase the emulated EEPROM memory (assume it is unformatted or
         * irrecoverably corrupt) */
        Serial.println("Erasing, remember programming erases all FLASH memory.");
        eeprom_emulator_erase_memory();
        eeprom_emulator_init();
    }
}

void setup() {
    Serial.begin(115200);

    while (!Serial) {
        delay(100);
    }

    eeprom_emulation_configure();

    uint8_t page[EEPROM_PAGE_SIZE];
    eeprom_emulator_read_page(0, page);

    page[0] = !page[0];

    eeprom_emulator_write_page(0, page);
    eeprom_emulator_commit_page_buffer();
}

void loop() {

}
