#ifndef A6SmsLib_h
#define A6SmsLib_h
#include "Arduino.h"
#include <SoftwareSerial.h>

class A6SmsLib
{
  public:
    A6SmsLib(int rx, int tx, int reset = -1, int nbMaxAttempts = 2);
    void debug(bool allowDebugMessages = true);    
    void begin(int baudrate = 115200);    
    void sms(String number, String message);
    void sms(String number, String message, void(*callback)(void));
    void requestStatus();
    void loop();
    void resetModule();
    bool isReady();
    void waitUntilReady();
    void setStatusCallback(void (*statusCallback)(String status, String message));
    void setSmsCallback(void (*smsCallback)(String status, String message));

  private:
    void(*_onSmsStatusUpdated)(int state);
    int _rx;
    int _tx;
    int _reset;
    int _nbMaxAttempts;
    int _nbAtempts;
    String _state;
    String _number;
    String _message;
    bool _allowDebugMessages;
    
    SoftwareSerial _a6Serial;
    void resetState();
    void debug(String message);
    void (*_statusCallback)(String status, String message);
    void (*_smsCallback)(String status, String message);
};


#endif