/* Industruino Home Lighting Controller

  Requires the Industruino IND.I/O kit:
  * https://industruino.com/shop/product/industruino-ind-i-o-kit-2

  And the following libraries:
  * https://github.com/mathertel/DmxSerial
  * https://github.com/Industruino/Indio
  * https://github.com/summitsystemsinc/IndIOButtonPanel
  * https://github.com/WickedDevice/SoftReset
  * https://github.com/Industruino/UC1701

*/

#include <Wire.h>

#include <DMXSerial.h>
#include <Indio.h>
#include <SoftReset.h>


#include "ihlc.h"
#include "menus.h"


static const int rs485EnablePin = 9;
static byte targetDmxValues[NUM_CHANNELS];


void setup()
{
    // Setup the screen and panel buttons
    setupMenus();

    // Setup RS-485
    pinMode(rs485EnablePin, OUTPUT);
    digitalWrite(rs485EnablePin, HIGH);
    DMXSerial.init(DMXController);

    // Set target DMX values to 0
    for (int i=1; i<NUM_CHANNELS; i++) {
        targetDmxValues[i] = 0;
    }

    // Set all the channels on the IND.I/O Baseboard to Input
    for(int i=1; i<=8; i++) {
        Indio.digitalWrite(i, LOW);
        Indio.digitalMode(i, INPUT);
    }
}


void loop()
{
    handleMenus();
    performFades();
}


void performFades()
{
    static unsigned long lastFade = 0;
    unsigned long now = micros();

    if ((now - lastFade) > FADE_DELAY_US) {

        for(int i=1; i <= NUM_CHANNELS; i++) {
            byte current = DMXSerial.read(i);
            byte target = map(targetDmxValues[i], 0, 100, 0, 255);
            if (current < target) {
                DMXSerial.write(i, current + 1);
            } else if (current > target) {
                DMXSerial.write(i, current - 1);
            }
        }

        lastFade = now;
    }
}

void setChannel(int channel, uint8_t value)
{
    targetDmxValues[channel] = value;
}

uint8_t getChannel(int channel)
{
    return targetDmxValues[channel];
}

void setAllChannels(uint8_t value)
{
    for(int i=1; i <= NUM_CHANNELS; i++) {
        setChannel(i, value);
    }
}
