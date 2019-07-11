


#define X_STP_PIN 2
#define X_DIR_PIN 5
#define X_EN_PIN 8
#define X_HOME_PIN A0
#define stepsPerRevolution 6400
#define motorSpeedFeed 4000
#define days 7

int feedDay = stepsPerRevolution/((days*2)+1);


void setup (){

  pinMode(X_STP_PIN, OUTPUT);
  pinMode(X_DIR_PIN, OUTPUT);
  pinMode(X_EN_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  delay(500);

  Serial.println("Setup ok");

  digitalWrite(X_DIR_PIN, LOW); //CCW
  digitalWrite(X_EN_PIN, LOW); //zapnutí motoru
  Serial.println("!!!Food incoming!!!");
  delay (500);

  for(int x = 0; x < feedDay; x++){
    digitalWrite(X_STP_PIN, HIGH);
    delayMicroseconds(motorSpeedFeed);
    digitalWrite(X_STP_PIN, LOW);
    delayMicroseconds(motorSpeedFeed);
  }
  delay (500);
  digitalWrite(X_EN_PIN, HIGH); //vypnutí motoru


}

void loop () {
}
