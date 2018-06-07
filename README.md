# Firebase and Web based IoT home automation simulator
Simulating IoT home automation system with wemos and firebase

## Introduction

- Controller: Wemos D1 mini pro
- Database: Firebase
- Client: Web (Vue.js + Element UI)


## Statement

- LED: Simulating Lighting system of home
- Servo: To Simulate curtains control
- PID: To simulate temperature control
- OLED Display: To display a message from user send through cloud
- Speaker: To play a music
- WiFi: To communicate with the cloud
<img src="https://github.com/wzsh/autohome_sim/blob/master/images/hardware.jpg" width="600" />


## System Design
### Wemos Pins

* Thermistor: A0
* Heater: D0
*	OLED Display: D1(SCL), D2(SDA)
*	Servo: D3
*	Led Array: D5,D6,D7
*	Speaker: D8


## Web UI
Demo URL: https://wzs-autohome.firebaseapp.com

<p float="center">
  <img src="https://github.com/wzsh/autohome_sim/blob/master/images/ui1.jpg" width="250" />
  <img src="https://github.com/wzsh/autohome_sim/blob/master/images/ui2.jpg" width="250" /> 
  <img src="https://github.com/wzsh/autohome_sim/blob/master/images/pid.jpg" width="250" />
</p>


