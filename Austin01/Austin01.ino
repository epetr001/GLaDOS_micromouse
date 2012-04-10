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
const int MotorEncoderBuffer = 5;
boolean StraightCorrecting = false;
boolean SwerveCorrecting = false;
int LeftTrackFile[5];
int RightTrackFile[5];
int LeftTrackIndex = 0;
int RightTrackIndex = 0;
//===================Sensor Offsets======================
const int FROffset = 0;
const int BROffset = 50;
const int FLOffset = 0;
const int BLOffset = 20;
//===================Sensor Data=========================
int IRserialRF = 0;
int IRserialRB = 0;
int IRserialLF = 0;
int IRserialLB = 0;
int IRserialF = 0;
/* Setup code */
void setup() {
  Serial.begin(9600);
  pinMode(A_1, OUTPUT);
  pinMode(A_2, OUTPUT);
  pinMode(EN, OUTPUT);
  pinMode(ENR, OUTPUT);
  pinMode(A_3, OUTPUT);
  pinMode(A_4, OUTPUT);
  digitalWrite(EN, HIGH);
  digitalWrite(ENR, HIGH);
  attachInterrupt(0,TickRInt, CHANGE);
  attachInterrupt(1,TickLInt, CHANGE);
  digitalWrite(A_1, HIGH);
  digitalWrite(A_2, LOW);
  digitalWrite(A_3, HIGH);
  digitalWrite(A_4, LOW);
}
void loop()
{   
  IRserialF = analogRead(Front);
  IRserialRF = analogRead(FrontRight);
  IRserialRB = analogRead(BackRight);
  IRserialLF = analogRead(FrontLeft);
  IRserialLB = analogRead(BackLeft);
  
  Serial.print("F: "); Serial.print(IRserialF); Serial.println();
  Serial.print("RF: "); Serial.print(IRserialRF); Serial.println();
  Serial.print("LF: "); Serial.print(IRserialLF); Serial.println();
  Serial.print("RB: "); Serial.print(IRserialRB); Serial.println();
  Serial.print("LB: "); Serial.print(IRserialLB); Serial.print("\n");
  delay(250);

/*
            LF    RF    LB    RB
  Empty     971   972   971   972
  Centered  899   858   818   820
  Left Wall 529   935   410   920
  Right Wall941   543   908   336
*/
  
  return;
  
  if(IRserialF < 800)
  {
    TurnRight();
  }
  
  if (SwerveCorrecting == false)
  {
    if (TickR > TickL + 5)
    {
      digitalWrite(ENR, LOW);
      digitalWrite(EN, HIGH);
      StraightCorrecting = true;
    }
    else if (TickL > TickR + 5)
    {
      digitalWrite(EN, LOW);
      digitalWrite(ENR, HIGH);
      StraightCorrecting = true;
    }
    else
    {
      digitalWrite(EN, HIGH);
      digitalWrite(ENR, HIGH);
      StraightCorrecting = false;
    }
  }
  
  if (((IRserialRB + BROffset) - (IRserialRF + FROffset)) > 50)  //Check if the mouse is swerving to the right
  {
    digitalWrite(EN, LOW);
    digitalWrite(ENR, HIGH);
    SwerveCorrecting = true;
  }
  else if (((IRserialLB + BLOffset) - (IRserialLF + FLOffset)) > 50)
  {
    digitalWrite(EN, HIGH);
    digitalWrite(ENR, LOW);
    SwerveCorrecting = true;
  }
  else
  {
    if (StraightCorrecting == false)
    {
      digitalWrite(EN, HIGH);
      digitalWrite(ENR, HIGH);
    }
    if (SwerveCorrecting == true)
    {
      TickR = 0;
      TickL = 0; 
    }
    SwerveCorrecting = false;
  }
  
  
    //Store Running Sensor Track Files
  LeftTrackFile[LeftTrackIndex] = IRserialLF;
  LeftTrackIndex++;
  if (LeftTrackIndex = 5)
    LeftTrackIndex = 0;
  
  RightTrackFile[RightTrackIndex] = IRserialRF;
  RightTrackIndex++;
  if (RightTrackIndex = 5)
    RightTrackIndex = 0;
    
  if (SwerveCorrecting == false)
  {
     
  }
}
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
void TurnRight()
{
    digitalWrite(EN,LOW);
    digitalWrite(ENR,LOW);
    
    digitalWrite(A_3,LOW);
    digitalWrite(A_4,HIGH);
    
    digitalWrite(EN,HIGH);
    digitalWrite(ENR,HIGH);
    delay(740);
    digitalWrite(A_3,HIGH);
    digitalWrite(A_4,LOW);
}

