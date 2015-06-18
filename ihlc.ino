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
#include <IndIOButtonPanel.h>
#include <SoftReset.h>
#include <UC1701.h>


static UC1701 lcd;
IndIOButtonPanel btns;
static const int rs485EnablePin = 9;
static const int backlightPin = 13;

int currentValue = 0;


void setup()
{
  // Setup the screen and panel buttons
  lcd.begin();
  pinMode(backlightPin, OUTPUT);
  analogWrite(backlightPin, 128);
  lcd.clear();
  lcd.println("IHLC Ready.");
  btns = IndIOButtonPanel();

  // Setup RS-485
  DMXSerial.init(DMXController);
  pinMode(rs485EnablePin, OUTPUT);
  digitalWrite(rs485EnablePin, HIGH);

  // Set all the channels on the IND.I/O Baseboard to Input
  for(int i=1; i<=8; i++) {
    Indio.digitalWrite(i, LOW);
    Indio.digitalMode(i, INPUT);
  }
}


void loop()
{
  handlePanelButtons();
  
  if (Indio.digitalRead(1)) {
    DMXSerial.write(6, 255);
  } else {
    DMXSerial.write(6, 0);
  }
}



void handlePanelButtons() {
  int currentButton = NONE_PRESSED;
  static int lastButton = NONE_PRESSED;

  currentButton = btns.readButtonPanel();
  if (currentButton != lastButton) {
    lastButton = currentButton;

    switch (currentButton){
      case ENTER_PRESSED:
        soft_restart();
        break;
      case UP_PRESSED:
        currentValue += 1;
        if (currentValue > 100)
          currentValue = 100;
        break;
      case DOWN_PRESSED:
        currentValue -= 1;
        if (currentValue < 0)
          currentValue = 0;
        break;
    }

    DMXSerial.write(1, map(currentValue, 0, 100, 0, 255));
    lcd.setCursor(0,2);
    lcd.print("Value: ");
    lcd.print(currentValue, DEC);
    lcd.print("%  ");

  }
}
