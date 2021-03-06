/**
 * Basic send example
 * Sends an sms to a given number
 **/

#include "A6SmsLib.h"

const String NUMBER = "00336XXXXXXXX";
const String MESSAGE = "whoop whoop!";

const int rx = D1;
const int tx = D2;
const int resetPin = D0;

A6SmsLib a6Sms(rx, tx, resetPin);

void setup()
{
    Serial.begin(9600);
    a6Sms.debug();
    a6Sms.begin();
    a6Sms.waitUntilReady();

    a6Sms.sms(NUMBER, MESSAGE);
}

void loop()
{
    a6Sms.loop();
}
