#include "ihlc.h"

#include <UC1701.h>
#include <IndIOButtonPanel.h>


static UC1701 lcd;
IndIOButtonPanel btns;
static const int backlightPin = 13;

// A custom arrow glyph
static const unsigned char arrowGlyph[] = { 0x00, 0x7f, 0x3e, 0x1c, 0x08 };
static const unsigned char arrowDown[] = { 0x10, 0x20, 0x7e, 0x20, 0x10 };

// Pointer to the current menu function
static char currentMenuItem = 1;
menuFunc_t currentMenu = NULL;
static byte testChannel = 1;

// Menu action definitions
//const int NONE_PRESSED = 0;
//const int ENTER_PRESSED = 1;
//const int UP_PRESSED = 2;
//const int DOWN_PRESSED = 3;
const int MENU_SETUP = 8;
const int MENU_REFRESH = 16;


static void switchMenus(menuFunc_t func)
{
    lcd.clear();
    currentMenu = func;
    currentMenuItem = 1;
    currentMenu(MENU_SETUP);
}

static char scrollMenuItemCursor(char button, char max=6)
{
    switch (button) {
    case ENTER_PRESSED:
        return currentMenuItem;
    case UP_PRESSED:
        currentMenuItem -= 1;
        if (currentMenuItem < 1)
            currentMenuItem = 1;
        break;
    case DOWN_PRESSED:
        currentMenuItem += 1;
        if (currentMenuItem > max)
            currentMenuItem = max;
        break;
    }

    for (int i=1; i <= max; i++) {
        lcd.setCursor(0, i);
        lcd.print(' ');
    }
    lcd.setCursor(0, currentMenuItem); //set new cursor position
    lcd.write(0x01);  // Arrow cursor glyph

    return -1;
}

static void testChannelMenu(char action)
{
    byte value = getChannel(testChannel);

    switch (action) {
    case MENU_SETUP:
        lcd.print(F("Test brightness"));
        break;
    case ENTER_PRESSED:
        switchMenus(mainMenu);
        return;
    case UP_PRESSED:
        if (value >= 90)
            setChannel(testChannel, 100);
        else
            setChannel(testChannel, value + 10);
        break;
    case DOWN_PRESSED:
        if (value <= 10)
            setChannel(testChannel, 0);
        else
            setChannel(testChannel, value - 10);
        break;
    }

    lcd.setCursor(8*7, 3);
    lcd.print(value, DEC);
    lcd.print(F("%  "));
}

static void testChooseChannelMenu(char action)
{
    switch (action) {
    case MENU_SETUP:
        lcd.println(F("Choose a channel:"));
        break;
    case ENTER_PRESSED:
        switchMenus(testChannelMenu);
        return;
    case UP_PRESSED:
        testChannel++;
        if (testChannel > NUM_CHANNELS)
            testChannel = NUM_CHANNELS;
        break;
    case DOWN_PRESSED:
        testChannel--;
        if (testChannel < 1)
            testChannel = 1;
        break;
    }

    lcd.setCursor(8*7, 3);
    lcd.print(testChannel, DEC);
    lcd.print(F("  "));
}

static void testInputs(char action)
{
    switch(action) {
    case ENTER_PRESSED:
        switchMenus(mainMenu);
        break;
    case MENU_SETUP:
        lcd.println(F("Input State:"));
        lcd.println();
        lcd.println(F("  12345678"));
        lcd.print(F("  "));
        for(int pin=1; pin <= 8; pin++) {
            lcd.write(0x02);
        }
    // no break
    case MENU_REFRESH:
        int inputs = Indio.gpio_read();
        lcd.setCursor(8*2, 4);
        for(int pin=1; pin <= 8; pin++) {
            if (bitRead(inputs, pin*2-2)) {
                lcd.print('1');
            } else {
                lcd.print('0');
            }
        }
        break;
    }
}

static void testCycle(char action)
{
    static unsigned long currentChannel = 1;
    static unsigned long lastChange = 0;
    static char step = 1;

    switch(action) {
    case ENTER_PRESSED:
        switchMenus(mainMenu);
        break;
    }
    
    if (millis() - lastChange >= 600) {
        if (currentChannel >= NUM_CHANNELS) {
            step = -1;
        } else if (currentChannel <= 1) {
            step = 1;
        }
        
        lcd.setCursor(8*7, 3);
        lcd.print(currentChannel, DEC);
        lcd.print(F("  "));
        
        if (step == 1) {
            setChannel(currentChannel, 255);
        } else {
            setChannel(currentChannel, 0);
        }

        currentChannel += step;
        lastChange = millis();
    }
}

static void mainMenu(char action)
{
    if (action == MENU_SETUP) {
        lcd.println(F("IHLC Main Menu"));
        lcd.println(F(" All Off"));      // 1
        lcd.println(F(" All On"));       // 2
        lcd.println(F(" Test Channel")); // 3
        lcd.println(F(" Test Inputs"));  // 4
        lcd.println(F(" Test Cycle"));  // 5
        lcd.println(F(" Reset"));        // 6
    }

    char item = scrollMenuItemCursor(action, 6);
    switch (item) {
    case 1:
        setAllChannels(0);
        break;
    case 2:
        setAllChannels(100);
        break;
    case 3:
        switchMenus(testChooseChannelMenu);
        break;
    case 4:
        switchMenus(testInputs);
        break;
    case 5:
        switchMenus(testCycle);
        break;
    case 6:
        lcd.clear();
        soft_restart();
        break;
    }
}


void setupMenus()
{
    lcd.begin();
    lcd.createChar(0x01, arrowGlyph);
    lcd.createChar(0x02, arrowDown);

    pinMode(backlightPin, OUTPUT);
    analogWrite(backlightPin, 128);

    switchMenus(mainMenu);
}


void handleMenus()
{
    int currentButton = NONE_PRESSED;
    static int lastButton = NONE_PRESSED;
    static unsigned long lastRefresh = 0;

    currentButton = btns.readButtonPanel();
    if (currentButton != lastButton) {
        lastButton = currentButton;
        currentMenu(currentButton);
    } else {
        if (millis() - lastRefresh >= 200) {
            currentMenu(MENU_REFRESH);
            lastRefresh = millis();
        }
    }
}
