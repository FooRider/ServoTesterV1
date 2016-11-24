#include "Arduino.h"
#include "PxButton.h"

PxButton::PxButton(int pin)
{
	pinMode(pin, INPUT_PULLUP);
	buttonPin = pin;
	lastState = digitalRead(buttonPin);
}

bool PxButton::wasPushed()
{
	int oldState = lastState;
	lastState = digitalRead(buttonPin);
	
	if (oldState == HIGH && lastState == LOW)
	{
		lastPushTime = millis();
		lastVirtualPushTime = lastPushTime + repeatBeginDelayMs;
		return true;
	}
	
	if (oldState == LOW && lastState == LOW && lastVirtualPushTime + repeatDelayMs < millis())
	{
		lastVirtualPushTime = millis();
		return true;
	}
	
	return false;
}

void PxButton::setRepeatProperties(int beginDelayMs, int delayMs)
{
	repeatBeginDelayMs = beginDelayMs;
	repeatDelayMs = delayMs;
}