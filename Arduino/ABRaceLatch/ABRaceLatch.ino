/*
  ABRaceLatch
*/

// LED
const int ReadyFlag = 7;
const int AButtonLED = 4;
const int BButtonLED = 5;
const int ALEDLine = 8;
const int BLEDLine = 9;

// Audio / Lock Signal
const int AudioOut = 11;
const int LockSignal = 12;

// Button
const int AButton = 2;
const int BButton = 3;

// delays [ms]
const unsigned long ErrorFlagDelay = 300;
const unsigned long LEDDelay = 500;
const unsigned long LockSignalDelay = 200;

// Music
const int ToneArray[] = {500, 700};
const unsigned long ToneDelayArray[] = {100, 100};

// changing varribles
// bool
bool error = false;
bool Locked = false;
bool SoundPlayed = false;

// ms track [unsigned long]
// Use unsigned long for deales(int can overflow after 32s, while unsigned long can keep up to 49 days)
unsigned long LastErrorFlagMs = 0;
unsigned long LastTonePlsyedMs = 0;
unsigned long LastLEDMs = 0;
unsigned long LastLockSignal = 0;

// int
int ToneArrayIndex = 0;
int ToneArraySize = 0;
int LED = ReadyFlag;
int LockSignalSended = 0;

// setup
void setup(){
  // LED
  pinMode(ReadyFlag, OUTPUT);
  pinMode(AButtonLED, OUTPUT);
  pinMode(BButtonLED, OUTPUT);
  pinMode(ALEDLine, OUTPUT);
  pinMode(BLEDLine, OUTPUT);

  // Audio / Lock Signal
  pinMode(AudioOut, OUTPUT);
  pinMode(LockSignal, OUTPUT);

  /* Button
    DO NOT USE INPUT_PULLUP, becuase:
     - It is buggy with interrupts trigger my FALLING edge
     - The circuit already have pull-up resistor  
  */
  pinMode(AButton, INPUT);
  pinMode(BButton, INPUT);
  // Interrupt
  attachInterrupt(digitalPinToInterrupt(AButton), AButtonFunction, FALLING);
  attachInterrupt(digitalPinToInterrupt(BButton), BButtonFunction, FALLING);

  // Uncomment the command starting with Serial
  // Serial.begin(9600);

  // check if lists match
  ToneArraySize = sizeof(ToneArray) / sizeof(ToneArray[0]);
  if(ToneArraySize != sizeof(ToneDelayArray) / sizeof(ToneDelayArray[0])) error = true;

  // Set Ready Flag on
  digitalWrite(ReadyFlag, HIGH);
}

// loop
void loop(){
  /*
  if (Serial.available() > 0) {
    char receivedChar = Serial.read(); // Read the incoming character

    if (receivedChar == 'r' || receivedChar == 'R') Reset();
  }
  */
  if(error) Error();
  else if(Locked){
    if(!SoundPlayed) { SoundPlay(); } else noTone(AudioOut);
    LEDToggle(LED, LastLEDMs, LEDDelay);
    if(LockSignalSended < 2) {
      LEDToggle(LockSignal, LastLockSignal, LockSignalDelay);
      LockSignalSended ++;
    }
  }
}



// Soft Reset / Not imporatant
void Reset(){
  digitalWrite(ReadyFlag, LOW);
  digitalWrite(LockSignal, LOW);
  digitalWrite(AButtonLED, LOW);
  digitalWrite(BButtonLED, LOW);
  digitalWrite(ALEDLine, LOW);
  digitalWrite(BLEDLine, LOW);
  error = false;
  Locked = false;
  SoundPlayed = false;
  LockSignalSended = 0;
  ToneArrayIndex = 0;
  ToneArraySize = 0;
  LED = ReadyFlag;
  ToneArraySize = sizeof(ToneArray) / sizeof(ToneArray[0]);
  if(ToneArraySize != sizeof(ToneDelayArray) / sizeof(ToneDelayArray[0])) error = true;
  // Serial.print("R");
  digitalWrite(ReadyFlag, HIGH);
}

// Error function
void Error(){
  // Serial.print("E");
  LEDToggle(ReadyFlag, LastErrorFlagMs, ErrorFlagDelay);
}

// Button Functions (IRS)
void AButtonFunction(){
  if(!Locked){
    Locked = true;
    LED = AButtonLED;
    // Serial.print("A");
    digitalWrite(ALEDLine, HIGH);
  }
}
void BButtonFunction(){
  if(!Locked){
    Locked = true;
    LED = BButtonLED;
    // Serial.print("B");
    digitalWrite(BLEDLine, HIGH);
  }
}

// Help functions
void SoundPlay(){
  if(TimeOut(LastTonePlsyedMs, ToneDelayArray[ToneArrayIndex])){
    if (ToneArrayIndex >= ToneArraySize - 1){ SoundPlayed = true; return; }
    tone(AudioOut, ToneArray[ToneArrayIndex]);
    ToneArrayIndex ++;
  }
}

void LEDToggle(int pin, unsigned long& LastMs, unsigned long Ms){
  if(TimeOut(LastMs, Ms)){
    if(digitalRead(pin) == HIGH) digitalWrite(pin, LOW);
    else digitalWrite(pin, HIGH);
  }
}

bool TimeOut(unsigned long& LastMs, unsigned long ms){ // & is use to reference tha original value, not make a copy of it
  bool output = (millis() - LastMs >= ms) || LastMs == 0;
  if(output) LastMs = millis();
  return output;
}
