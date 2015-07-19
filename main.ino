#include <Thread.h>
#include <ThreadController.h>
#include <TrueRandom.h>
#include <Stepper.h>

#define STEPS 100
Stepper stepper(STEPS, 8, 11, 12, 13);

///////////////////////////////////////////////////
//ThreadController that will controll all threads//
///////////////////////////////////////////////////
ThreadController controll = ThreadController();
Thread rightThread = Thread();
Thread leftThread = Thread();

//////////////////
///// Angle //////
//////////////////
int center = 750;
int limitRight = 700;
int limitLeft = 820;
int angPin = 4;
int a;
char dir;

//////////////////////
///// Solenoids //////
//////////////////////
int solRight = 2;
int solLeft = 3;

///////////////////
///// timers //////
///////////////////
int intervalRight;
int intervalLeft;
float r = 0;
int dropsRight = 0;
int dropsLeft = 0;

/////////////////////////////////////
/////////////////////////////////////

int spead;
boolean solPower = true;
boolean sysState = true;

/////////////////////////////////////
/////////////////////////////////////

// callback for right Drop
void dropHitRight() {
  digitalWrite(solRight, solPower);
  delay(15);
  digitalWrite(solRight, LOW);
  dropsRight++;
}

// callback for left Drop
void dropHitLeft() {
  digitalWrite(solLeft, solPower);
  delay(15);
  digitalWrite(solLeft, LOW);
  dropsLeft++;
}

/////////////////////////////////////
/////////////////////////////////////

int angleData() {
  a = analogRead(angPin);
  return a;
}

/////////////////////////////////////
/////////////////////////////////////

int safeBalance = 0;

void restart() {
  dropsRight = 0;
  dropsLeft = 0;
  sysState = true;

  intervalRight = TrueRandom.random(1, 10) * 1000;
  intervalLeft = TrueRandom.random(1, 10) * 1000;

  // Configure rightThread
  rightThread.onRun(dropHitRight);
  rightThread.setInterval(intervalRight);

  // Configure leftThread
  leftThread.onRun(dropHitLeft);
  leftThread.setInterval(intervalLeft);

  // Adds both threads to the controller
  controll.add(&rightThread);
  controll.add(&leftThread);

  if (intervalRight == intervalLeft) {
    intervalRight += 1000;
  }
  r = intervalRight - intervalLeft;
  if (r < 0) {
    r = r * -1;
  }

  if (intervalRight > intervalLeft) {
    dir = 'l';
  }
  if (intervalRight < intervalLeft) {
    dir = 'r';
  }

  spead = map(r, 0, 10000, 1, 10);
  stepper.setSpeed(spead);

} Industrial adhesive velcro
abrazaderas metal:
- para agarrar el motor con el marco
zip - tie pequenios
curl wire wrapper
power source 5V and
screws for potentiometer shield
something to tie the wires that move the pot

/////////////////////////////////////
/////////////////////////////////////

void setup() {

  //Serial.begin(9600);

  pinMode(6, OUTPUT);

  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  digitalWrite(9, HIGH);
  digitalWrite(10, HIGH);

  pinMode(solRight, OUTPUT);
  pinMode(solLeft, OUTPUT);

  restart();
}

/////////////////////////////////////
/////////////////////////////////////
int safe = 0;

void loop() {
  int ang = angleData();
  if (sysState) {
    controll.run();
    if (dir == 'r') {
      stepper.step(1);
    } else {
      stepper.step(-1);
    }
    if (ang <= limitRight || ang >= limitLeft) {
      if (safe > 0) {
        sysState = false;
        safe = 0;
      }
    }
    safe++;
    debug();

  } else {
    // -10 for a margin of error
    if (safeBalance == 0) {
      delay(10000);
      stepper.setSpeed(50);
    }
    safeBalance++;
    if (ang > center + 5) {
      dir = 'l';
    } else if (ang < center) {
      dir = 'r';
    } else {
      dir = 'c';
    }
    if (dir == 'r') {
      stepper.step(-1);
    } else if (dir == 'l') {
      stepper.step(1);
    } else if (dir == 'c') {
      //Serial.println("CENTERED");
      safeBalance = 0;
      delay(10000);
      sysState = true;
      restart();
    }
  }
}

/////////////////////////////////////
/////////////////////////////////////

void debug() {
  Serial.print("force_diff:");
  Serial.print(r);
  Serial.print(" / speed:");
  Serial.print(spead);
  Serial.print(" / direction:");
  Serial.print(dir);
  Serial.print(" / int_right:");
  Serial.print(intervalRight);
  Serial.print(" / int_left:");
  Serial.print(intervalLeft);
  Serial.print(" / Drops#L:");
  Serial.print(dropsLeft);
  Serial.print(" / Drops#R:");
  Serial.print(dropsRight);
  Serial.print(" / sysState:");
  Serial.print(sysState);
  Serial.print(" / ang:");
  Serial.println(a);
}
