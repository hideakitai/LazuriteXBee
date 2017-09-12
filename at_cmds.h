#include "lazurite.h"
#include <ctype.h>
// #include <stdio.h>
// #include <string.h>
// #include <conio.h>

// TODO: use these methods
// static bool is_channel_correct(uint8_t ch, uint8_t rate)
// {
//     return
//     (
//         ((ch >= 24) && (ch <= 61) && (rate == SUBGHZ_50KBPS))
//     ||  ((ch >= 24) && (ch <= 60) && (ch != 32) && (rate == SUBGHZ_100KBPS))
//     );
// }

// static bool is_power_correct(uint8_t pwr)
// {
//     return ((pwr == SUBGHZ_PWR_1MW) || (pwr == SUBGHZ_PWR_20MW));
// }

// static bool is_data_rate_correct(uint8_t data_rate)
// {
//     return ((data_rate == SUBGHZ_50KBPS) || (data_rate == SUBGHZ_100KBPS));
// }



// TODO: terminate if buffer pointer excess serial buffer size
static void at_cmd_mode()
{
    uint16_t serial_rx_size = Serial.available();
    static const uint32_t last_cmd_time_limit = 10000;

    if (serial_rx_size)
    {
        static uint8_t serial_rx_data[128];
        static uint8_t curr_buf_ptr = 0;
        uint16_t i = 0;
        for (i = 0; i < serial_rx_size; ++i, ++curr_buf_ptr)
            serial_rx_data[curr_buf_ptr] = (uint8_t)Serial.read();

        // Serial.print("current count : ");
        // Serial.println_long(curr_buf_ptr, DEC);
        // Serial.print("current char  : ");
        // Serial.println(serial_rx_data);

        if (serial_rx_data[curr_buf_ptr - 1] == '\n')
        {
            char *token_ptr = NULL;
            char *cmd_ptr = NULL;
            char* tmp_ptr = NULL;
            size_t cmd_size = 0;

            serial_rx_data[curr_buf_ptr] = NULL;
            curr_buf_ptr = 0;

            token_ptr = cmd_ptr = strtok(serial_rx_data, " \r\n");
            cmd_size = strlen(cmd_ptr);
            for (tmp_ptr = cmd_ptr; *tmp_ptr != NULL; ++tmp_ptr)
                *tmp_ptr = (char)toupper(*tmp_ptr);

            token_ptr = strtok(NULL, " \r\n");


            // checker_func;
            // pointer_to_variable

            if (strncmp(cmd_ptr, "AT", cmd_size) == 0)
            {
                Serial.println("OK");
            }
            else if (strncmp(cmd_ptr, "ATBD", cmd_size) == 0)
            {
                if (token_ptr == NULL)
                {
                    Serial.print("Serial Baud Rate Index : ");
                    Serial.println_long((long)Flash.read(0, FLASH_SERIAL_BAUD_RATE), DEC);
                }
                else
                {
                    char *endptr;
                    uint32_t baud_index = (uint32_t)strtol(token_ptr, &endptr, 10);
                    if (*endptr != NULL)
                        Serial.println("invalid token value");
                    else if (baud_index < 5)
                    {
                        baud_rate = SERIAL_BAUD_LIST[baud_index];
                        Serial.print("New Serial Baud Rate : ");
                        Serial.println_long((long)baud_rate, DEC);
                    }
                    else
                    {
                        Serial.println("excess baud rate index");
                    }
                }
            }
            else if (strncmp(cmd_ptr, "ATCH", cmd_size) == 0)
            {
                if (token_ptr == NULL)
                {
                    Serial.print("SUBGHZ Channel : ");
                    Serial.println_long((long)Flash.read(0, FLASH_SGHZ_CHANNEL), DEC);
                }
                else
                {
                    char *endptr;
                    uint32_t ch = (uint32_t)strtol(token_ptr, &endptr, 10);
                    if (*endptr != NULL)
                        Serial.println("invalid token value");
                    // TODO: chan to yaru
                    else if ((ch > 23) && (ch < 62))
                    {
                        sghz_channel = (uint16_t)ch;
                        Serial.print("New SGHZ Channel : ");
                        Serial.println_long((long)sghz_channel, DEC);
                    }
                    else
                    {
                        Serial.print("invalid channel ");
                        Serial.println_long((long)ch, DEC);
                    }
                }
            }
            else if (strncmp(cmd_ptr, "ATID", cmd_size) == 0)
            {
                if (token_ptr == NULL)
                {
                    Serial.print("PAN ID : ");
                    Serial.println_long((long)Flash.read(0, FLASH_SGHZ_PAN_ID), HEX);
                }
                else
                {
                    char *endptr;
                    uint32_t pan_id = (uint32_t)strtol(token_ptr, &endptr, 16);
                    if (*endptr != NULL)
                        Serial.println("invalid token value");
                    else if (pan_id <= 0xFFFF)
                    {
                        sghz_pan_id = (uint16_t)pan_id;
                        Serial.print("New PAN ID : ");
                        Serial.println_long((long)sghz_pan_id, HEX);
                    }
                    else
                    {
                        Serial.print("invalid value ");
                        Serial.println_long((long)pan_id, HEX);
                    }
                }
            }
            else if (strncmp(cmd_ptr, "ATDT", cmd_size) == 0)
            {
                if (token_ptr == NULL)
                {
                    // TODO: HOST_ADDDR -> DEST_ADDR
                    Serial.print("Destination Address : ");
                    Serial.println_long((long)Flash.read(0, FLASH_SGHZ_HOST_ADDR), HEX);
                }
                else
                {
                    char *endptr;
                    uint32_t addr = (uint32_t)strtol(token_ptr, &endptr, 16);
                    if (*endptr != NULL)
                        Serial.println("invalid token value");
                    else if (addr <= 0xFFFF)
                    {
                        sghz_host_addr = (uint16_t)addr;
                        Serial.print("New SGHZ Destination Addr : ");
                        Serial.println_long((long)sghz_host_addr, HEX);
                    }
                    else
                    {
                        Serial.print("invalid value ");
                        Serial.println_long((long)addr, HEX);
                    }
                }
            }
            else if (strncmp(cmd_ptr, "ATDR", cmd_size) == 0)
            {
                if (token_ptr == NULL)
                {
                    Serial.print("SUBGHZ Data Rate : ");
                    Serial.println_long((long)Flash.read(0, FLASH_SGHZ_DATA_RATE), DEC);
                }
                else
                {
                    char *endptr;
                    uint32_t dr = (uint32_t)strtol(token_ptr, &endptr, 10);
                    if (*endptr != NULL)
                        Serial.println("invalid token value");
                    // TODO: chan to yaru
                    else if ((dr == SUBGHZ_50KBPS) || (dr == SUBGHZ_100KBPS))
                    {
                        sghz_data_rate = (uint16_t)dr;
                        Serial.print("New SGHZ Data Rate : ");
                        Serial.println_long((long)sghz_data_rate, DEC);
                    }
                    else
                    {
                        Serial.print("invalid value ");
                        Serial.println_long((long)dr, DEC);
                    }
                }
            }
            // TODO: _POWER -> _POWER_LEVEL
            else if (strncmp(cmd_ptr, "ATPL", cmd_size) == 0)
            {
                if (token_ptr == NULL)
                {
                    Serial.print("SUBGHZ Power Level : ");
                    Serial.println_long((long)Flash.read(0, FLASH_SGHZ_POWER), DEC);
                }
                else
                {
                    char *endptr;
                    uint32_t ch = (uint32_t)strtol(token_ptr, &endptr, 10);
                    if (*endptr != NULL)
                        Serial.println("invalid token value");
                    else if ((ch == SUBGHZ_PWR_1MW) || (ch == SUBGHZ_PWR_20MW))
                    {
                        sghz_power = (uint16_t)ch;
                        Serial.print("New SGHZ Power Level : ");
                        Serial.println_long((long)sghz_power, DEC);
                    }
                    else
                    {
                        Serial.print("invalid value ");
                        Serial.println_long((long)ch, DEC);
                    }
                }
            }
            else if (strncmp(cmd_ptr, "ATCN", cmd_size) == 0)
            {
                Serial.println("command mode finished");
                is_cmd_mode = false;
            }
            else if (strncmp(cmd_ptr, "ATWR", cmd_size) == 0)
            {
                Serial.println("write values to Flash");
                write_flash_param();
            }
            else if (strncmp(cmd_ptr, "ATRE", cmd_size) == 0)
            {
                Serial.println("reset module...");
                /* Set ELEVEL 2 */
                __asm("mov r0,psw\n or r0,#2\n mov psw,r0\n");
                /* Software Reset */
                __asm("brk");
            }
            else
            {
                Serial.print("commands undefined : ");
                Serial.println(cmd_ptr);
            }
            memset(serial_rx_data, 0, sizeof(serial_rx_data));
        }
        last_cmd_time = millis();
    }
    else
    {
        if (millis() - last_cmd_time > last_cmd_time_limit)
        {
            Serial.println("command mode finished");
            is_cmd_mode = false;
        }
    }
}
