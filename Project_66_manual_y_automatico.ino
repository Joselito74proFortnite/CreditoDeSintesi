//*******************************************************************************
#define SCL_Pin  A5  //Set the clock pin to A5
#define SDA_Pin  A4  //Set data pin to A4

//Array, used to store the data of pattern, can be calculated by yourself or obtained from the modulus tool

unsigned char start01[] = {0x02, 0x3e, 0x02, 0x3e, 0x2a, 0x22, 0x02, 0x3e, 0x02, 0x3e, 0x2a, 0x22, 0x32, 0x0c, 0x0c, 0x32};
unsigned char efront[] = {0x02, 0x3e, 0x02, 0x3e, 0x2a, 0x22, 0x02, 0x3e, 0x02, 0x3e, 0x2a, 0x22, 0x32, 0x0c, 0x0c, 0x32};
unsigned char eback[] = {0x02, 0x3e, 0x02, 0x3e, 0x2a, 0x22, 0x02, 0x3e, 0x02, 0x3e, 0x2a, 0x22, 0x32, 0x0c, 0x0c, 0x32};
unsigned char eleft[] = {0x02, 0x3e, 0x02, 0x3e, 0x2a, 0x22, 0x02, 0x3e, 0x02, 0x3e, 0x2a, 0x22, 0x32, 0x0c, 0x0c, 0x32};
unsigned char eright[] = {0x02, 0x3e, 0x02, 0x3e, 0x2a, 0x22, 0x02, 0x3e, 0x02, 0x3e, 0x2a, 0x22, 0x32, 0x0c, 0x0c, 0x32};
unsigned char STOP01[] = {0x7c, 0x12, 0x12, 0x12, 0x7c, 0x7e, 0x04, 0x08, 0x04, 0x7e, 0x7c, 0x12, 0x12, 0x7c, 0x00, 0x7e};
unsigned char clear[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

#include <Arduino.h>
#include <IRremote.h>//function library of IR remote control
int RECV_PIN = 3;//set the pin of IR receiver to D3
IRrecv irrecv(RECV_PIN);
long irr_val;
decode_results results;

int left_ctrl = 2;//define the direction control pins of group B motor
int left_pwm = 5;//define the PWM control pins of group B motor
int right_ctrl = 4;//define the direction control pins of group A motor
int right_pwm = 6;//define the PWM control pins of group A motor

//sigue lineas

#define ML_Ctrl 4     //define direction control pin of B motor
#define ML_PWM 5   //define PWM control pin of B motor
#define MR_Ctrl 2    //define direction control pin of A motor
#define MR_PWM 9   //define PWM control pin of A motor
int sensor_L = 11;//define the pin of left line tracking sensor
int sensor_M = 7;//define the pin of middle line tracking sensor
int sensor_R = 8;//define the pin of right line tracking sensor
int L_val,M_val,R_val;//define these variables


#include <Servo.h>
Servo servo_A3;//set the pin of servo to A3 

unsigned char data_line = 0;
unsigned char delay_count = 0;

int mode = 0; 

#include "SR04.h"//define the library of ultrasonic sensor
#define TRIG_PIN 12// set the signal output of ultrasonic sensor to D12 
#define ECHO_PIN 13//set the signal input of ultrasonic sensor to D13 
SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN);
long distance,a1,a2;//define three distance
const int servopin = A3;//set the pin of servo to A3 



void setup() {
  Serial.begin(9600);//
  // In case the interrupt driver crashes on setup, give a clue
  // to the user what's going on.
  Serial.println("Enabling IRin");
  irrecv.enableIRIn(); // Start the receiver
  Serial.println("Enabled IRin");
  pinMode(left_ctrl,OUTPUT);//set direction control pins of group B motor to OUTPUT
  pinMode(left_pwm,OUTPUT);//set PWM control pins of group B motor to OUTPUT
  pinMode(right_ctrl,OUTPUT);//set direction control pins of group A motor to OUTPUT
  pinMode(right_pwm,OUTPUT);//set PWM control pins of group A motor to OUTPUT
  servo_A3.attach(A3);
  servo_A3.write(90);//the angle of servo is 90 degree
  delay(300);
  pinMode(SCL_Pin,OUTPUT);// Set the clock pin to output
  pinMode(SDA_Pin,OUTPUT);//Set the data pin to output
  matrix_display(clear);
  matrix_display(start01); //display start01 expression pattern

  //SIGUELINEAS
  pinMode(sensor_L,INPUT);//set the pins of left line tracking sensor to INPUT
  pinMode(sensor_M,INPUT);//set the pins of middle line tracking sensor to INPUT
  pinMode(sensor_R,INPUT);//set the pins of right line tracking sensor to INPUT
  
}

