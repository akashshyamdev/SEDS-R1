#include <Adafruit_LiquidCrystal.h>

// Setting PINS
const int BUTTON_PIN = 2;
const int LED_PIN    = 13;
const int BUZZER_PIN = 8;
const int TRIG_PIN   = 9;
const int ECHO_PIN   = 10;
const int LDR_PIN    = A0;

// For LCD
Adafruit_LiquidCrystal lcd(0);

// For machine states
enum ShipState {
  OPEN_SEA,
  ANCHOR_DROPPED,
  STORM,
  CHARYBDIS,
  WRECKED
};

ShipState currentState = OPEN_SEA;


bool lastButtonState = LOW;
unsigned long hazardStartTime = 0;
unsigned long lastBlinkTime   = 0;
bool ledState = LOW;

void setup() {
  // Linking components
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  digitalWrite(TRIG_PIN, LOW);
  
  // Starting LCDs
  lcd.begin(16, 2);
  updateDisplay("OPEN SEA");
}

void loop() {
  // Wrecked state
  if (currentState == WRECKED) {
    digitalWrite(LED_PIN, LOW);
    noTone(BUZZER_PIN);
    return;
  }

  // Check sensors and determine conditions
  int lightVal = analogRead(LDR_PIN);
  bool isStorm = (lightVal < 512);
  
  float distance = readDistanceCM();
  bool isCharybdis = (distance < 100.0 && distance > 0);
  
  // Anchoring logic
  bool currentButtonState = digitalRead(BUTTON_PIN);
  if (currentButtonState == HIGH && lastButtonState == LOW) {
    if (currentState == ANCHOR_DROPPED) {
      currentState = OPEN_SEA;
      updateDisplay("OPEN SEA");
    } else {
      currentState = ANCHOR_DROPPED;
      digitalWrite(LED_PIN, LOW);
      noTone(BUZZER_PIN);
      updateDisplay("ANCHOR DROPPED");
    }
  }
  lastButtonState = currentButtonState;

  // Handling the different states(only when anchor isn't dropped)
  if (currentState != ANCHOR_DROPPED) {
    
    // Keep first entered hazard active until safe or wrecked
    if (currentState == OPEN_SEA) {
      if (isStorm) {
        currentState = STORM;
        hazardStartTime = millis();
        updateDisplay("STORM");
      } else if (isCharybdis) {
        currentState = CHARYBDIS;
        hazardStartTime = millis();
        updateDisplay("CHARYBDIS");
      }
    } 
    // Return to OPEN SEA if hazard clears before 5 seconds
    else if (currentState == STORM && !isStorm) {
      currentState = OPEN_SEA;
      digitalWrite(LED_PIN, LOW);
      updateDisplay("OPEN SEA");
    } 
    else if (currentState == CHARYBDIS && !isCharybdis) {
      currentState = OPEN_SEA;
      noTone(BUZZER_PIN);
      updateDisplay("OPEN SEA");
    }

    // Wreck after 5 seconds of hazard
    if (currentState == STORM || currentState == CHARYBDIS) {
      if (millis() - hazardStartTime >= 5000) {
        currentState = WRECKED;
        updateDisplay("WRECKED");
      }
    }
  }

  // Send alerts
  if (currentState == STORM) {
    if (millis() - lastBlinkTime >= 250) {
      lastBlinkTime = millis();
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
    }
  } else if (currentState == CHARYBDIS) {
    tone(BUZZER_PIN, 1000);
  } else if (currentState != WRECKED) {
    digitalWrite(LED_PIN, LOW);
    noTone(BUZZER_PIN);
  }
}

// Measure distance from sensor
float readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); 
  if (duration == 0) return 999.0;
  return (duration * 0.0343) / 2.0;
}

// Helper function to display text on LCD.
void updateDisplay(String text) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("State:");
  lcd.setCursor(0, 1);
  lcd.print(text);
}