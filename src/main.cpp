#include <Arduino.h>
#include <librobus.h>
#include <TimedAction.h>
#include <QTRSensors.h>

#define NUM_SENSORS             8  // number of sensors used
#define NUM_SAMPLES_PER_SENSOR  4  // average 4 analog samples per sensor reading
#define EMITTER_PIN             2  // emitter is controlled by digital pin 2

// sensors 0 through 5 are connected to analog inputs 0 through 5, respectively
QTRSensorsAnalog qtra((unsigned char[]) {0, 1, 2, 3, 4, 5,6,7},
  NUM_SENSORS, NUM_SAMPLES_PER_SENSOR, EMITTER_PIN);
unsigned int sensorValues[NUM_SENSORS];


bool isLine(unsigned int  sensorValues[NUM_SENSORS])
{
  int somme = 0;
  for(int i = 0; i < NUM_SENSORS; i++)
  {
    somme += sensorValues[i];
  }
  return constrain(somme,0,1);
}

int angleLigne()
{
  // read raw sensor values
  qtra.read(sensorValues);

  // print the sensor values as numbers from 0 to 1023, where 0 means maximum reflectance and
  // 1023 means minimum reflectance
  for (unsigned char i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(sensorValues[i]);
    Serial.print('\t'); // tab to format the raw data into columns in the Serial monitor
  }
  for (unsigned char i = 0; i < NUM_SENSORS; i++)
  {
    if(sensorValues[i] < 100)
    {
      sensorValues[i] = 0;
    }
    else sensorValues[i] = 1;

    Serial.print(sensorValues[i]);
    Serial.print('\t'); // tab to format the raw data into columns in the Serial monitor
  }

  if(!isLine(sensorValues)) return 360;


  Serial.println();

  delay(100);
}

void setup()
{
  delay(500);
  Serial.begin(9600); // set the data rate in bits per second for serial data transmission
  delay(1000);
}


void loop()
{
  angleLigne();
}
