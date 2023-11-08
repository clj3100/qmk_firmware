/* Copyright 2021 Glorious, LLC <salman@pcgamingrace.com>
 * Modified 2022 by rustedaperture for qmk_firmware

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H
#include "analog.h"
#include "qmk_midi.h"

// char* textstrings[] = {"trey1016@gmail.com","craig@cljones.info", "clj3100@me.com"};

// #define TXT_MACRO(i) SEND_STRING(textstrings[1])

enum custom_keycodes {
    MD_BOOT = SAFE_RANGE,
    EMAIL1,
    EMAIL2,
    EMAIL3,
    RGB_TOGL,
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

//      NUM      /       *       -
//      7        8       9       +
//      4        5       6       ENCODER
//      1        2       3       RET
//      0                        .

  [0] = LAYOUT(
    LT(1,KC_NUM),   KC_PSLS,   KC_PAST,   KC_PMNS,
    KC_P7,   KC_P8,     KC_P9,     KC_PPLS,
    KC_P4,   KC_P5,     KC_P6,     KC_MEDIA_PLAY_PAUSE,
    KC_P1,   KC_P2,     KC_P3,     KC_PENT,
    LT(2,KC_P0),                         KC_PDOT
  ),
  [1] = LAYOUT(
    _______,   _______,   _______,   _______,
    _______,     RGB_VAI,   RGB_SPI,     _______,
    RGB_RMOD,  RGB_TOGL,     RGB_MOD,   _______,
    _______,     RGB_VAD,   RGB_SPD,     _______,
    _______,                         MD_BOOT
  ),
  [2] = LAYOUT(
    _______,   KC_PSLS,   _______,   KC_PMNS,
    KC_P7,   KC_P8,     KC_P9,     KC_PPLS,
    _______,   KC_P5,     KC_P6,     KC_ASSISTANT,
    EMAIL1,   EMAIL2,     EMAIL3,     KC_PENT,
    KC_P0,                         KC_PDOT
  ),
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static uint32_t key_timer;

    switch(keycode) {
        case MD_BOOT:
            if (record->event.pressed) {
                key_timer = timer_read32();
            } else {
                if (timer_elapsed32(key_timer) >= 500) {
                    reset_keyboard();
                }
            }
            return false;
        case EMAIL1:
            if (record->event.pressed) {
                SEND_STRING("trey1016@gmail.com");
            }
            return false;
        case EMAIL2:
            if (record->event.pressed) {
                SEND_STRING("craig@cljones.info");
            }
            return false;
        case EMAIL3:
            if (record->event.pressed) {
                SEND_STRING("clj3100@me.com");
            }
            return false;
        case RGB_TOGL:
            if (record->event.pressed) {
              switch (rgb_matrix_get_flags()) {
                case LED_FLAG_ALL: {
                    rgb_matrix_set_flags(LED_FLAG_KEYLIGHT | LED_FLAG_MODIFIER | LED_FLAG_INDICATOR);
                    rgb_matrix_set_color_all(0, 0, 0);
                  }
                  break;
                default: {
                    rgb_matrix_set_flags(LED_FLAG_ALL);
                    rgb_matrix_enable_noeeprom();
                  }
                  break;
              }
            }
            return false;
        default:
            return true; //Process all other keycodes normally
    }

}

bool encoder_update_user(uint8_t index, bool clockwise) {
    if (layer_state_is(2)) {
        if (clockwise) {
            register_code(KC_LCTL);
            tap_code(KC_F1);
            unregister_code(KC_LCTL);
        } else {
            register_code(KC_LCTL);
            tap_code(KC_F2);
            unregister_code(KC_LCTL);
        }
    } else {
        if (clockwise) {
            register_code(KC_LCTL);
            tap_code(KC_F11);
            unregister_code(KC_LCTL);
        } else {
            register_code(KC_LCTL);
            tap_code(KC_F12);
            unregister_code(KC_LCTL);
        }
    }
    return false;
}

// Potentiometer Slider, MIDI Control

uint8_t divisor = 0;

// void slider(void) {
//     if (divisor++) { /* only run the slider function 1/256 times it's called */
//         return;
//     }
//     midi_send_cc(&midi_device, 2, 0x3E, 0x7F + (analogReadPin(SLIDER_PIN) >> 3));
// }

uint8_t last_read = 0;

void slider(void) {
    uint8_t current_read = (analogReadPin(SLIDER_PIN) +last_read)/8; //filter strength

    if (current_read != last_read ) {
        midi_send_cc(&midi_device, 2, 0x3E, 0x7F - (analogReadPin(SLIDER_PIN) >> 3));

    last_read = current_read;
    }

}

void housekeeping_task_user(void) {
    slider();
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {

    if (layer_state_is(2)) {
        rgb_matrix_set_color(11, 255, 255, 255);
        rgb_matrix_set_color(12, 255, 255, 255);
        rgb_matrix_set_color(13, 255, 255, 255);
    }

    if (host_keyboard_led_state().num_lock) {
        RGB_MATRIX_INDICATOR_SET_COLOR(0, 255, 255, 255); //capslock key
    }
    return false;
}
