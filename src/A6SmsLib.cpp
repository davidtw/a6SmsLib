#include "Arduino.h"
#include "A6SmsLib.h"
#include <SoftwareSerial.h>

A6SmsLib::A6SmsLib(int rx,int tx, int reset, int nbMaxAttempts) : _a6Serial(SoftwareSerial(rx,tx))
{
  _rx = rx;
  _tx = tx;
  _reset = reset;
  _nbMaxAttempts = nbMaxAttempts;
  _state = "initing";
  _allowDebugMessages = false;
}

void A6SmsLib::debug(bool allowDebugMessages)
{
  _allowDebugMessages = allowDebugMessages;
}

void A6SmsLib::debug(String message)
{
  if(_allowDebugMessages) {
    Serial.println("A6 - " + message);
  }
}

void A6SmsLib::begin(int baudrate)
{
  debug("Ab - audrate : " + baudrate);
  _a6Serial.begin(baudrate);
  delay(500);
  if(_reset > -1) {
    pinMode(_reset, OUTPUT); 
    resetModule();
  }
}

void A6SmsLib::resetModule()
{
  if(_reset > -1) {
    debug("resetModule");
    digitalWrite(_reset, HIGH);
    delay(200);
    digitalWrite(_reset, LOW); 
  }
}

void A6SmsLib::requestStatus() 
{
  if(isReady()) {
    delay(2000);
    _state = "requesting_status";
    _a6Serial.println("AT");  
  }
}

void A6SmsLib::sms(String number, String message) {
  if(isReady()) {
    _state = "sending_sms";
    _number = number;
    _message = message;
    _a6Serial.print("AT+CMGS=\"");
    _a6Serial.print(number);
    _a6Serial.write(0x22);
    _a6Serial.write(0x0D);  // hex equivalent of Carraige return    
    _a6Serial.write(0x0A);  // hex equivalent of newline
    delay(2000);
    _a6Serial.print(message);
    delay(500);
    _a6Serial.println(char(26));//the ASCII code of the ctrl+z is 26
  }
}

bool A6SmsLib::isReady() {
  return _state == "ready";
}

void A6SmsLib::resetState()
{
  _nbAtempts = 0;
  _number = "";
  _message = "";
  _state = "ready";
}

void A6SmsLib::waitUntilReady()
{
  debug("Waiting for A6");
  debug("state : " + _state);
  unsigned long startTime = millis();
  while(_state == "initing" || _state == "sms_mode") {    
    if (_a6Serial.available()) {
      String recievedA6 = _a6Serial.readString();
      debug(recievedA6);
      if(_state == "initing" && recievedA6.indexOf("+CREG: 5") >= 0) {
        debug("setting sms mode");
        _state = "sms_mode";
        delay(2000);
        _a6Serial.println("AT+CMGF=1");  
      } else if(_state == "sms_mode" && recievedA6.indexOf("OK") >= 0) {
        _state = "ready";
        debug("A6 Ready!");
      }
    }
    yield();
    unsigned long now = millis();
    if(now - startTime  > 30000) {      
      startTime = now;
      resetModule();
    }
  }
}

void A6SmsLib::setStatusCallback(void (*function)(String status, String message)) {
  _statusCallback = function;
}

void A6SmsLib::setSmsCallback(void (*function)(String status, String message)) {
  _smsCallback = function;
}

void A6SmsLib::loop() { 
  if (_a6Serial.available()) {
    String recievedA6 = _a6Serial.readString();
    debug(recievedA6);
    if(_state == "initing" && recievedA6.indexOf("+CREG: 5") >= 0) {
      _state = "ready";
    }
    if(_state == "sending_sms") {
      if(recievedA6.indexOf("OK") >= 0) {
        _smsCallback(_state, "OK");
        resetState();
      } else if(recievedA6.indexOf("ERROR") >= 0) {
        if(_nbAtempts < _nbMaxAttempts) {
          _nbAtempts++;
          debug("Resending sms");
          sms(_number, _message);
        } else {
          resetState();
          _smsCallback(_state, "ERROR");
        }
      }
    } else if(_state == "requesting_status") {
      if(recievedA6.indexOf("OK") >= 0) {
        _statusCallback(_state, "OK");
        resetState();
      } else if(recievedA6.indexOf("ERROR") >= 0) {
        _statusCallback(_state, "ERROR");
        resetState();
      }
    }
  }
}