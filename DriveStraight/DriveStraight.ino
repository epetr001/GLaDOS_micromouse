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
int SquareLength = 160; //The number of ticks representing 1 maze square
int SlowDownDelay = 60; //This delay makes it so that the motors are completely stopped before doing something else (needs tweaking)
int RightMotorDelay = 150; //This delay makes it so the motors stop together (needs tweaking) <- is there a better way of doing this?
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
  if(IsCentered())
  {
    LineUp(); //First line up
    delay(1000);
    TravelASquare(); //Then move 1 square
    delay(1000);
  }
  else
  {
    MoveToCenter();
    delay(1000);
    TravelASquare();
    delay(1000);
    LineUp();
    delay(1000);
  }
}

/*void TravelASquare()    //This only works if the ticks are reset before being called. (use later is current function breaks program)
{
 
  Moving = true;
  DistanceTravelled = 0;
  analogWrite(ENR, MotorSpeed); //Turn on both motors
  analogWrite(EN, MotorSpeed);
  while (DistanceTravelled < SquareLength)
  {
    DistanceTravelled = (TickL + TickR) / 2; //This only works once!!!!!!!!!!!!!!!!!!!! needs to be changed to work multiple times!
  }
  //OutputTicks();
  digitalWrite(ENR, LOW);
  digitalWrite(EN, LOW);
  Moving = false;
}*/

void TravelASquare()
{
  Moving = true;
  int OriginalDistance = TickL;
  analogWrite(ENR, MotorSpeed); //Turn on both motors
  analogWrite(EN, MotorSpeed);
  while (DistanceTravelled < OriginalDistance + SquareLength)  //Check if distance travelled is over a threshold
  {
    DistanceTravelled = TickL;
  }
  OutputTicks();
  digitalWrite(ENR, LOW);
  digitalWrite(EN, LOW);
  Moving = false;
}

void OutputTicks()
{
  Serial.print("L: ");
  Serial.println(TickL);
  Serial.print("R: ");
  Serial.println(TickR);
}

void LineUp()
{
  LineUpComplete = false;
  
  //Add these numbers to the front sensor value and you should get the back sensor value IF THE SIDE IS STRAIGHT
  int RightStraightOffset = -50;
  int LeftStraightOffset = -70;

  while (!LineUpComplete)
  {
    IRFront = analogRead(Front);
    IRFrontRight = analogRead(FrontRight);
    IRBackRight = analogRead(BackRight);
    IRFrontLeft = analogRead(FrontLeft);
    IRBackLeft = analogRead(BackLeft);

    int LeftError = (IRFrontLeft + LeftStraightOffset - IRBackLeft);
    int LeftAverage = (IRFrontLeft + LeftStraightOffset + IRBackLeft)/2;
    int RightError = (IRFrontRight + RightStraightOffset - IRBackRight);
    Serial.println(LeftAverage);
    Serial.println(LeftError);
   if (LeftAverage < 800)
   {
     
    if (LeftError < -10) //Robot is turned counter-clockwise
    {
      
     Serial.println("Left Error: ");
      Serial.print(LeftError);
      Serial.println("Turned CCW, Rotate CW");
      analogWrite(EN, MotorSpeed);
      digitalWrite(ENR, LOW);
      delay(10); //Lowering this will make is more precise, but take more time
      digitalWrite(EN, LOW);
      digitalWrite(ENR, LOW);
      delay(SlowDownDelay);
    }
    else if ((LeftError > 10) ) //Robot is turned clockwise
    {
      Serial.println("Left Error: ");
      Serial.print(LeftError);
      Serial.println("Turned CW, Rotate CCW");
      digitalWrite(EN, LOW);
      analogWrite(ENR, MotorSpeed);
      delay(10); //Lowering this will make is more precise, but take more time
      digitalWrite(EN, LOW);
      digitalWrite(ENR, LOW);
      delay(SlowDownDelay);
    }
    else //Robot is straight
    {
      Serial.println("Robot is straight.");
      LineUpComplete = true;
      TickR = 0;//JERRY: THIS LINE ALLOWS THE ROBOT TO CONTINUE CORRECTING AND MOVING
      TickL = 0;
    }
   }//END OF OUTER IF
   else
   {
         if (RightError > 10) //Robot is turned counter-clockwise
    {
      Serial.println("Right Error: ");
      Serial.print(RightError);
      Serial.println("Turned CCW, Rotate CW");
      analogWrite(EN, MotorSpeed);
      digitalWrite(ENR, LOW);
      delay(10); //Lowering this will make is more precise, but take more time
      digitalWrite(EN, LOW);
      digitalWrite(ENR, LOW);
      delay(SlowDownDelay);
    }
    else if (RightError < -10) //Robot is turned clockwise
    {
      Serial.println("Right Error: ");
      Serial.print(RightError);
      Serial.println("Turned CW, Rotate CCW");
      digitalWrite(EN, LOW);
      analogWrite(ENR, MotorSpeed);
      delay(10); //Lowering this will make is more precise, but take more time
      digitalWrite(EN, LOW);
      digitalWrite(ENR, LOW);
      delay(SlowDownDelay);
    }
    else //Robot is straight
    {
      Serial.println("Robot is straight.");
      LineUpComplete = true;
      TickR = 0;//JERRY: THIS LINE ALLOWS THE ROBOT TO CONTINUE CORRECTING AND MOVING
      TickL = 0;
    } 
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
  int OriginalDistance = TickL;  //Save the current TickL
  analogWrite(ENR, MotorSpeed); //Turn on both motors
  analogWrite(EN, MotorSpeed);
  while (DistanceTravelled < OriginalDistance + 40)    //Check if distance travelled is over a threshold
  {
    DistanceTravelled = TickL;
  }
  OutputTicks();
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
    digitalWrite(ENR, LOW);
    analogWrite(EN, MotorSpeed);
    return;
}
 else if (TickL > TickR + 1) //If the left motor is ahead of the right
  {
    digitalWrite(EN, LOW);
    analogWrite(ENR, MotorSpeed);
    return;
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
    return;
  } 
  }
else 
{return;}
}

boolean IsCentered()
{
  if( (((IRFrontLeft + IRBackLeft) - (IRFrontRight + IRBackRight))/2) > 40 || (((IRFrontRight + IRBackRight) - (IRFrontLeft + IRBackLeft))/2) > 40)
 {
   return false;
 }
else
   return true;
}

void MoveToCenter()
{
  if((IRFrontLeft + IRBackLeft) < (IRFrontRight + IRBackRight))
 {
   while((TickL - 20)> TickR)
   {
     analogWrite(EN,MotorSpeed);
     digitalWrite(ENR,LOW); 
   }
   
   TickL = 0;
   TickR = 0;
   
 } 
 else if((IRFrontLeft + IRBackLeft) > (IRBackRight+ IRFrontRight))
 {
   while((TickR - 20) > TickL)
   {
     analogWrite(ENR,MotorSpeed);
     digitalWrite(EN,LOW);  
   }
   TickL = 0;
   TickR = 0;
 } 
digitalWrite(EN, LOW);
digitalWrite(ENR, LOW);
  
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
