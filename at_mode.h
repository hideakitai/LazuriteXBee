#include "lazurite.h"

// TODO: NO frame rate is better
// #define USE_FRAME_RATE
#ifdef USE_FRAME_RATE
const uint32_t sghz_send_interval_ms = 16;
uint32_t prev_time = 0;
#endif

uint8_t sghz_rx_data[256];
uint8_t sghz_tx_data[256];

const uint8_t sghz_payload_offset = 9;

static void at_mode()
{
    // TODO: serial buffer overflow ( > 128)
    // TODO: error message (msg != SUBGHZ_OK)

    // pass data through from Serial to SubGHz
    {
        SUBGHZ_MSG msg;
        static uint32_t cmd_mode_wait_start_ms = 0;
        static bool b_wait_for_cmd_mode = false;
        static uint8_t cmd_mode_cnt = 0;
        static uint8_t curr_serial_tx_count = 0;

        if (Serial.available())
        {

            while (Serial.available())
            {
                static const uint8_t cmd_mode_cnt_limit = 3;
                static const char cmd_mode_char = '+';
                // command mode check
                if (Serial.peek() == cmd_mode_char)
                {
                    if(++cmd_mode_cnt >= cmd_mode_cnt_limit)
                    {
                        b_wait_for_cmd_mode = true;
                        cmd_mode_wait_start_ms = millis();
                    }
                }
                else
                {
                    b_wait_for_cmd_mode = false;
                    cmd_mode_cnt = 0;
                    // cmd_mode_wait_start_ms = millis();
                }
                sghz_tx_data[curr_serial_tx_count++] = (uint8_t)Serial.read();
                // delayMicroseconds(100);
            }

#ifndef USE_FRAME_RATE
            msg = SubGHz.send(sghz_pan_id, sghz_host_addr, sghz_tx_data, curr_serial_tx_count, NULL);
            if (msg != SUBGHZ_OK) SubGHz.msgOut(msg);
            memset(sghz_tx_data, 0, sizeof(sghz_tx_data));
            curr_serial_tx_count = 0;
#endif
            last_cmd_time = millis();
        }
        else if (b_wait_for_cmd_mode)
        {
            static const uint32_t cmd_mode_wait_end_ms = 3000;
            if (millis() - cmd_mode_wait_start_ms > cmd_mode_wait_end_ms)
            {
                Serial.println("enter to at cmd mode");
                is_cmd_mode = true;
                b_wait_for_cmd_mode = false;
                cmd_mode_cnt = 0;
            }
        }


#ifdef USE_FRAME_RATE
        if (millis() - prev_time > sghz_send_interval_ms)
        {
            if (curr_serial_tx_count)
            {
                msg = SubGHz.send(sghz_pan_id, sghz_host_addr, sghz_tx_data, curr_serial_tx_count, NULL);
                if (msg != SUBGHZ_OK) SubGHz.msgOut(msg);
                memset(sghz_tx_data, 0, sizeof(sghz_tx_data));
                curr_serial_tx_count = 0;
            }
        }
#endif
    }

    // pass data through from SubGHz to Serial
    {
        uint16_t sghz_rx_size = SubGHz.readData(sghz_rx_data, sizeof(sghz_rx_data));
        if(sghz_rx_size)
        {
            digitalWrite(LED_B, LOW);
            Serial.write(sghz_rx_data + sghz_payload_offset, sghz_rx_size - sghz_payload_offset);
            // Serial.println_long((long)sghz_rx_size, DEC);
            digitalWrite(LED_B, HIGH);
        }
    }
}