void loop() {

  
  if (irrecv.decode(&results)) {
    irr_val = results.value;
    Serial.println("BOTON:");
    Serial.println(irr_val, HEX);//serial prints the read IR remote signals 


    
    switch(irr_val)
    {

// * FF42BD
// # FF52AD

      case 0xFF42BD : mode = 1;
            // AUTO
            break;
  
      case 0xFF52AD : mode = 0;
            // MANUAL
            break;

      case 0xFF6897 : mode = 2;
            // SIGUELINEAS
            break;
        
      case 0xFF629D : car_front(); //Recive 0xFF629D, el coche va hacia delante
      matrix_display(clear);
      matrix_display(efront);   
      break;
      
      case 0xFFA857 : car_back(); //Receive 0xFFA857, el coche va para atras
      matrix_display(clear);
      matrix_display(eback); 
      break;

      case 0xFF22DD : car_left(); //Receive 0xFF22DD,el coche gira hacia la izquierda
      matrix_display(clear);
      matrix_display(eleft); 
      break;
     
      case 0xFFC23D : car_right();//Receive 0xFFC23D,el coche gira hacia la derecha
      matrix_display(clear);
      matrix_display(eright);  
      break;
     
      case 0xFF02FD : car_Stop();//Receive 0xFF02FD,el coche para
      matrix_display(clear);
      matrix_display(STOP01); 
      break;
    }
        irrecv.resume(); // recive el siguiente valor
  }

  
   if (mode == 1) {
      //automatico
      avoid(); //inicia el control automatico
   } 

   if (mode == 2) {
    // siguelineas
    tracking(); //INICIA EL TRACKING
   }
}


void avoid()
{
  distance=sr04.Distance(); //obtain the value detected by ultrasonic sensor 

  if((distance < 20)&&(distance != 0))//if the distance is greater than 0 and less than 10  

  {
    car_Stop();//stop
    matrix_display(clear);
    matrix_display(STOP01);//en la pantalla sale que para
    delay(1000);
    servopulse(servopin,160);//el servo gira 160°
    delay(500);
    a1=sr04.Distance();//mide la distancia
    delay(100);
    servopulse(servopin,20);//rotate to 20 degree
    delay(500);
    a2=sr04.Distance();//mide la distancia
    delay(100);
    servopulse(servopin,90);  //Return to the 90 degree position
    delay(500);
    if(a1 > a2)//compara la distancia, si la distancia de la izquierda es mas que la de la derecha
    {
      car_left();//gira hacia la izquierda
      matrix_display(clear);
      matrix_display(eleft);    //display left-turning pattern
      servopulse(servopin,90); //servo gira 90 grados
      delay(700); //turn left 700ms
      matrix_display(clear);
      matrix_display(efront);  //show forward pattern
    }
    else//if the right distance is greater than the left
    {
      car_right();//turn right
      matrix_display(clear);
      matrix_display(eright);  //display right-turning pattern
      servopulse(servopin,90);//servo rotates to 90 degree
      delay(700);
      matrix_display(clear);
      matrix_display(efront);  //show forward pattern
    }
  }
  else //otherwise
  {
    car_front();//va recto
    matrix_display(clear);
    matrix_display(efront);  // show forward pattern
  }
}

void car_front() //define que va hacia delante
{
  digitalWrite(left_ctrl,HIGH); //hacia delante
  analogWrite(left_pwm,105); //velocidad
  digitalWrite(right_ctrl,HIGH);
  analogWrite(right_pwm,105);
}
void car_back() //define que va hacia detras
{
  digitalWrite(left_ctrl,LOW);
  analogWrite(left_pwm,150);
  digitalWrite(right_ctrl,LOW);
  analogWrite(right_pwm,150);
}
void car_left() //define que el coche gira hacia la izquierda
{
  digitalWrite(left_ctrl, LOW);
  analogWrite(left_pwm, 100);  
  digitalWrite(right_ctrl, HIGH);
  analogWrite(right_pwm, 155);
}
void car_right() //define que el coche gira hacia la derecha
{
  digitalWrite(left_ctrl, HIGH);
  analogWrite(left_pwm, 155);
  digitalWrite(right_ctrl, LOW);
  analogWrite(right_pwm, 100);
}
void car_Stop() //con esto defines que el coche pare
{
  digitalWrite(left_ctrl,LOW);
  analogWrite(left_pwm,0);
  digitalWrite(right_ctrl,LOW);
  analogWrite(right_pwm,0);
}

//this function is used for dot matrix display

