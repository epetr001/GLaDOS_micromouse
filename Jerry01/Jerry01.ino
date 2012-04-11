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
const int MAXPOWER = 50;

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
  analogWrite(EN, MAXPOWER);
  analogWrite(ENR, MAXPOWER);
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
  if (IRserialF < 800)
  {
    TurnRight();
    
  }
  
  if (SwerveCorrecting == false)
  {
    if (TickR > TickL)
    {
      digitalWrite(ENR, LOW);
      analogWrite(EN, MAXPOWER);
      StraightCorrecting = true;
    }
    else if (TickL > TickR)
    {
      digitalWrite(EN, LOW);
      analogWrite(ENR, MAXPOWER);
      StraightCorrecting = true;
    }
    else
    {
      analogWrite(EN, MAXPOWER);
      analogWrite(ENR, MAXPOWER);
      StraightCorrecting = false;
    }
  }

/*
  if (((IRserialRB + BROffset) - (IRserialRF + FROffset)) > 50)  //Check if the mouse is swerving to the right
  {
    digitalWrite(EN, LOW);
    analogWrite(ENR, MAXPOWER);
    SwerveCorrecting = true;
  }
  else if (((IRserialLB + BLOffset) - (IRserialLF + FLOffset)) > 50)
  {
    analogWrite(EN, MAXPOWER);
    digitalWrite(ENR, LOW);
    SwerveCorrecting = true;
  }
  else
  {
    if (StraightCorrecting == false)
    {
      analogWrite(EN, MAXPOWER);
      analogWrite(ENR, MAXPOWER);
    }
    if (SwerveCorrecting == true)
    {
      TickR = 0;
      TickL = 0; 
    }
    SwerveCorrecting = false;
  }
  */
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
void ScootLeft()
{
   analogWrite(ENR, MAXPOWER);
   digitalWrite(EN, LOW);
   delay(200);
   TickR = 0;
   TickL = 0; 
}
void ScootRight()
{
   digitalWrite(ENR, LOW);
   analogWrite(EN, MAXPOWER);
   delay(200);
   TickR = 0;
   TickL = 0; 
}
void TurnRight()
{   
    digitalWrite(EN,LOW);
    digitalWrite(ENR,LOW);
    delay(60);
    TickR = 0;
    TickL = 0;
    
    Serial.print("L: ");
    Serial.println(TickL);
    Serial.print("R: ");
    Serial.println(TickR);
    while(TickR < 90 && TickL < 85)
    {
      if(TickR >= 90){
         digitalWrite(A_3,HIGH);
         digitalWrite(A_4,HIGH);}
      else{
        digitalWrite(A_3,LOW);
        digitalWrite(A_4,HIGH);
        analogWrite(ENR, MAXPOWER);}
      
      if(TickL >= 85){
        digitalWrite(A_2,HIGH);
        digitalWrite(A_1,HIGH);}
      else{
        digitalWrite(A_2,LOW);
        digitalWrite(A_1,HIGH);
        analogWrite(EN, MAXPOWER);}
    } 
    delay(60);
    TickR = 0;
    TickL = 0;
    digitalWrite(A_1, HIGH);
    digitalWrite(A_2, LOW);
    digitalWrite(A_3, HIGH);
    digitalWrite(A_4, LOW);
   
}
