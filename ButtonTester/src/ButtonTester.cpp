/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "IoTClassroom_CNM.h"

const int REDSIGPIN = D10;
const int BLUSIGPIN = D6;
const int YLWSIGPIN = D5;
const int GRNSIGPIN = D4;

Button redButton(REDSIGPIN);
Button bluButton(BLUSIGPIN);
Button ylwButton(YLWSIGPIN);
Button grnButton(GRNSIGPIN);

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(SEMI_AUTOMATIC);

// setup() runs once, when the device is first turned on
void setup() {

  // set up serial monitor
  Serial.begin(9600);
  waitUntil(Serial.isConnected);
  delay(1000);
  Serial.printf("Ready to go!\n\n");
}

void loop() {

  if (redButton.isClicked()) {
    Serial.printf("red button clicked\n");
  }
  if (bluButton.isClicked()) {
    Serial.printf("blu button clicked\n");
  }
  if (ylwButton.isClicked()) {
    Serial.printf("rylw button clicked\n");
  }
  if (grnButton.isClicked()) {
    Serial.printf("grn button clicked\n");
  }


}
