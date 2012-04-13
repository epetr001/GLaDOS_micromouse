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
boolean Centered = true;
int MotorSpeed = 100; //0 - 255 (0 = minimum, 255 = maximum)
int DistanceTravelled = 0; //Used to count the distance the mouse has travelled when moving a square
int SquareLength = 170; //The number of ticks representing 1 maze square
int SlowDownDelay = 60; //This delay makes it so that the motors are completely stopped before doing something else (needs tweaking)
int RightMotorDelay = 150; //This delay makes it so the motors stop together (needs tweaking) <- is there a better way of doing this?
int OriginalDistance = 0;
//===================Sensor Data=========================
int IRFrontRight = 0;
int IRBackRight = 0;
int IRFrontLeft = 0;
int IRBackLeft = 0;
int IRFront = 0;
//========================================================
/*    Notes:
 *    Optical mouse sensors chips:
 *    Agilent A2610, (this is the chip in the logitech mouse we have) 
 *        Resolution: 400cpi      Max-Speed: 12ips
 *    ADNS-9500 ($10 @ digi-key)
 *        Resolution: 5670cpi     Max-Speed: 150ips
 *
 *
 */
//===================Setup Function=======================
void setup()
{
  Serial.begin(9600);
  pinMode(A_1, OUTPUT);
  pinMode(A_2, OUTPUT);
  pinMode(EN, OUTPUT);
  pinMode(ENR, OUTPUT);
  pinMode(A_3, OUTPUT);
  pinMode(A_4, OUTPUT);
  digitalWrite(EN, LOW);
  digitalWrite(ENR, LOW);
  attachInterrupt(0, TickRInt, CHANGE);
  attachInterrupt(1, TickLInt, CHANGE);
  digitalWrite(A_1, HIGH);
  digitalWrite(A_2, LOW);
  digitalWrite(A_3, HIGH);
  digitalWrite(A_4, LOW);
}

void loop()
{
  if (IsCentered())
  {
    LineUp(); //First line up
    delay(1000);
    ReadSensors();
    if (IRFront < 850)
      TurnRight();
    else
      TravelASquare(); //Then move 1 square
    delay(1000);
  }
  else
  {
    MoveToCenter();
    delay(1000);
    if (IRFront < 850)
      TurnRight();
    else
      TravelASquare();
    delay(1000);
    LineUp();
    delay(1000);
  }
}

void TravelASquare()
{
  Moving = true;
  DistanceTravelled = 0;
  OriginalDistance = TickL;
  analogWrite(ENR, MotorSpeed); //Turn on both motors
  analogWrite(EN, MotorSpeed);
  while (DistanceTravelled < OriginalDistance + SquareLength) //Check if distance travelled is over a threshold
  {
    DistanceTravelled = TickL;
    ReadSensors();
    if (IRFront < 940)
    {
      break; 
    }
  }
  digitalWrite(ENR, LOW);
  digitalWrite(EN, LOW);
  Moving = false;
}

