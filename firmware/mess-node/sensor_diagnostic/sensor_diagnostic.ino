const int SENSOR_A = D1;
const int SENSOR_B = D2;

const char* sensorState(int value) {
  return value == HIGH ? "object_detected" : "not_detected";
}

void setup() {
  Serial.begin(115200);
  pinMode(SENSOR_A, INPUT_PULLUP);
  pinMode(SENSOR_B, INPUT_PULLUP);

  Serial.println();
  Serial.println("HostelPulse HW-201 pin diagnostic");
  Serial.println("Move an object in front of one sensor at a time.");
}

void loop() {
  int stateA = digitalRead(SENSOR_A);
  int stateB = digitalRead(SENSOR_B);

  Serial.print("D1/A=");
  Serial.print(sensorState(stateA));
  Serial.print("  D2/B=");
  Serial.println(sensorState(stateB));

  delay(300);
}