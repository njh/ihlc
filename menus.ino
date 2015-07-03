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


static void menu1(char action)
{
    if (action == MENU_SETUP) {
        lcd.println(F("Sub Menu 1"));
        lcd.println(F(" Item 1"));
        lcd.println(F(" Item 2"));
        lcd.println(F(" Exit"));
    }

    char item = scrollMenuItemCursor(action, 3);
    switch (item) {
    case 1:
        lcd.clear();
        lcd.println(F("Selected 1"));
        break;
    case 2:
        lcd.clear();
        lcd.println(F("Selected 2"));
        break;
    case 3:
        switchMenus(mainMenu);
        break;
    }

}

static void testInputs(char action)
{
    switch(action) {
    case ENTER_PRESSED:
        switchMenus(mainMenu);
        break;
    case MENU_SETUP:
        lcd.clear();
        lcd.println("Input State:");
        lcd.println();
        lcd.println("  12345678");
        lcd.print("  ");
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


static void mainMenu(char action)
{
    if (action == MENU_SETUP) {
        lcd.clear();
        lcd.println(F("IHLC Main Menu"));
        lcd.println(F(" All Off"));     // 1
        lcd.println(F(" All On"));      // 2
        lcd.println(F(" Sub Menu 2"));  // 3
        lcd.println(F(" Test Inputs")); // 4
        lcd.println(F(" Reset"));       // 5
    }

    char item = scrollMenuItemCursor(action, 5);
    switch (item) {
    case 1:
        setAll(0);
        break;
    case 2:
        setAll(255);
        break;
    case 3:
        switchMenus(menu1);
        break;
    case 4:
        switchMenus(testInputs);
        break;
    case 5:
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

    currentButton = btns.readButtonPanel();
    if (currentButton != lastButton) {
        lastButton = currentButton;
        currentMenu(currentButton);
    } else {
        currentMenu(MENU_REFRESH);
    }
}
