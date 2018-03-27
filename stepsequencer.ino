#include <EEPROM.h>
//This was lab 7's midi step sequencer with EEPROM memory
int buttonPin[6] = {7, 8, 9, 10, 11, 12};
int ledPin[4] = {2, 3, 4, 5};
boolean lastButtonState[6] = {LOW, LOW, LOW, LOW, LOW};
boolean buttonState[6] = {LOW, LOW, LOW, LOW, LOW};
boolean stepState [3][4] = {
  {false, true, false, false},
  {true, false, true, false},
  {true, true, true, true}
};

int midiNote[4] = {42, 43, 44};

int tempo = 0;

int currentStep = 0;
int currentChannel = 0;
unsigned long lastStepTime = 0;


void setup() {
  for (int i = 0; i < 6; i++) {
    pinMode(buttonPin[i], INPUT);
  }
  for (int i = 0; i < 4; i++) {
    pinMode(ledPin[i], OUTPUT);
  }
  loadFromEEPROM();
}

void loop() {
  for (int i = 0; i < 4; i++) {
    checkButtons(i);
    setLeds(i);
  }
  for (int i = 4; i < 6; i++) {
    updateChannel(i);
  }

  sequence();
  while (usbMIDI.read()) {
  }
}

void loadFromEEPROM(){
  int loadcount = 0;
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) {
      stepState[i][j] = EEPROM.read(loadcount);
      loadcount++;
    }
  }  
}

void saveToEEPROM() {
  int savecount = 0;
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) {
      EEPROM.write(savecount, stepState[i][j]);
      savecount++;
    }
  }
}



void setMIDI() {
  for (int j = 0; j < 3; j++) {
    //usbMIDI.sendNoteOff((midiNote[j]), 0, 1);
    if (stepState[j][currentStep] == true) {
      usbMIDI.sendNoteOn((midiNote[j]), 99, 1);
    } else if (stepState[j][currentStep] == false) {
      usbMIDI.sendNoteOff((midiNote[j]), 0, 1);
    }
  }
}

void setLeds(int slNum) {
  if (currentStep == slNum) {
    analogWrite(ledPin[slNum], 255);
  }
  else if (stepState[currentChannel][slNum] == true) {
    analogWrite(ledPin[slNum], 80);
  } else if (stepState[currentChannel][slNum] == false) {
    analogWrite(ledPin[slNum], 0);
  }
}

void sequence() {

  tempo = analogRead(A0);

  if (millis() > lastStepTime + tempo) {   //if its time to go to the next step...
    //digitalWrite(ledPin[currentStep], LOW);  //turn off the current led

    currentStep = currentStep + 1;         //increment to the next step
    if (currentStep > 3) {
      currentStep = 0;
    }
    //digitalWrite(ledPin[currentStep], HIGH); //turn on the new led

    lastStepTime = millis();               //set lastStepTime to the current time
    setMIDI();
  }
}

void updateChannel(int ucNum) {
  lastButtonState[ucNum] = buttonState[ucNum];
  buttonState[ucNum] = digitalRead(buttonPin[ucNum]);

  if (buttonState[ucNum] == HIGH && lastButtonState[ucNum] == LOW) {
    //Up Button code
    if (ucNum == 4) {
      currentChannel++;
      if (currentChannel > 2) {
        currentChannel = 2;
      }
    }
    //Down Button code
    if (ucNum == 5) {
      currentChannel--;
      if (currentChannel < 0) {
        currentChannel = 0;
      }
    }
  }
}

void checkButtons(int cbNum) {

  lastButtonState[cbNum] = buttonState[cbNum];
  buttonState[cbNum] = digitalRead(buttonPin[cbNum]);

  if (buttonState[cbNum] == HIGH && lastButtonState[cbNum] == LOW) {
    if (stepState[currentChannel][cbNum] == false) {
      stepState[currentChannel][cbNum] = true;
    } else if (stepState[currentChannel][cbNum] == true) {
      stepState[currentChannel][cbNum] = false;
    }
    saveToEEPROM();
  }
}

/*void setLeds() {
  for (int i = 0; i < 4; i++) {
    if (stepState[i] == true) {
      //digitalWrite(ledPin[i], HIGH);
    } else if (stepState[i] == false) {
      //digitalWrite(ledPin[i], LOW);
    }
  }
  } */