void LineUp()
{
  LineUpComplete = false;

  //Add these numbers to the front sensor value and you should get the back sensor value IF THE SIDE IS STRAIGHT
  int RightStraightOffset = -100;
  int LeftStraightOffset = -70;

  while (!LineUpComplete)
  {
    ReadSensors();
    boolean LeftWall = true;
    boolean RightWall = true;
    if (IRFrontLeft > 940 && IRBackLeft > 940)  //No Left Wall
      LeftWall = false;
    if (IRFrontRight > 955 && IRBackRight > 950)  //No Right Wall
      RightWall = false;

    int LeftError = (IRFrontLeft + LeftStraightOffset - IRBackLeft);
    int RightError = (IRFrontRight + RightStraightOffset - IRBackRight);
    int LeftAdjustment = -1;    //-1 = unset, 0 = go CW, 1 = go CCW, 2 = do nothing
    int RightAdjustment = -1;

    OutputSensors();
    Serial.print("LeftWall: ");
    Serial.println(LeftWall);
    Serial.print("RightWall: ");
    Serial.println(RightWall);
    Serial.print("LeftError: ");
    Serial.println(LeftError);
    Serial.print("RightError: ");
    Serial.println(RightError);

    if (LeftWall && RightWall)
    {
      if (LeftError < -10) //Robot is turned counter-clockwise
        LeftAdjustment = 0;
      else if (LeftError > 10)
        LeftAdjustment = 1; 
      else //Robot is straight
      LeftAdjustment = 2;

      if (RightError < -10)  //Robot is turned clockwise
        RightAdjustment = 1;
      else if (RightError > 10)
        RightAdjustment = 0;
      else
        RightAdjustment = 2;

      Serial.print("LeftAdjustment: ");
      Serial.println(LeftAdjustment);
      Serial.print("RightAdjustment: ");
      Serial.println(RightAdjustment);

      if (LeftAdjustment == RightAdjustment)
      {
        if (LeftAdjustment == 0)  //Go CW
        {
          analogWrite(EN, MotorSpeed);
          digitalWrite(ENR, LOW);
          delay(20); //Lowering this will make is more precise, but take more time
          digitalWrite(EN, LOW);
          digitalWrite(ENR, LOW);
          delay(SlowDownDelay);
        }
        else if (LeftAdjustment == 1)  //Go CCW
        {
          analogWrite(ENR, MotorSpeed);
          digitalWrite(EN, LOW);
          delay(20); //Lowering this will make is more precise, but take more time
          digitalWrite(EN, LOW);
          digitalWrite(ENR, LOW);
          delay(SlowDownDelay);
        }
        else if (LeftAdjustment == 2)
        {
          LineUpComplete = true;
          TickL = 0;
          TickR = 0;
        }
      }
      else  //The directions do not agree
      {
        if (((IRFrontLeft + IRBackLeft) / 2) >= ((IRFrontRight + IRBackRight) / 2))
        {
          if (RightAdjustment == 0)  //Go CW
          {
            analogWrite(EN, MotorSpeed);
            digitalWrite(ENR, LOW);
            delay(20); //Lowering this will make is more precise, but take more time
            digitalWrite(EN, LOW);
            digitalWrite(ENR, LOW);
            delay(SlowDownDelay);
          }
          else if (RightAdjustment == 1)  //Go CCW
          {
            analogWrite(ENR, MotorSpeed);
            digitalWrite(EN, LOW);
            delay(20); //Lowering this will make is more precise, but take more time
            digitalWrite(EN, LOW);
            digitalWrite(ENR, LOW);
            delay(SlowDownDelay);
          }
          else if (RightAdjustment == 2)
          {
            LineUpComplete = true;
            TickL = 0;
            TickR = 0; 
          }
        }
        else
        {
          if (LeftAdjustment == 0)  //Go CW
          {
            analogWrite(EN, MotorSpeed);
            digitalWrite(ENR, LOW);
            delay(20); //Lowering this will make is more precise, but take more time
            digitalWrite(EN, LOW);
            digitalWrite(ENR, LOW);
            delay(SlowDownDelay);
          }
          else if (LeftAdjustment == 1)  //Go CCW
          {
            analogWrite(ENR, MotorSpeed);
            digitalWrite(EN, LOW);
            delay(20); //Lowering this will make is more precise, but take more time
            digitalWrite(EN, LOW);
            digitalWrite(ENR, LOW);
            delay(SlowDownDelay);
          }
          else if (LeftAdjustment == 2)
          {
            LineUpComplete = true;
            TickL = 0;
            TickR = 0; 
          }
        }
      }
    }
    else if (LeftWall && !RightWall)
    {
      if (LeftError < -10) //Robot is turned counter-clockwise
        LeftAdjustment = 0;
      else if (LeftError > 10)
        LeftAdjustment = 1; 
      else //Robot is straight
      LeftAdjustment = 2;

      Serial.print("LeftAdjustment: ");
      Serial.println(LeftAdjustment);

      if (LeftAdjustment == 0)  //Go CW
      {
        analogWrite(EN, MotorSpeed);
        digitalWrite(ENR, LOW);
        delay(20); //Lowering this will make is more precise, but take more time
        digitalWrite(EN, LOW);
        digitalWrite(ENR, LOW);
        delay(SlowDownDelay);
      }
      else if (LeftAdjustment == 1)  //Go CCW
      {
        analogWrite(ENR, MotorSpeed);
        digitalWrite(EN, LOW);
        delay(20); //Lowering this will make is more precise, but take more time
        digitalWrite(EN, LOW);
        digitalWrite(ENR, LOW);
        delay(SlowDownDelay);
      }
      else if (LeftAdjustment == 2)
      {
        LineUpComplete = true;
        TickL = 0;
        TickR = 0; 
      }
    }
    else if (!LeftWall && RightWall)
    {
      if (RightError < -10)
        RightAdjustment = 1;
      else if (RightError > 10)
        RightAdjustment = 0;
      else
        RightAdjustment = 2;

      Serial.print("RightAdjustment: ");
      Serial.println(RightAdjustment);

      if (RightAdjustment == 0)  //Go CW
      {
        analogWrite(EN, MotorSpeed);
        digitalWrite(ENR, LOW);
        delay(20); //Lowering this will make is more precise, but take more time
        digitalWrite(EN, LOW);
        digitalWrite(ENR, LOW);
        delay(SlowDownDelay);
      }
      else if (RightAdjustment == 1)  //Go CCW
      {
        analogWrite(ENR, MotorSpeed);
        digitalWrite(EN, LOW);
        delay(20); //Lowering this will make is more precise, but take more time
        digitalWrite(EN, LOW);
        digitalWrite(ENR, LOW);
        delay(SlowDownDelay);
      }
      else if (RightAdjustment == 2)
      {
        LineUpComplete = true;
        TickL = 0;
        TickR = 0; 
      }
    }
    else
    {
      LineUpComplete = true;
      TickL = 0;
      TickR = 0;
    }
  }
}

