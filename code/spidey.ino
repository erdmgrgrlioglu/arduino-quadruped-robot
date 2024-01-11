#include <Servo.h>

// define servo ports
const uint8_t servoPinsSize = 8;
const uint8_t servoPins[servoPinsSize] = { 2, 3, 4, 5, 6, 7, 8, 9 };

const uint8_t inversePinsSize = 4;
const uint8_t inversePins[inversePinsSize] = { 4, 5, 6, 7 };

// define delay amount between moves
const unsigned int delayAmount = 300;

// define foot movement angles
const uint8_t footLiftDeg = 30;
const uint8_t footResetDeg = 60;

const uint8_t frontTravelDeg = 20;
const uint8_t frontTravelResetDeg = 90;

const uint8_t backTravelDeg = 110;
const uint8_t backTravelResetDeg = 70;

// joint class that holds move() functionality and holds various data about the servo and how it should be moved.
class Joint{
  private:
    Servo servo;
    bool inverse = false;
    uint8_t currentDeg = 90;
    unsigned int stepDelay = 1;
    uint8_t pin = 0;

  public:
    Joint(){}

    Joint(uint8_t servoPin, uint8_t degree) {
      servo.attach(servoPin);
      pin = servoPin;
      servo.write(90);   
      // Serial.println((String)"init joint with pin: " + servoPin + " inverse: false");
    }

    Joint(uint8_t servoPin, bool isInverse, uint8_t degree) {
      inverse = isInverse;
      servo.attach(servoPin);
      pin = servoPin;
      servo.write(90);
      // Serial.println((String)"init joint with pin: " + servoPin + " inverse: " + isInverse);
    }

    void move(uint8_t newDeg, unsigned int wait) {
      //Serial.println((String)"move pin: "+ pin + " deg: " + newDeg);
      if(inverse) {
        if(currentDeg < newDeg) {
          for(int deg = currentDeg; deg < newDeg; deg++) {
            servo.write(180-deg);
            delay(stepDelay);
          }
        }
        else {
          for(int deg = currentDeg; deg > newDeg; deg--) {
            servo.write(180-deg);
            delay(stepDelay);
          }
        }
      }
      else {
        if(currentDeg < newDeg) {
          for(int deg = currentDeg; deg < newDeg; deg++) {
            servo.write(deg);
            delay(stepDelay);
          }
        }
        else {
          for(int deg = currentDeg; deg > newDeg; deg--) {
            servo.write(deg);
            delay(stepDelay);
          }
        }
      }
      currentDeg = newDeg;
      delay(wait); // delay between moves
    } 

    uint8_t getCurrentDeg() {
      return currentDeg;
    }

    uint8_t getPin(){
      return pin;
    }

    void setPin(uint8_t pin) {
      this->pin = pin;
      servo.attach(pin);
    }

    void setInverse() {
        inverse = !inverse;
    } 

    void setStepDelay(unsigned int stepDelay) {
      this->stepDelay = stepDelay;
    }
};

struct Group {
  Joint* joint;
  uint8_t targetDeg;
  float stepLength;
};

void moveJointGroup(Group group[], uint8_t arraySize, int step, int stepDelay) {
  // Serial.println((String)"move group size: " + arraySize);
  if(step <= 0){
    for(uint8_t i = 0; i < arraySize; i++) {
      group[i].joint->move(group[i].targetDeg, 0);
    }
    delay(stepDelay);
    return;
  }

  for(uint8_t i = 0; i < arraySize; i++) {
    group[i].stepLength = -(group[i].joint->getCurrentDeg() - group[i].targetDeg)/step;
    group[i].targetDeg = group[i].joint->getCurrentDeg();
  }
  
  for(int currentStep = 1; currentStep < step+1; currentStep++) {
    for(int i = 0; i < arraySize; i++) {
      group[i].joint->move(group[i].targetDeg + (group[i].stepLength * currentStep), 0);
    }
    delay(stepDelay);
  }
  
  for(uint8_t i = 0; i < arraySize; i++) {
    group[i].targetDeg = group[i].joint->getCurrentDeg();
  }
}

Joint* joints = (Joint*)malloc(sizeof(Joint) * 8);
Group* right = (Group*)malloc(sizeof(Group) * 2);
Group* left = (Group*)malloc(sizeof(Group) * 2);

void setup() {
  // Serial.begin(9600); Serial.println("program starts");
  //initialize all servos
  bool isInverse = false;
  for (uint8_t i = 0; i < servoPinsSize; i++) {
    isInverse = false;
    for(uint8_t j = 0; j < inversePinsSize; j++) {
      if(servoPins[i] == inversePins[j]) isInverse = true;
    }
    if(isInverse) joints[i] = Joint(servoPins[i],true,90);
    else joints[i] = Joint(servoPins[i],90);
    delay(300);
  }
  // define joint groups
  // right side of the robot
  right[0].joint = &joints[0];
  right[0].targetDeg = frontTravelResetDeg;
  right[1].joint = &joints[2];
  right[1].targetDeg = backTravelResetDeg;
  ////  left side of the robot
  left[0].joint = &joints[4];
  left[0].targetDeg = frontTravelResetDeg;
  left[1].joint = &joints[6]; 
  left[1].targetDeg = backTravelResetDeg;

  // set joints to pos 45
  for(uint8_t i = 0; i < 8; i++) {
    joints[i].move(45,delayAmount);
  }

  // say hello
  joints[0].move(0,200);
  joints[4].move(0,200);

  joints[1].move(0,0);
  joints[5].move(20,800);

  joints[1].move(20,0);
  joints[5].move(0,200);

  joints[1].move(0,0);
  joints[5].move(20,200);

  joints[1].move(20,0);
  joints[5].move(0,200);

  joints[1].move(0,0);
  joints[5].move(20,200);

  joints[1].move(20,0);
  joints[5].move(0,200);

  joints[1].move(0,0);
  joints[5].move(20,800);

  joints[1].move(45,0);
  joints[5].move(45,0);
  joints[0].move(45,0);
  joints[4].move(45,800);

  // dance!
  for(uint8_t i = 0; i < 12; i++) {
    joints[2*(i%4)+1].move(0,200);
    joints[2*(i%4)+1].move(45,400);
  }

  // ready
  moveJointGroup(right, 2, 0, 0);
  moveJointGroup(left, 2, 0, 0);

  // wait before loop
  delay(2000);
}

void loop(void) { 
  // step 1
  joints[3].move(footLiftDeg,delayAmount);
  joints[2].move(backTravelDeg,delayAmount);
  joints[3].move(footResetDeg,delayAmount); 
  // step 2
  joints[1].move(footLiftDeg,delayAmount);
  joints[0].move(frontTravelDeg,delayAmount);
  joints[1].move(footResetDeg,delayAmount);
  // reset joint group pos
  moveJointGroup(right, 2, 20, 50);
  // step 3
  joints[7].move(footLiftDeg,delayAmount);
  joints[6].move(backTravelDeg,delayAmount);
  joints[7].move(footResetDeg,delayAmount);
  // step 4
  joints[5].move(footLiftDeg,delayAmount);
  joints[4].move(frontTravelDeg,delayAmount);
  joints[5].move(footResetDeg,delayAmount);
  // reset joint group pos
  moveJointGroup(left, 2, 20, 50);
}