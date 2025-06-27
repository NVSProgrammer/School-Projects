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
// AudioOut is not currently used for tone() or other audio functions
const int LockSignal = 12;

// Button
const int AButton = 2;
const int BButton = 3;

// delays [ms]
const unsigned long LEDDelay = 500;

// changing varribles-----------
// bool
bool Locked = false;

// ms track [unsigned long]
// Use unsigned long for delays(int can overflow after 32s, while unsigned long can keep up to 49 days)
unsigned long LastLEDMs = 0;

// int
int LED = ReadyFlag;
int LockSignalSended = 0;

bool LEDB = false;

// Button Functions (IRS)
void AButtonFunction(){ // DOC
  if(!Locked){
    Locked = true;
    LED = AButtonLED;
    Serial.print("A");
    digitalWrite(ALEDLine, HIGH);
  }
}
void BButtonFunction(){ // DOC
  if(!Locked){
    Locked = true;
    LED = BButtonLED;
    Serial.print("B");
    digitalWrite(BLEDLine, HIGH);
  }
}

// setup
void setup(){ // DOC
  // LED
  pinMode(ReadyFlag, OUTPUT);
  pinMode(AButtonLED, OUTPUT);
  pinMode(BButtonLED, OUTPUT);
  pinMode(ALEDLine, OUTPUT);
  pinMode(BLEDLine, OUTPUT);

  // Audio / Lock Signal
  // pinMode(AudioOut, OUTPUT); // Pin still needs to be configured if you plan to use it later
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
  Serial.begin(9600);

  // Set Ready Flag on
  digitalWrite(ReadyFlag, HIGH);
  Serial.print("R");
}

// loop
void loop(){ // DOC
  if(Locked){
    bool output = millis() - LastLEDMs >= LEDDelay;
    if(output){
      Serial.println("Blink");
      if(LEDB) {
        Serial.print("1");
        digitalWrite(LED, HIGH);
        LEDB = false;
      }
      else {
        Serial.print("0");
        digitalWrite(LED, LOW);
        LEDB = true;
      }
    }
    if(output) LastLEDMs = millis();
    if(LockSignalSended != 1) {
      Serial.println("Locked");
      digitalWrite(LockSignal, HIGH);
      Serial.println(digitalRead(LockSignal));
      delay(500);
      digitalWrite(LockSignal, LOW);
      Serial.println(digitalRead(LockSignal));
      LockSignalSended = 1;
    }
  }
}