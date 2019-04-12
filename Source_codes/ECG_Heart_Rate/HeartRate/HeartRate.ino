const int Pin_in = A0; // ECG signal in
const int Beat_out = 11; 

int threshold = 150;
int HR = 0;
int PulseCount = 0;

unsigned long startTime = 0;
unsigned long elapsedTime = 0;

float abl = 0;
float PulseGraph = 0;
float Pulse = 0;
float Pulse_Old = 0;
float val_old = 0;
float val_new = 0;
float val_new_1 = 0;
float weight = 0.4;
float con_weight = 1 - weight;

bool Pulse_State = false;
bool HR_State = false;
bool HR_2_State = false;

void setup() {
  Serial.begin(9600);
  pinMode(Pin_in, INPUT);
  pinMode(Beat_out, OUTPUT);
  digitalWrite(Beat_out, LOW);
}

void loop() {
  val_new = analogRead(Pin_in);
  delay(2);
  PulseGraph = (val_old * con_weight) + (val_new * weight);
  val_old = PulseGraph;

  abl = (PulseGraph - val_new_1);

  if (abl < 0) {
    abl = 0;
  }
  abl = pow(abl, 1.5);
  if (abl < threshold) {
    abl = 0;
  }

  if (abl < 1) {
    Pulse_State = false;
  }

  elapsedTime = millis() - startTime;
  if ((abl > 0) && (Pulse_State == false)) {
    Pulse_State = true;
    HR_State = false;
    HR_2_State = false;
    digitalWrite(Beat_out, HIGH);
    delay(10);
    digitalWrite(Beat_out, LOW);

    PulseCount ++;

    if (PulseCount == 1) {
      startTime = millis();
    }

    if (PulseCount == 2) {
      Pulse = 60000 / double(elapsedTime);
      HR = (Pulse + Pulse_Old) / 2;
      Pulse_Old = Pulse;
      PulseCount = 0;
      if (HR < 200) {
        Serial.println(String("HR")+HR);
      } else {
        Serial.println(String("HR")+200); // HR above 200 bpm
      }
    }
  }

  if ((elapsedTime > 4000) && (elapsedTime < 12000) && (HR_State == false)) {
    Serial.println(String("HR")+14); // HR below 15 bpm
    HR_State = true;
  } else if ((elapsedTime > 12000) && (HR_2_State == false)) {
    Serial.println(String("HR")+0);
    PulseCount = 0;
    HR_2_State = true;
  }

  val_new_1 = PulseGraph;

  //  Serial.print(PulseGraph);
  //  Serial.print(",");
  //  //  Serial.print(abl);
  //  //  Serial.print(",");
  //  Serial.println(0);
}
