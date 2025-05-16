// ========================================================================================================
// --- Mapeamento de Hardware ---
#define encoder1_C1   32
#define encoder1_C2   33
#define encoder2_C1   34
#define encoder2_C2   35

#define pwm_motor1    25
#define motor1_IN1    18
#define motor1_IN2    19

#define pwm_motor2    26
#define motor2_IN1    5
#define motor2_IN2    23

#define PPR 20

// ========================================================================================================
// --- Variáveis Globais ---
volatile int pulse_motor1 = 0;
volatile int pulse_motor2 = 0;
bool direction_motor1 = false;
bool direction_motor2 = false;
byte encoder1_C1Last, encoder2_C1Last;
float rpm_motor1 = 0, rpm_motor2 = 0;
unsigned long lastMillis = 0;

int vel1 = 0, vel2 = 0;  // Velocidade dos motores (-100 a 100)

// ========================================================================================================
// --- Configuração Inicial ---
void setup() {
  Serial.begin(115200);

  pinMode(encoder1_C1, INPUT);
  pinMode(encoder1_C2, INPUT);
  pinMode(motor1_IN1, OUTPUT);
  pinMode(motor1_IN2, OUTPUT);

  pinMode(encoder2_C1, INPUT);
  pinMode(encoder2_C2, INPUT);
  pinMode(motor2_IN1, OUTPUT);
  pinMode(motor2_IN2, OUTPUT);

  ledcAttach(pwm_motor1, 5000, 10); // Canal automático
  ledcAttach(pwm_motor2, 5000, 10);

  attachInterrupt(digitalPinToInterrupt(encoder1_C1), count_pulses_motor1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoder2_C1), count_pulses_motor2, CHANGE);
}

// ========================================================================================================
// --- Loop Principal ---
void loop() {
  readSerialInput();    // Lê os comandos do serial
  motor_control();      // Aplica controle
  printRPM();           // Mostra RPM
}

// ========================================================================================================
// --- Leitura dos Comandos pela Serial ---
void readSerialInput() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    // Serial.println(input);
    input.trim();
    if (input.length() > 0) {
      int spaceIndex = input.indexOf(' ');
      if (spaceIndex > 0) {
        vel1 = input.substring(0, spaceIndex).toInt();
        vel2 = input.substring(spaceIndex + 1).toInt();
        Serial.print(vel1);
        Serial.print(" ");
        Serial.println(vel2);

        // Limita valores entre -100 e 100
        // vel1 = constrain(vel1, -255, 255);
        // vel2 = constrain(vel2, -255, 255);
      }
    }
  }
}

// ========================================================================================================
// --- Controle dos Motores ---
void motor_control() {
  // Motor 1
  if (vel1 >= 0) {
    digitalWrite(motor1_IN1, LOW);
    digitalWrite(motor1_IN2, HIGH);
    ledcWrite(pwm_motor1, vel1);
  } else {
    digitalWrite(motor1_IN1, HIGH);
    digitalWrite(motor1_IN2, LOW);
    ledcWrite(pwm_motor1, -vel1);
  }

  // Motor 2
  if (vel2 >= 0) {
    digitalWrite(motor2_IN1, LOW);
    digitalWrite(motor2_IN2, HIGH);
    ledcWrite(pwm_motor2, vel2);
  } else {
    digitalWrite(motor2_IN1, HIGH);
    digitalWrite(motor2_IN2, LOW);
    ledcWrite(pwm_motor2, -vel2);
  }
}

// ========================================================================================================
// --- Impressão da Velocidade (RPM) ---
void printRPM() {
  if (millis() - lastMillis >= 1000) {
    lastMillis = millis();

    rpm_motor1 = (pulse_motor1 * 60.0) / PPR;
    rpm_motor2 = (pulse_motor2 * 60.0) / PPR;

    Serial.print("Motor 1 - RPM: ");
    Serial.print(rpm_motor1);
    Serial.print(" | Motor 2 - RPM: ");
    Serial.println(rpm_motor2);

    pulse_motor1 = 0;
    pulse_motor2 = 0;
  }
}

// ========================================================================================================
// --- Encoder Motor 1 ---
void count_pulses_motor1() {
  int Lstate = digitalRead(encoder1_C1);
  if (!encoder1_C1Last && Lstate) {
    int val = digitalRead(encoder1_C2);
    if (!val && direction_motor1) direction_motor1 = false;
    else if (val && !direction_motor1) direction_motor1 = true;
  }
  encoder1_C1Last = Lstate;
  if (!direction_motor1) pulse_motor1++;
  else pulse_motor1--;
}

// ========================================================================================================
// --- Encoder Motor 2 ---
void count_pulses_motor2() {
  int Lstate = digitalRead(encoder2_C1);
  if (!encoder2_C1Last && Lstate) {
    int val = digitalRead(encoder2_C2);
    if (!val && direction_motor2) direction_motor2 = false;
    else if (val && !direction_motor2) direction_motor2 = true;
  }
  encoder2_C1Last = Lstate;
  if (!direction_motor2) pulse_motor2++;
  else pulse_motor2--;
}
