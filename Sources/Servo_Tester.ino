#include <LiquidCrystal.h>
#include <Servo.h>
#include <PxButton.h>

#define MODECOUNT 4;
#define MODE_CONST 0
#define MODE_TRIANGLE 1
#define MODE_SINE 2
#define MODE_SQUARE 3

const int minDegrees = 45;
const int maxDegrees = 135;

const int minPeriodMs = 200;
const int maxPeriodMs = 9900;
const int periodStepMs = 100;

const int servoPin = 14;
const int buttonAPin = 19;
const int buttonBPin = 18;
const int buttonCPin = 17;

int mode = 0;

LiquidCrystal lcd(12, 11, 10, 9, 8, 7);
Servo servo;
PxButton buttonA(buttonAPin);
PxButton buttonB(buttonBPin);
PxButton buttonC(buttonCPin);

void setup() 
{
  lcd.begin(16, 2);
  servo.attach(servoPin);
  pinMode(buttonAPin, INPUT_PULLUP);
  pinMode(buttonBPin, INPUT_PULLUP);
  pinMode(buttonCPin, INPUT_PULLUP);
  pinMode(13, OUTPUT);

  buttonA.setRepeatProperties(500, 100);
  buttonB.setRepeatProperties(500, 100);
  buttonC.setRepeatProperties(1500, 500);
  
  introScreen();
  initConstantMode();
}

void loop() 
{
  bool bA = buttonA.wasPushed();
  bool bB = buttonB.wasPushed();
  bool bC = buttonC.wasPushed();

  if (bC)
  {
    mode = (mode + 1) % MODECOUNT;

    switch (mode)
    {
      case MODE_CONST: 
        initConstantMode(); break;
      case MODE_TRIANGLE: 
        initTriangleMode(); break;
      case MODE_SINE: 
        initSineMode(); break;
      case MODE_SQUARE: 
        initSquareMode(); break;
    }
  }

  switch (mode)
    {
      case MODE_CONST: 
        runConstantMode(bA, bB); break;
      case MODE_TRIANGLE: 
        runTriangleMode(bA, bB); break;
      case MODE_SINE: 
        runSineMode(bA, bB); break;
      case MODE_SQUARE: 
        runSquareMode(bA, bB); break;
    }
  
  delay(10);
}

void introScreen()
{
  lcd.setCursor(2, 0);
  lcd.print("pX  hardware");
  lcd.setCursor(2, 1);
  lcd.print("Servo tester");
  delay(1000);
  lcd.clear();
}

int lastDegValue = -1;
void setOutput(int degValue)
{
  if (lastDegValue == degValue)
    return;
  
  lastDegValue = degValue;
  servo.write(degValue);

  int tmpVal = degValue;
  lcd.setCursor(8, 1);
  if (tmpVal >= 0)
    lcd.print(" ");
  else
    tmpVal = -tmpVal;

  if (tmpVal < 100)
    lcd.print(" ");

  if (tmpVal < 10)
    lcd.print(" ");

  lcd.print(degValue);
  lcd.print("deg");    
}

int constantMode_levelDeg = 90;

void initConstantMode()
{
  lcd.setCursor(0, 0);
  lcd.print(" Constant mode  ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lastDegValue = -1;
  setOutput(constantMode_levelDeg);
}

void runConstantMode(bool bA, bool bB)
{
  if (bA)
  {
    if (constantMode_levelDeg < maxDegrees)
    {
      constantMode_levelDeg++;
    }
  }
  else if (bB)
  {
    if (constantMode_levelDeg > minDegrees)
    {
      constantMode_levelDeg--;
    }
  }
  setOutput(constantMode_levelDeg);
}

int cyclicModes_periodMs = 2000;
long cyclicModes_startTimeMs = 0;

void printPeriod()
{
  int val = cyclicModes_periodMs / 100;
  int ds = val % 10;
  int s = val / 10;
  lcd.setCursor(1, 1);
  lcd.print(s);
  lcd.print(".");
  lcd.print(ds);
  lcd.print("s");
}

double modifyPeriodAndGetPhase(bool bA, bool bB)
{
  unsigned long curTime = millis();
  double phase = ((double)((curTime - cyclicModes_startTimeMs) % cyclicModes_periodMs)) / ((double)cyclicModes_periodMs);
  if (bA)
  {
    if (cyclicModes_periodMs + periodStepMs <= maxPeriodMs)
    {
      cyclicModes_periodMs += periodStepMs;
      cyclicModes_startTimeMs = curTime - ((long)(cyclicModes_periodMs * phase));
      printPeriod();
    }
  }
  else if (bB)
  {
    if (cyclicModes_periodMs - periodStepMs >= minPeriodMs)
    {
      cyclicModes_periodMs -= periodStepMs;
      cyclicModes_startTimeMs = curTime - ((long)(cyclicModes_periodMs * phase));      
      printPeriod();
    }
  }
  return phase;
}

void initTriangleMode()
{
  lcd.setCursor(0, 0);
  lcd.print(" Triangle mode  ");
  lcd.setCursor(0, 1);
  lcd.print("                ");

  printPeriod();  
  cyclicModes_startTimeMs = millis() - (cyclicModes_periodMs / 4);
}

void runTriangleMode(bool bA, bool bB)
{
  double phase = modifyPeriodAndGetPhase(bA, bB);

  phase *= 2;
  if (phase > 1)
    phase = 2 - phase;
  
  setOutput(minDegrees + phase * (maxDegrees - minDegrees));
  
  delay(10);
}

void initSineMode()
{
  lcd.setCursor(0, 0);
  lcd.print(" Sine mode      ");
  lcd.setCursor(0, 1);
  lcd.print("                ");

  printPeriod();  
  cyclicModes_startTimeMs = millis() - (cyclicModes_periodMs / 4);
}

void runSineMode(bool bA, bool bB)
{
  double phase = modifyPeriodAndGetPhase(bA, bB);

  phase = (sin(phase * 2 * 3.14159) + 1) / 2;
  
  setOutput(minDegrees + phase * (maxDegrees - minDegrees));
  
  delay(10);
}

void initSquareMode()
{
  lcd.setCursor(0, 0);
  lcd.print(" Square mode    ");
  lcd.setCursor(0, 1);
  lcd.print("                ");

  printPeriod();  
  cyclicModes_startTimeMs = millis() - (cyclicModes_periodMs / 4);
}

void runSquareMode(bool bA, bool bB)
{
  double phase = modifyPeriodAndGetPhase(bA, bB);

  if (phase > 0.5)
    phase = 1;
  else
    phase = 0;
  
  setOutput(minDegrees + phase * (maxDegrees - minDegrees));
  
  delay(10);
}
