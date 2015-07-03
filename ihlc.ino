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

int currentValue = 0;


void setup()
{
  // Setup the screen and panel buttons
  setupMenus();

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
  handleMenus();

  if (Indio.digitalRead(1)) {
    DMXSerial.write(6, 255);
  } else {
    DMXSerial.write(6, 0);
  }
}

void setAll(uint8_t value)
{
    for(int i=1; i <= NUM_CHANNELS; i++) {
        DMXSerial.write(i, value);
    }
}
