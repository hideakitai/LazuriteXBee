#include "lazurite.h"
#define SUBGHZ_CH 36        // channel number (frequency)
#define SUBGHZ_PANID 0xABCD // panid
#define HOST_ADDRESS 0xFFFF // distination address

#define LED_B 26
unsigned char send_data[] = {"Welcome to Lazurite Sub-GHz\r\n"};

uint32_t baud_rate = 115200;
uint16_t sghz_channel = 36;
uint16_t sghz_pan_id = 0xABCD;
uint16_t sghz_host_addr = 0xFFFF;
uint16_t sghz_data_rate = SUBGHZ_100KBPS;
uint16_t sghz_power = SUBGHZ_PWR_20MW;

bool is_cmd_mode = false;
uint32_t last_cmd_time = 0;

enum FLASH_PARAM_ADDRESS
{
    FLASH_SERIAL_BAUD_RATE = 0,
    FLASH_SGHZ_CHANNEL,
    FLASH_SGHZ_PAN_ID,
    FLASH_SGHZ_HOST_ADDR,
    FLASH_SGHZ_DATA_RATE,
    FLASH_SGHZ_POWER,
    FLASH_SAVE_FLAG = 0x1FF
};

const uint16_t FLASH_SAVE_FLAG_VALUE = 0xABCD;

const uint32_t SERIAL_BAUD_LIST[5] = {9600, 19200, 38400, 57600, 115200};

static bool is_channel_correct(uint8_t ch, uint8_t rate)
{
    return
    (bool)(
        ((ch >= 24) && (ch <= 61) && (rate == SUBGHZ_50KBPS))
    ||  ((ch >= 24) && (ch <= 60) && (ch != 32) && (rate == SUBGHZ_100KBPS))
    );
}

static bool is_power_correct(uint8_t pwr)
{
    return (bool)((pwr == SUBGHZ_PWR_1MW) || (pwr == SUBGHZ_PWR_20MW));
}

static bool is_data_rate_correct(uint8_t data_rate)
{
    return (bool)((data_rate == SUBGHZ_50KBPS) || (data_rate == SUBGHZ_100KBPS));
}

void print_flash_param()
{
    Serial.print("addr 0 : ");
    Serial.println_long((long)Flash.read(0, FLASH_SERIAL_BAUD_RATE), DEC);
    Serial.print("addr 1 : ");
    Serial.println_long((long)Flash.read(0, FLASH_SGHZ_CHANNEL), DEC);
    Serial.print("addr 2 : ");
    Serial.println_long((long)Flash.read(0, FLASH_SGHZ_PAN_ID), DEC);
    Serial.print("addr 3 : ");
    Serial.println_long((long)Flash.read(0, FLASH_SGHZ_HOST_ADDR), DEC);
    Serial.print("addr 4 : ");
    Serial.println_long((long)Flash.read(0, FLASH_SGHZ_DATA_RATE), DEC);
    Serial.print("addr 5 : ");
    Serial.println_long((long)Flash.read(0, FLASH_SGHZ_POWER), DEC);
}

void write_flash_param()
{
    uint16_t baud_index = 0;
    uint16_t i = 0;
    for (baud_index = 0; baud_index < 5; ++baud_index)
    {
        if (baud_rate == SERIAL_BAUD_LIST[baud_index])
            break;
    }
    Flash.erase(0);
    Flash.write(0, FLASH_SERIAL_BAUD_RATE, baud_index);
    Flash.write(0, FLASH_SGHZ_CHANNEL, sghz_channel);
    Flash.write(0, FLASH_SGHZ_PAN_ID, sghz_pan_id);
    Flash.write(0, FLASH_SGHZ_HOST_ADDR, sghz_host_addr);
    Flash.write(0, FLASH_SGHZ_DATA_RATE, sghz_data_rate);
    Flash.write(0, FLASH_SGHZ_POWER, sghz_power);
    Flash.erase(1);
    Flash.write(1, FLASH_SAVE_FLAG, FLASH_SAVE_FLAG_VALUE);
}

void read_flash_param()
{
    if (Flash.read(1, FLASH_SAVE_FLAG) == FLASH_SAVE_FLAG_VALUE)
    {
        uint16_t baud_index = Flash.read(0, FLASH_SERIAL_BAUD_RATE);
        if (baud_index > 4) baud_index = 4;
        baud_rate = SERIAL_BAUD_LIST[baud_index];

        sghz_data_rate = Flash.read(0, FLASH_SGHZ_DATA_RATE);
        if (!is_data_rate_correct((uint8_t)sghz_data_rate))
            sghz_data_rate = SUBGHZ_100KBPS;

        sghz_channel = Flash.read(0, FLASH_SGHZ_CHANNEL);
        if (!is_channel_correct((uint8_t)sghz_channel, (uint8_t)sghz_data_rate))
            sghz_channel = 36;

        sghz_power = Flash.read(0, FLASH_SGHZ_POWER);
        if (!is_power_correct((uint8_t)sghz_power))
            sghz_power = SUBGHZ_PWR_20MW;

        sghz_pan_id = Flash.read(0, FLASH_SGHZ_PAN_ID);
        if (sghz_pan_id == 0xFFFF)
            sghz_pan_id = 0xABCD;

        sghz_host_addr = Flash.read(0, FLASH_SGHZ_HOST_ADDR);
    }
    else
    {
        write_flash_param();
    }

}
