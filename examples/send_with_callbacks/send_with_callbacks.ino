/**
 * Send example with callbacks
 * Sends an sms to a given number
 **/

#include "A6SmsLib.h"

const String NUMBER = "00336XXXXXXXX";
const String MESSAGE = "whoop whoop!";

const int rx = D1;
const int tx = D2;
const int resetPin = D0;

A6SmsLib a6Sms(rx, tx, resetPin);

void statusUpdate(String state, String message)
{
  Serial.println("[statusUpdate] " + state + " : " + message);
}

void smsUpdate(String state, String message)
{
    Serial.println("[smsUpdate] " + state + " : " + message);
}

void setup()
{
    Serial.begin(9600);
    a6Sms.debug();
    a6Sms.begin();
    a6Sms.setStatusCallback(statusUpdate);
    a6Sms.setSmsCallback(smsUpdate);
    a6Sms.waitUntilReady();

    a6Sms.sms(NUMBER, MESSAGE);
}

void loop()
{
    a6Sms.loop();
}
