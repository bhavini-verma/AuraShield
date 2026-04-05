// -------------------- PINS --------------------
#define PIR_PIN     27
#define ECG_PIN     33
#define GSR_PIN     34
#define MIC_PIN     32   // 🔥 ADDED MIC

#define BUTTON_PIN  4

#define RED_LED     14
#define BLUE_LED    26
#define GREEN_LED   25

// -------------------- THRESHOLDS --------------------
int PIR_THRESHOLD = 1;

// 🔥 TUNED VALUES
int ECG_THRESHOLD = 1800;
int GSR_THRESHOLD = 1700;
int MIC_THRESHOLD = 2200;   // 🔥 ADDED MIC THRESHOLD

// -------------------- VARIABLES --------------------
bool systemOn = false;
bool panicOverride = false;

unsigned long lastPressTime = 0;
bool firstPress = false;

// -------------------- SETUP --------------------
void setup() {
  pinMode(PIR_PIN, INPUT);
  pinMode(ECG_PIN, INPUT);
  pinMode(GSR_PIN, INPUT);
  pinMode(MIC_PIN, INPUT);   // 🔥 ADDED

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  pinMode(RED_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  Serial.begin(115200);

  Serial.println("System Ready ✅");
}

// -------------------- LOOP --------------------
void loop() {

  // -------- BUTTON CONTROL --------
  static bool lastState = HIGH;
  bool currentState = digitalRead(BUTTON_PIN);

  if (lastState == HIGH && currentState == LOW) {

    if (firstPress && millis() - lastPressTime < 600) {
      panicOverride = true;
      Serial.println("🔥 PANIC ");
    } 
    else {
      systemOn = !systemOn;
      Serial.println(systemOn ? "System ON" : "System OFF");
    }

    firstPress = true;
    lastPressTime = millis();
  }

  lastState = currentState;

  if (millis() - lastPressTime > 600) {
    firstPress = false;
  }

  // -------- SYSTEM OFF --------
  if (!systemOn) {
    digitalWrite(RED_LED, LOW);
    digitalWrite(BLUE_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    return;
  }

  // -------- SENSOR READ --------
  int pirValue = digitalRead(PIR_PIN);
  int ecgValue = analogRead(ECG_PIN);
  int gsrValue = analogRead(GSR_PIN);
  int micValue = analogRead(MIC_PIN);   // 🔥 ADDED

  // -------- FLAGS --------
  bool pirFlag = (pirValue >= PIR_THRESHOLD);
  bool ecgFlag = (ecgValue > ECG_THRESHOLD);
  bool gsrFlag = (gsrValue > GSR_THRESHOLD);
  bool micFlag = (micValue > MIC_THRESHOLD);   // 🔥 ADDED

  int total = pirFlag + ecgFlag + gsrFlag + micFlag;   // 🔥 UPDATED COUNT

  // -------- DECISION --------
  String zone = "SAFE";

  if (panicOverride || total >= 3) {
    zone = "DANGER";
  }
  else if (total == 2) {
    zone = "ALERT";
  }

  // -------- SERIAL OUTPUT --------
  Serial.print("PIR: "); Serial.print(pirValue);
  Serial.print(" | ECG: "); Serial.print(ecgValue);
  Serial.print(" | GSR: "); Serial.print(gsrValue);
  Serial.print(" | MIC: "); Serial.print(micValue);   // 🔥 ADDED
  Serial.print(" | Count: "); Serial.print(total);
  Serial.print(" | Zone: "); Serial.println(zone);

  // -------- LED OUTPUT --------
  if (zone == "DANGER") {
    digitalWrite(RED_LED, millis() % 300 < 150);
    digitalWrite(BLUE_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
  }
  else if (zone == "ALERT") {
    digitalWrite(RED_LED, LOW);
    digitalWrite(BLUE_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
  }
  else {
    digitalWrite(RED_LED, LOW);
    digitalWrite(BLUE_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
  }

  delay(500);
}