void TurnRight()
{
  //Set motors to turn (right/left)
  digitalWrite(A_1, HIGH);
  digitalWrite(A_2, LOW);
  digitalWrite(A_3, LOW);
  digitalWrite(A_4, HIGH);

  Moving = true;
  DistanceTravelled = 0;
  OriginalDistance = TickL; //Save the current TickL
  analogWrite(ENR, MotorSpeed); //Turn on both motors
  analogWrite(EN, MotorSpeed);
  while (DistanceTravelled < OriginalDistance + 40) //Check if distance travelled is over a threshold
  {
    DistanceTravelled = TickL;
  }
  digitalWrite(ENR, LOW);
  digitalWrite(EN, LOW);

  //Reset motor to normal direction
  digitalWrite(A_1, HIGH);
  digitalWrite(A_2, LOW);
  digitalWrite(A_3, HIGH);
  digitalWrite(A_4, LOW);

  Moving = false;
}

void CorrectStraightness()
{
  if (LineUpComplete == true)
  {
    if (TickR > TickL + 1) //If the right motor is ahead of the left
    {
      digitalWrite(ENR, LOW);      //Turn CW
      analogWrite(EN, MotorSpeed);
    }
    else if (TickL > TickR + 1) //If the left motor is ahead of the right
    {
      digitalWrite(EN, LOW);      //Turn CCW
      analogWrite(ENR, MotorSpeed);
    }
    else
    {
      if (Moving) //Both motors are in sync
      {
        analogWrite(EN, MotorSpeed); //If you were previously moving, stay moving
        analogWrite(ENR, MotorSpeed);
      }
      else
      {
        digitalWrite(EN, LOW); //If you were previously moving, stop moving
        digitalWrite(ENR, LOW);
      }
    }
  }
}

boolean IsCentered()
{
  if ((((IRFrontLeft + IRBackLeft) - (IRFrontRight + IRBackRight)) / 2) > 40 || (((IRFrontRight + IRBackRight) - (IRFrontLeft + IRBackLeft)) / 2) > 40)
    return false;
  else
    return true;
}

void MoveToCenter()
{
  if ((IRFrontLeft + IRBackLeft) < (IRFrontRight + IRBackRight))
  {
    while ((TickL - 20) > TickR)
    {
      analogWrite(EN, MotorSpeed);
      digitalWrite(ENR, LOW);
    }
    TickL = 0;
    TickR = 0;
  }
  else if ((IRFrontLeft + IRBackLeft) > (IRBackRight + IRFrontRight))
  {
    while ((TickR - 20) > TickL)
    {
      analogWrite(ENR, MotorSpeed);
      digitalWrite(EN, LOW);
    }
    TickL = 0;
    TickR = 0;
  }
  digitalWrite(EN, LOW);
  digitalWrite(ENR, LOW);
}

//====================Output Functions===================
void OutputTicks()
{
  Serial.println("==============Ticks==============");
  Serial.print("Left: ");
  Serial.print(TickL);
  Serial.print(" Right: ");
  Serial.println(TickR);
  Serial.println("=================================");
}

void OutputSensors()
{
  Serial.println("==============Sensors==============");
  Serial.print("Front: "); 
  Serial.print(IRFront);
  Serial.print(" Front-Left: "); 
  Serial.print(IRFrontLeft);
  Serial.print(" Front-Right: "); 
  Serial.println(IRFrontRight);
  Serial.print("Back-Left: "); 
  Serial.print(IRBackLeft);
  Serial.print(" Back-Right: "); 
  Serial.println(IRBackRight);
  Serial.println("===================================");
}

void ReadSensors()
{
  IRFront = analogRead(Front);
  IRFrontLeft = analogRead(FrontLeft);
  IRFrontRight = analogRead(FrontRight);
  IRBackLeft = analogRead(BackLeft);
  IRBackRight = analogRead(BackRight);    
}

//====================Interrupts=========================
void TickRInt()
{
  TickR++;
  CorrectStraightness();
  return;
}
void TickLInt()
{
  TickL++;
  CorrectStraightness();
  return;
}
//=======================================================





