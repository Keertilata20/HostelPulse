const int SENSOR_A = D1;
const int SENSOR_B = D2;

void setup() {
  Serial.begin(115200);
  pinMode(SENSOR_A, INPUT_PULLUP);
  pinMode(SENSOR_B, INPUT_PULLUP);

  Serial.println();
  Serial.println("HostelPulse HW-201 pin diagnostic");
  Serial.println("Raw states: 1 = HIGH, 0 = LOW");
  Serial.println("Move an object in front of one sensor at a time.");
}

void loop() {
  Serial.print("D1/A=");
  Serial.print(digitalRead(SENSOR_A));
  Serial.print("  D2/B=");
  Serial.println(digitalRead(SENSOR_B));
  delay(300);
}
