
const int SENSOR_A = D5;
const int SENSOR_B = D6;

int peopleCount = 0;
int previousA = HIGH;
int previousB = HIGH;

unsigned long sequenceStart = 0;
const unsigned long TIMEOUT = 1500;

int sequence = 0;
// 0 = waiting
// 1 = A triggered first
// 2 = B triggered first

void setup() {
  Serial.begin(115200);

  pinMode(SENSOR_A, INPUT);
  pinMode(SENSOR_B, INPUT);

  Serial.println("HostelPulse People Counter");
  Serial.println("Initial count: 0");
}

void loop() {
  int a = digitalRead(SENSOR_A);
  int b = digitalRead(SENSOR_B);

  bool hitA = (a == LOW && previousA == HIGH);
  bool hitB = (b == LOW && previousB == HIGH);

  if (sequence == 0) {
    if (hitA) {
      sequence = 1;
      sequenceStart = millis();
    } else if (hitB) {
      sequence = 2;
      sequenceStart = millis();
    }
  }

  else if (sequence == 1 && hitB) {
    peopleCount++;
    sequence = 0;

    Serial.print("ENTRY | People inside: ");
    Serial.println(peopleCount);
  }

  else if (sequence == 2 && hitA) {
    if (peopleCount > 0) {
      peopleCount--;
    }

    sequence = 0;

    Serial.print("EXIT | People inside: ");
    Serial.println(peopleCount);
  }

  if (sequence != 0 && millis() - sequenceStart > TIMEOUT) {
    sequence = 0;
  }

  previousA = a;
  previousB = b;

  delay(20);
}
