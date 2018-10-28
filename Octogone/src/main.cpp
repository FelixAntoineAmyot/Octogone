#include <Arduino.h>
#include <librobus.h>
#include <TimedAction.h>
#include <QTRSensors.h>
#include <math.h>

#define NUM_SENSORS             8  // number of sensors used
#define NUM_SAMPLES_PER_SENSOR  4  // average 4 analog samples per sensor reading
#define EMITTER_PIN             2  // emitter is controlled by digital pin 2

#define SCALE 28

#define LIMIT_LOW 10.0
#define LIMIT_HIGH 60.0
#define TOLERANCE 0.05
#define OFFSET 0.007
// sensors 0 through 5 are connected to analog inputs 0 through 5, respectively
QTRSensorsAnalog qtra((unsigned char[]) {2, 3, 4, 5, 6, 7,8,9},
  NUM_SENSORS, NUM_SAMPLES_PER_SENSOR, EMITTER_PIN);
unsigned int sensorValues[NUM_SENSORS];
int prevLigne[5];
int prevMur = 0;

float IR_distance(int id)
{
  float volts = ROBUS_ReadIR(id)*0.0048828125;  // value from sensor * (5/1024)
  float distance = SCALE*pow(volts, -1); // worked out from datasheet graph
  //  Serial.println(distance);   // print the distance
  return distance;
}
void sifflet()
{
  int khz5 = analogRead(A10);
  Serial.println(khz5);
  if (khz5 > 390)
  {
    MOTOR_SetSpeed(0,0);
    MOTOR_SetSpeed(1,0);
    Serial.println("STOP");
    delay(10000);
  }
}
void mur()
{
  float distHaut = IR_distance(1);
  if(distHaut < 30)
  {
    if(prevMur > 5)
    {
      MOTOR_SetSpeed(0,-.5);
      MOTOR_SetSpeed(1,-.5);
      delay(500);
    }
    MOTOR_SetSpeed(0,.5);
    MOTOR_SetSpeed(1,-.5);
    delay(500);
    MOTOR_SetSpeed(0,0);
    MOTOR_SetSpeed(1,0);
    prevMur +=1;
  }
  else prevMur = 0;
}

TimedAction threadSon = TimedAction(200,sifflet);
TimedAction threadCollision = TimedAction(100,mur);


int trouveLigne()
{
  int moyenne = 0;
  int nbSensorsActif = 0;
  qtra.read(sensorValues);
  for (unsigned char i = 0; i < NUM_SENSORS; i++)
  {
    threadSon.check();
    threadCollision.check();
    
    if(sensorValues[i] > 300)
    {
      sensorValues[i] = 100 * (i+1);
      nbSensorsActif += 1;
      moyenne += sensorValues[i];
    }
    else
    {
      sensorValues[i] = 0;
    }
    ///Serial.print(sensorValues[i]);
    //Serial.print('\t'); // tab to format the raw data into columns in the Serial monitor
  }
  //Serial.println();
  if (nbSensorsActif != 0)
  {
    if(nbSensorsActif == 8)
    {
      return 1;
    }
    else return moyenne / nbSensorsActif;
  }
  else return 0;
}

void followLine(float ligne,float vitesse = 0.5)
{
  float sum = 0;
  float nbMem = 0;
  float moyenne;
  for(int i = 0; i < 5 ;i++)
  {
    threadSon.check();
    threadCollision.check();
    prevLigne[i+1] = prevLigne[i];
  }
  prevLigne[0] = ligne;
  for(int i = 0; i < 5 ;i++)
  {
    threadSon.check();
    threadCollision.check();
    if (prevLigne[i] != 0)
    {
      sum += prevLigne[i];
      nbMem += 1;
    }
  }
  if (nbMem > 0)
  {
    moyenne = sum/nbMem;
  }
  float erreur = prevLigne[0] - moyenne;
  float correction = erreur / 500;
  MOTOR_SetSpeed(0,vitesse + correction);
  MOTOR_SetSpeed(1,vitesse - correction);

}
void chercheBalle()
{
  float distBas = IR_distance(0);
  float distHaut = IR_distance(1);
  float diff = distHaut - distBas;
  if (diff > 30)
  {
    if(distBas < 15)
    {
      
      SERVO_SetAngle(0,130);
      delay(100);
      SERVO_SetAngle(0,90);
      delay(100);

    }
    MOTOR_SetSpeed(0,.5);
    MOTOR_SetSpeed(1,.5);
  }
  else
  {
    float ligne = trouveLigne();
    followLine(ligne,.4);
  }
}
void setup()
{
  BoardInit();
 
  Serial.begin(9600);
  SERVO_Enable(0);
  while(!ROBUS_IsBumper(3))
  {
    delay(10);
  }
  delay(5000);
}


void loop()
{
  threadSon.check();
  threadCollision.check();

  chercheBalle();
  

  delay(50);
}
