//======================Motor Encoders====================
volatile int TickR = 0;
volatile int TickL = 0;
//======================Pin Aliases======================
const int A_1 = 7;
const int A_2 = 6;
const int A_3 = 5;
const int A_4 = 4;
const int EN = 9;
const int ENR = 10;
const int Front = 0;
const int FrontLeft = 1;
const int FrontRight = 2;
const int BackLeft = 3;
const int BackRight = 4;
//=====================Logic Variables==================
boolean Moving = false;
boolean LineUpComplete = false;
int MotorSpeed = 100;    //0 - 255 (0 = minimum, 255 = maximum)
int DistanceTravelled = 0;    //Used to count the distance the mouse has travelled when moving a square
int SquareLength = 160;       //The number of ticks representing 1 maze square
int SlowDownDelay = 60;       //This delay makes it so that the motors are completely stopped before doing something else (needs tweaking)
int RightMotorDelay = 150;    //This delay makes it so the motors stop together (needs tweaking) <- is there a better way of doing this?
//===================Sensor Data=========================
int IRFrontRight = 0;
int IRBackRight = 0;
int IRFrontLeft = 0;
int IRBackLeft = 0;
int IRFront = 0;
//===================Setup Function=======================
void setup() {
  Serial.begin(9600);
  pinMode(A_1, OUTPUT);
  pinMode(A_2, OUTPUT);
  pinMode(EN, OUTPUT);
  pinMode(ENR, OUTPUT);
  pinMode(A_3, OUTPUT);
  pinMode(A_4, OUTPUT);
  digitalWrite(EN, LOW);
  digitalWrite(ENR, LOW);
  attachInterrupt(0,TickRInt, CHANGE);
  attachInterrupt(1,TickLInt, CHANGE);
  digitalWrite(A_1, HIGH);
  digitalWrite(A_2, LOW);
  digitalWrite(A_3, HIGH);
  digitalWrite(A_4, LOW);
}

void loop()
{
  LineUp();        //First line up
  delay(1000);
  TravelASquare();  //Then move 1 square
  delay(1000000);
}

void TravelASquare()
{
  Moving = true;
  DistanceTravelled = 0;
  analogWrite(ENR, MotorSpeed);    //Turn on both motors
  analogWrite(EN, MotorSpeed);
  while (DistanceTravelled < SquareLength)
  {
    DistanceTravelled = (TickL + TickR) / 2;  //This only works once!!!!!!!!!!!!!!!!!!!! needs to be changed to work multiple times!
    Serial.print("Distance: ");
    Serial.println(DistanceTravelled);
    CorrectStraightness();                    //While moving, keep the robot driving straight
  }
  Moving = false;
  digitalWrite(ENR, LOW);    //Turn off the right motor
  delay(RightMotorDelay);    //Wait
  digitalWrite(EN, LOW);     //Now turn off the left motor
  delay(SlowDownDelay);      //Deceleration
}

void LineUp()
{
  LineUpComplete = false;
  
  //Add these numbers to the front sensor value and you should get the back sensor value IF THE SIDE IS STRAIGHT
  int RightStraightOffset = 0;
  int LeftStraightOffset = -70;

  while (!LineUpComplete)
  {
    IRFront = analogRead(Front);
    IRFrontRight = analogRead(FrontRight);
    IRBackRight = analogRead(BackRight);
    IRFrontLeft = analogRead(FrontLeft);
    IRBackLeft = analogRead(BackLeft);

    int LeftError = IRFrontLeft + LeftStraightOffset - IRBackLeft;
    if (LeftError < -10)	//Robot is turned counter-clockwise
    {
      Serial.println("Turned CCW, Rotate CW");
      analogWrite(EN, MotorSpeed);
      digitalWrite(ENR, LOW);
      delay(10);      //Lowering this will make is more precise, but take more time
      digitalWrite(EN, LOW);
      digitalWrite(ENR, LOW);
      delay(SlowDownDelay);
    }
    else if (LeftError > 10)	//Robot is turned clockwise
    {
      Serial.println("Turned CW, Rotate CCW");
      digitalWrite(EN, LOW);
      analogWrite(ENR, MotorSpeed);
      delay(10);      //Lowering this will make is more precise, but take more time
      digitalWrite(EN, LOW);
      digitalWrite(ENR, LOW);
      delay(SlowDownDelay);
    }
    else	//Robot is straight
    {
      Serial.println("Robot is straight.");
      LineUpComplete = true;
    }
  }
}

boolean CorrectStraightness()
{
  if (TickR > TickL + 5)    //If the right motor is ahead of the left
  {
    digitalWrite(ENR, LOW);
    analogWrite(EN, MotorSpeed);
    return true;
  }
  else if (TickL > TickR + 5)  //If the left motor is ahead of the right
  {
    digitalWrite(EN, LOW);
    analogWrite(ENR, MotorSpeed);
    return true;
  }
  else
  {
    if (Moving)    //Both motors are in sync
    {
      analogWrite(EN, MotorSpeed);  //If you were previously moving, stay moving
      analogWrite(ENR, MotorSpeed);
    }
    else
    {
      digitalWrite(EN, LOW);        //If you were previously moving, stop moving
      digitalWrite(ENR, LOW); 
    }
    return false;
  }
}

//====================Interrupts=========================
void TickRInt()
{
  TickR++;
  return;
}
void TickLInt()
{
  TickL++;
  return;
}
//=======================================================

