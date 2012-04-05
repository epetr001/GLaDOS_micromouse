volatile int TickR = 0;
volatile int TickL = 0;
const int A_1 = 7;
const int A_2 = 6;
const int A_3 = 5;
const int A_4 = 4;
const int EN = 9;
const int ENR = 10;
const int MotorEncoderBuffer = 5;

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
  Serial.print("R: ");
  Serial.print(TickR);
  Serial.print("\n");
  Serial.print("L: ");
  Serial.print(TickL);
  Serial.print("\n");
  
  if (TickR > TickL + MotorEncoderBuffer)  //Right is ahead by over 5
  {
    digitalWrite(ENR, LOW);
  }
  else if (TickL > TickR + MotorEncoderBuffer)  //Left is ahead by over 5
  {
    digitalWrite(EN, LOW);
  }
  else    //Both Encoders are within 5 counts of each other. Enable both motors
  {
    digitalWrite(EN, HIGH);
    digitalWrite(ENR, HIGH); 
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
