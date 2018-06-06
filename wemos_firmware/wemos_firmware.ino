#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include "SSD1306.h"
#include <FirebaseArduino.h>
#include <Servo.h>

// Set these to run example.
#define FIREBASE_HOST "<YOUR_PROJECT_ID>.firebaseio.com"
#define FIREBASE_AUTH "<YOUR_DB_KEY>"

SSD1306 display(0x3c, SDA, SCL);
#define CHARS_IN_LINE 13

const char* ssid     = "<YOUR WIFI SSID>";      // SSID
const char* password = "<YOUR WIFI PASSWORD>";      // Password
const int   watchdog = 200;        //  Watchdog frequency
unsigned long previousMillis = millis(); 
char cmd_timestamp[64] = "init";

//for heater
const int THERMISTOR_PIN = A0;
const int HEATER_PIN = D0;

#define A 1.131786e-003
#define B 2.336422e-004
#define C 8.985204e-008
#define R 10000
#define T0 273.15

float kp =100.0, ki=0.1, kd=200.0;
float dt=0, pre_dt=0, sum_dt=0;
bool alarm;
float setT=0;
float pt = 0;
int pid_index=0;
char pid_payload[100];

// for led array
const int RED_LED = D5;
const int GREEN_LED = D6;
const int BLUE_LED = D7;

// for Servo
const int SERVO_PIN = D3;
const int PERIOD = 22000;
const int ZERODUTY = 544;
const int PIDUTY = 2400;
const int DELAY = 15;
Servo my_servo;

// for speaker
int SPEAKER_PIN = D8; 
#define NOTE_C4  1915
#define NOTE_D4  1700
#define NOTE_E4  1519
#define NOTE_F4  1432
#define NOTE_G4  1275
#define NOTE_A4  1134
#define NOTE_B4  1014
#define NOTE_C5  954
int melody[] = {NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4, 
        NOTE_A4, NOTE_A4, NOTE_G4, NOTE_F4, NOTE_F4, 
        NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4};

int beats[] = {1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 4};
int tempo = 300;


void oledStr(String st){
  display.clear();
  //display.setTextAlignment(TEXT_ALIGN_CENTER);
  //display.setFont(ArialMT_Plain_16);
  //display.drawString(64, 0, "Draw String");
  
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  int size = st.length() / CHARS_IN_LINE;
  for(int i=0; i<=size; i++){
    display.drawString(2, 16*i + 2, st.substring(CHARS_IN_LINE*i, CHARS_IN_LINE*(i+1)));
  }
  display.display();
}


void playTone(int tone, int duration)
{
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(SPEAKER_PIN, HIGH);
    delayMicroseconds(tone);
    digitalWrite(SPEAKER_PIN, LOW);
    delayMicroseconds(tone);
  }
}

void playMelody()
{
  Serial.println("playMelody()");
  int length = sizeof(melody)/sizeof(int);
  //Serial.println("length : "+ length);
  for(int i=0; i<length; i++){
    //if(is_stopped) return;
    playTone(melody[i], beats[i]*tempo);
    delay(tempo/2);
  }
}

void runServo(){ 
  int pos;
  for(pos=0; pos<=180;pos++){
    my_servo.write(pos);
    delay(DELAY);
  }
  for(pos=180; pos>=0;pos--){
    my_servo.write(pos);
    delay(DELAY);
  }
}

float getTemp() {

  float nom = 5;
  float sum = 0;
  for ( int i = 0; i < nom;i++ ) {
    int adc = analogRead(THERMISTOR_PIN);
   //conversion to temperature
    float u = (float)adc / 1024;
    float lnR = log( (1/u-1.) * R );
    float tmp = A + B * lnR + C * lnR * lnR * lnR;
    //filtering
    float val = (1./tmp - T0);
    sum += val;
  }
  sum = sum/nom;
  
  return sum;
}

void setHeater(float setT, float ct){
      dt = setT-ct;
      sum_dt  += dt;
      float fpwm = kp*dt+ki*sum_dt+kd*(dt-pre_dt);
      if ( fpwm > 1023) fpwm=1023;
      if (fpwm<0) fpwm = 0;
      int pwm = int(fpwm);
      pre_dt = dt;
      if ( pwm > 0 ) analogWrite(HEATER_PIN,pwm);
      else analogWrite(HEATER_PIN,0);
}

String float2str(float fin) {
  int  ipart = int(fin);
  int frac = int((fin - ipart)*10.);
  return String(ipart + "." + frac);
}

void setup() {
  Serial.begin(115200);

  // OLED
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);

  // LED ARRAY
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

  pinMode(SERVO_PIN, OUTPUT);
  pinMode(SPEAKER_PIN, OUTPUT);
  my_servo.attach(SERVO_PIN);
  my_servo.write(0);
  analogWrite(HEATER_PIN,0);
  
  Serial.print("Connecting to ");
  Serial.print(ssid);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.setInt("music",0);
}



void loop() {
  unsigned long currentMillis = millis();
  
  if ( currentMillis - previousMillis > watchdog ) {
    previousMillis = currentMillis;
    
    float ct = getTemp();
    ct = 0.1*ct + 0.9*pt;
    pt = ct;
    if ( ct > 80 || alarm == true) {
     Serial.println("Emergency Stop");
     analogWrite(HEATER_PIN,0);
    }
    Firebase.setFloat("temp",ct);
    
    
    String message = Firebase.getString("message");
    if(message.equals("0")){
      oledStr("");
    }else{
      oledStr(message);
    }
  
    String led = Firebase.getString("led");
    if(led.indexOf("R") >= 0) 
      digitalWrite(RED_LED, HIGH);
    else
      digitalWrite(RED_LED, LOW);
  
    if(led.indexOf("G") >= 0) 
      digitalWrite(GREEN_LED, HIGH);
    else
      digitalWrite(GREEN_LED, LOW);
  
    if(led.indexOf("B") >= 0) 
      digitalWrite(BLUE_LED, HIGH);
    else
      digitalWrite(BLUE_LED, LOW);

    int music = Firebase.getInt("music");
    if(music != 0){
      playMelody();
      Firebase.setInt("music",0);
    }

    int servo = Firebase.getInt("servo");
    if(servo > 0){
      for(int i=0; i<servo; i++)
        runServo();
      Firebase.setInt("servo",0);
    }

    float target_temp = Firebase.getFloat("target_temp");
    if(target_temp > 1){
      if(pid_index == 0){
        Firebase.remove("pid");
        setHeater(target_temp, ct);
      }
      setHeater(target_temp, ct);
      sprintf(pid_payload, "%d:%f", pid_index, ct);
      //Serial.println(pid_payload);
      Firebase.pushString("pid", pid_payload );
      pid_index++;
    }else{
      analogWrite(HEATER_PIN,0);
      pid_index = 0;
    }   
  } // end of read per 0.5s
  
}