void matrix_display(unsigned char matrix_value[])
{
  IIC_start();  //the function that calls the data transfer start condition
  IIC_send(0xc0);  //select address

  for (int i = 0; i < 16; i++) //the pattern data is 16 bytes
  {
    IIC_send(matrix_value[i]); //Transmit the data of the pattern
  }
  IIC_end();   //End pattern data transmission
  IIC_start();
  IIC_send(0x8A);  //Display control, select 4/16 pulse width
  IIC_end();
}
//Conditions under which data transmission begins

void IIC_start()
{
  digitalWrite(SDA_Pin, HIGH);
  digitalWrite(SCL_Pin, HIGH);
  delayMicroseconds(3);
  digitalWrite(SDA_Pin, LOW);
  delayMicroseconds(3);
  digitalWrite(SCL_Pin, LOW);
}

//Indicates the end of data transmission

void IIC_end()
{
  digitalWrite(SCL_Pin, LOW);
  digitalWrite(SDA_Pin, LOW);
  delayMicroseconds(3);
  digitalWrite(SCL_Pin, HIGH);
  delayMicroseconds(3);
  digitalWrite(SDA_Pin, HIGH);
  delayMicroseconds(3);
}

//transmit data

void IIC_send(unsigned char send_data)
{
  for (byte mask = 0x01; mask != 0; mask <<= 1) //Each byte has 8 bits and is checked bit by bit starting at the lowest level
  {
    if (send_data & mask) { //Sets the high and low levels of SDA_Pin depending on whether each bit of the byte is a 1 or a 0
      digitalWrite(SDA_Pin, HIGH);
    } else {
      digitalWrite(SDA_Pin, LOW);
    }
    delayMicroseconds(3);
    digitalWrite(SCL_Pin, HIGH); //Pull the clock pin SCL_Pin high to stop data transmission
    delayMicroseconds(3);
    digitalWrite(SCL_Pin, LOW); //pull the clock pin SCL_Pin low to change the SIGNAL of SDA 
  }
}

//FF6897 es el numero 1
//FF9867 es el numero 2


//*******************************************************************************


void servopulse(int servopin,int myangle)//the running angle of servo
{
  for(int i=0; i<20; i++)
  {
    int pulsewidth = (myangle*11)+500;
    digitalWrite(servopin,HIGH);
    delayMicroseconds(pulsewidth);
    digitalWrite(servopin,LOW);
    delay(20-pulsewidth/1000);
  } 
}

///codigo siguelineas

void tracking()
{
  L_val = digitalRead(sensor_L); //lee el valor del sensor de movimiento izquierdo
  M_val = digitalRead(sensor_M); //lee el valor del sensor de movimiento del medio
  R_val = digitalRead(sensor_R); //lee el valor del sensor de movimiento derecho

  if(M_val == 1){// si el estado del del medio es 1, que significa que detecta la linea negra

     if (L_val == 1 && R_val == 0) { //si la linea negra se detecta en la izquierda, pero no en la derecha, gira hacia la izquierda
        left();
    }
     else if (L_val == 0 && R_val == 1) { //por otra parte, si la linea negra se detecta en la derecha, pero no en la izquierda, gira hacia la derecha
      right();
    }
     else { //por otra parte, va recto
      front();
    }
  }
  else { //No black lines detected in the middle
    if (L_val == 1 && R_val == 0) { //If a black line is detected on the left, but not on the right, turn left
      left();
    }
    else if (L_val == 0 && R_val == 1) { //Otherwise, if a black line is detected on the right and not on the left, turn right
      right();
    }
    else { //Otherwise,stop
      Stop();
    }
  }
}
void front()//define the status of going forward
{
  digitalWrite(left_ctrl,HIGH);
  analogWrite(left_pwm,155);
  digitalWrite(right_ctrl,HIGH);
  analogWrite(right_pwm,155);
}
void back()//define the state of going back
{
  digitalWrite(left_ctrl,LOW);
  analogWrite(left_pwm,100);
  digitalWrite(right_ctrl,LOW);
  analogWrite(right_pwm,100);
}
void left()//define the left-turning state
{
  digitalWrite(left_ctrl, LOW);
  analogWrite(left_pwm, 100);  
  digitalWrite(right_ctrl, HIGH);
  analogWrite(right_pwm, 155);
}
void right()//define the right-turning state
{
  digitalWrite(left_ctrl, HIGH);
  analogWrite(left_pwm, 155);
  digitalWrite(right_ctrl, LOW);
  analogWrite(right_pwm, 100);
}
void Stop()//define the state of stop
{
  digitalWrite(left_ctrl, LOW);
  analogWrite(left_pwm,0);
  digitalWrite(right_ctrl, LOW);
  analogWrite(right_pwm,0);
}
//*********************************************************
