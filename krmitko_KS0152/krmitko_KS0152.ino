/*
krmitko pro ryby po dobu 14 dní
- pasivní přístup k časování - millis
- krmení v odpolendích hodinách 15-18
- napojeno na X axis

- pinmapa
  - X_STP - D2
  - X_DIR - D5
  - X_EN  - D8
  - X_HOME - A0

- setup:

- loop
  - home
    - otočení dokud nesepne sonda
  - poototčenáí o dané otáčky jednou za odpolendích
  - konstrukce napojena na přímo
  - v mezipoloze motor uvolněn


*/


#define X_STP_PIN 2
#define X_DIR_PIN 5
#define X_EN_PIN 8
#define X_HOME_PIN A0
#define stepsPerRevolution 6400
#define dayDelay 86400000//*60 //1 day delay is (1000*60*60*24) -  86400000 millis
#define blinkNormalSpeed 1000 //in ms
#define blinkErrSpeed 200//in ms
#define motorSpeedHome 1000
#define motorSpeedFeed 4000
#define days 14

bool ledState = HIGH;
byte state = 0; //0 == err; undef total mistakes
byte previousState;
int xHomeSens = 0;
bool firstHome = 0;
int sensorMinEdge = 100;
byte prevDays = 0;
unsigned long homeStepsCW = 1;
unsigned long homeStepsCCw = 1;
int feedDay = stepsPerRevolution/(days+1);
int blinkSpeedSet;
unsigned long currentMillis = 0;
unsigned long previousLedMillis = 0;
unsigned long previousFeedMillis = 0;


void setup (){
  Serial.begin(9600);
  Serial.println("Program starting");
  digitalWrite(LED_BUILTIN, ledState);

  pinMode(X_STP_PIN, OUTPUT);
  pinMode(X_DIR_PIN, OUTPUT);
  pinMode(X_EN_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  delay(500);
  Serial.println("Setup ok");

}

void loop () {
  currentMillis = millis();
  xHomeSens = analogRead(X_HOME_PIN);
  digitalWrite(X_DIR_PIN, HIGH); //směr CW
  digitalWrite(X_EN_PIN, HIGH); //vypnutí motoru

  if ((firstHome == 0) && (xHomeSens > sensorMinEdge)){
    homeCCW();
  }
  else if ((firstHome == 0) && (xHomeSens < sensorMinEdge)){
    homeCW();
  }
  else if (firstHome != 0){
    if(xHomeSens > sensorMinEdge) { // || ((xHomeSens > sensorMinEdge) && (prevDays > days)))
      state = 0;
    }
    else if ((prevDays >= days) && (xHomeSens < sensorMinEdge)) {
      state = 2;
    }
    else{
      state = 1;
    }
  }
  else{
    state = 0;
  }

  feedFood();
  blinkSpeed();
  blinkStatusLed();

  Serial.println(currentMillis);
  delay(500);

}


void homeCCW(){
  digitalWrite(X_EN_PIN, LOW); //zapnutí motoru
  digitalWrite(X_DIR_PIN, LOW);//směr CWW
  for(int x = 0; x < homeStepsCCw; x++){
    digitalWrite(X_STP_PIN, HIGH);
    delayMicroseconds(motorSpeedHome*3);
    digitalWrite(X_STP_PIN, LOW);
    delayMicroseconds(motorSpeedHome*3);
    xHomeSens = analogRead(X_HOME_PIN);
    //Serial.println(xHomeSens + "CCW");

    if(xHomeSens > sensorMinEdge){
      homeStepsCCw++;
    }
  }
  digitalWrite(X_DIR_PIN, HIGH);//směr CW
  delay(500);
  digitalWrite(X_EN_PIN, HIGH);
  previousFeedMillis = millis();
  //firstHome = 1;
}


void homeCW(){
  digitalWrite(X_EN_PIN, LOW); //zapnutí motoru
  for(int x = 0; x < homeStepsCW; x++){
    digitalWrite(X_STP_PIN, HIGH);
    delayMicroseconds(motorSpeedHome);
    digitalWrite(X_STP_PIN, LOW);
    delayMicroseconds(motorSpeedHome);
    xHomeSens = analogRead(X_HOME_PIN);
    //Serial.println(xHomeSens + "CW");

    if(xHomeSens < sensorMinEdge){
      homeStepsCW++;
    }
  }
  delay(500);
  digitalWrite(X_EN_PIN, HIGH);
  previousFeedMillis = millis();
  firstHome = 1;
}


void feedFood(){
  if(((firstHome !=0) && (state == 1) && (currentMillis >= (previousFeedMillis + dayDelay)) && (prevDays < days)) || ((firstHome !=0) && (state == 0) && (currentMillis >= (previousFeedMillis + dayDelay)) && (prevDays == 0))) {
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
    Serial.println("Feed " + String(prevDays+1) + " days/days");
    prevDays++;
    previousFeedMillis = currentMillis;
    delay (500);
    digitalWrite(X_EN_PIN, HIGH); //vypnutí motoru
  }
}


void blinkSpeed(){
  switch (state){
    case 0:
    if (state != previousState){
      blinkSpeedSet = blinkErrSpeed;
      Serial.println("Use ERROR speed for blink.");
    }
    break;

    case 1:
    if (state != previousState){
      blinkSpeedSet = blinkNormalSpeed;
      Serial.println("Use NORMAL speed for blink.");
    }
    break;

    case 2:
    if(state != previousState){
      blinkSpeedSet = 50;
      Serial.println("Use MAXIMUM DAY speed for blink.");
    }
    else {
      blinkSpeedSet = 100;
    }
    if (state != previousState){
      Serial.println("Maximum day done. Please refill food");
    }
    break;


    default:
    if(ledState == 1){
      blinkSpeedSet = 30;
    }
    else {
      blinkSpeedSet = 470;
    }
    if (state != previousState){
      Serial.println("Use brutal error speed - error loop and blink speed - chceck sketch and search big mistakes");
    }
  }
  previousState = state;
}


void blinkStatusLed(){
  if (currentMillis >= (previousLedMillis+blinkSpeedSet)){
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState);
    //Serial.println("ledState: " + String(ledState));
    previousLedMillis = currentMillis;
  }
}
