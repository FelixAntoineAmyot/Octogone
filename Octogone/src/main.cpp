#include <Arduino.h>
#include <librobus.h>
#include <TimedAction.h>
#include <QTRSensors.h>
#include <math.h>

#define NUM_SENSORS             8  // number of sensors used
#define NUM_SAMPLES_PER_SENSOR  4  // average 4 analog samples per sensor reading
#define EMITTER_PIN             2  // emitter is controlled by digital pin 2

// sensors 0 through 5 are connected to analog inputs 0 through 5, respectively
QTRSensorsAnalog qtra((unsigned char[]) {0, 1, 2, 3, 4, 5,6,7},
  NUM_SENSORS, NUM_SAMPLES_PER_SENSOR, EMITTER_PIN);
unsigned int sensorValues[NUM_SENSORS];


void sifflet()
{
  int khz5 = analogRead(A8);
  /*
  Serial.println(khz5);
  if (khz5 > 500)
  {
    MOTOR_SetSpeed(0,0);
    MOTOR_SetSpeed(1,0);
    Serial.println("STOP");
    delay(10000);
  }*/
}

TimedAction threadSon = TimedAction(1300,sifflet);

int trouveLigne()
{
  int moyenne = 0;
  int nbSensorsActif = 0;
  //read raw sensor values
  qtra.read(sensorValues);

  // print the sensor values as numbers from 0 to 1023, where 0 means maximum reflectance and
  // 1023 means minimum reflectance
  for (unsigned char i = 0; i < NUM_SENSORS; i++)
  {
    threadSon.check();
    
    
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
    Serial.print(sensorValues[i]);
    Serial.print('\t'); // tab to format the raw data into columns in the Serial monitor
  }
  Serial.println();
  if (nbSensorsActif != 0)
  {
    return moyenne / nbSensorsActif - 450;
  }
  else return 0;
}

void followLine(float ligne,float vitesse = 0.5)
{
  float correction = ligne/10000;
  MOTOR_SetSpeed(0, vitesse);
  MOTOR_SetSpeed(1,vitesse + correction);
}
void setup()
{
  Serial.begin(9600);
}


void loop()
{
  threadSon.check();
  Serial.println(trouveLigne());
  
  delay(250);
}
