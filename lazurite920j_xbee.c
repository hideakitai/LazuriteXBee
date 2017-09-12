#include "lazurite920j_xbee_ide.h" // Additional Header
#include "global.h"
#include "at_mode.h"
#include "at_cmds.h"

void setup(void)
{
    SUBGHZ_MSG msg;
    
	delay(1000);
	
    read_flash_param();
    Serial.begin(baud_rate);

    msg = SubGHz.init();
    if (msg != SUBGHZ_OK)
    {
        SubGHz.msgOut(msg);
        while (1)
        {
            Serial.println("lazurite initialization failed...");
            delay(1000);
        }
    }

    msg = SubGHz.begin((uint8_t)sghz_channel, sghz_pan_id, sghz_data_rate, sghz_power);

    if (msg != SUBGHZ_OK)
    {
        SubGHz.msgOut(msg);
        while (1)
        {
            Serial.println("lazurite wireless start failed...");
            delay(1000);
        }
    }
    msg = SubGHz.rxEnable(NULL);
    if (msg != SUBGHZ_OK)
    {
        SubGHz.msgOut(msg);
        while (1)
        {
            Serial.println("lazurite wireless receiver enable failed...");
            delay(1000);
        }
    }

    pinMode(LED_B, OUTPUT);
    digitalWrite(LED_B, HIGH);

    return;
}

void loop(void)
{
    if (is_cmd_mode)
        at_cmd_mode();
    else
        at_mode();
}
