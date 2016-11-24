/*
  PxButton.h - eases software button usage a bit
  Created by pX, November 16, 2016
  Released into the public domain.
*/
#ifndef PxButton_h
#define PxButton_h

class PxButton
{
	public:
		int buttonPin;
		int lastState;		
		unsigned long lastPushTime;
		unsigned long lastVirtualPushTime;
		
		PxButton(int pin);
		bool wasPushed();
		void setRepeatProperties(int beginDelayMs, int delayMs);
	
	private:
		int repeatBeginDelayMs = 500;
		int repeatDelayMs = 100;
};

#endif