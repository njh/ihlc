#include "ihlc.h"

#include <UC1701.h>
#include <IndIOButtonPanel.h>


static UC1701 lcd;
IndIOButtonPanel btns;
static const int backlightPin = 13;

// A custom arrow glyph
static const unsigned char arrowGlyph[] = { 0x00, 0x7f, 0x3e, 0x1c, 0x08 };

// Pointer to the current menu function
static char currentMenuItem = 1;
menuFunc_t currentMenu = NULL;


static void switchMenus(menuFunc_t func)
{
    lcd.clear();
    currentMenu = func;
    currentMenuItem = 1;
    currentMenu(NONE_PRESSED);
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


static void menu1(char button)
{
    if (button == NONE_PRESSED) {
      lcd.println(F("Sub Menu 1"));
      lcd.println(F(" Item 1"));
      lcd.println(F(" Item 2"));
      lcd.println(F(" Exit"));
    }

    char item = scrollMenuItemCursor(button, 3);
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


static void mainMenu(char button)
{
    if (button == NONE_PRESSED) {
        lcd.clear();
        lcd.println(F("IHLC Main Menu"));
        lcd.println(F(" All Off"));     // 1
        lcd.println(F(" All On"));      // 2
        lcd.println(F(" Sub Menu 2"));  // 3
        lcd.println(F(" Test Inputs")); // 4
        lcd.println(F(" Reset"));       // 5
    }

    char item = scrollMenuItemCursor(button, 5);
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
  lcd.createChar(1, arrowGlyph);

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
        if (currentMenu && currentButton != NONE_PRESSED) {
            currentMenu(currentButton);
        }
    }
}
