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

#include <DMXSerial.h>
#include <Indio.h>
#include <SoftReset.h>


#include "ihlc.h"
#include "menus.h"


static const int rs485EnablePin = 9;
static byte targetDmxValues[NUM_CHANNELS];

static char sceneData[][NUM_CHANNELS] = {
    { 0,     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },   // 0: All Off
    { 100,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1 },   // 1
    { 50,   -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1 },   // 2
    { 0,    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1 },   // 3
    { -1,   -1,  -1,  -1,  -1, 100,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1 },   // 4
    { -1,   -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1 },   // 5
    { -1,   -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1 },   // 6
    { -1,   -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1 },   // 7
    { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 },   // 8: All On
};

static const byte numScenes = sizeof(sceneData);


void setup()
{
    // Setup the screen and panel buttons
    setupMenus();

    // Setup RS-485 / DMX
    pinMode(rs485EnablePin, OUTPUT);
    digitalWrite(rs485EnablePin, HIGH);
    DMXSerial.init(DMXController);

    // Switch off all channels
    setAllChannels(0);

    // Set all the channels on the IND.I/O Baseboard to Input
    for(int i=1; i<=8; i++) {
        Indio.digitalWrite(i, LOW);
        Indio.digitalMode(i, INPUT);
    }
}


void loop()
{
    handleMenus();
    handleInputs();
    performFades();
}


void performFades()
{
    static unsigned long lastFade = 0;
    unsigned long now = micros();

    if ((now - lastFade) > FADE_DELAY_US) {

        for(int i=0; i < NUM_CHANNELS; i++) {
            byte current = DMXSerial.read(i+1);
            byte target = map(targetDmxValues[i], 0, 100, 0, 255);
            if (current < target) {
                DMXSerial.write(i+1, current + 1);
            } else if (current > target) {
                DMXSerial.write(i+1, current - 1);
            }
        }

        lastFade = now;
    }
}

void setScene(uint8_t scene)
{
    // First check if the scene has already been set
    bool switchOff = true;
    for(int i=1; i <= NUM_CHANNELS; i++) {
        char value = sceneData[scene][i-1];
        if (value >= 0) {
            if (value != getChannel(i)) {
                switchOff = false;
                break;
            }
        }
    }

    for(int i=1; i <= NUM_CHANNELS; i++) {
        char value = sceneData[scene][i-1];
        if (value >= 0) {
            if (switchOff) {
                setChannel(i, 0);
            } else {
                setChannel(i, value);
            }
        }
    }
}

void setChannel(int channel, uint8_t value)
{
    targetDmxValues[channel-1] = value;
}

uint8_t getChannel(int channel)
{
    return targetDmxValues[channel-1];
}

void setAllChannels(uint8_t value)
{
    for(int i=0; i < NUM_CHANNELS; i++) {
        targetDmxValues[i] = value;
    }
}


void handleInputs()
{
    int pressed = readInputs();

    if (pressed) {
        setScene(pressed);
    }
}

byte firstSetBit(int value)
{
    for(byte pin=1; pin <= 8; pin++) {
        if (bitRead(value, pin*2-2)) {
            return pin;
        }
    }
    return 0;
}


static const unsigned long debounceDelay = 100;

byte readInputs()
{
    static byte lastPressed = 0;
    static int lastReadState = 0;
    static unsigned int lastDebounceTime = 0;
    byte result = 0;

    // Read the current input state
    int reading = Indio.gpio_read();

    // Reset the debounce if the last read state has changed.
    if (reading != lastReadState) {
        lastDebounceTime = millis();
    }
    lastReadState = reading;

    if ((millis() - lastDebounceTime) > debounceDelay) {
        byte pressed = firstSetBit(reading);
        if (pressed != lastPressed) {
            result = pressed;
        }
        lastPressed = pressed;
    }

    return result;
}
