#include <Arduino.h>
#include <librobus.h>
#include <TimedAction.h>
#include <QTRSensors.h>
#include <math.h>
#include <Time.h>
#define pi 3.14159265359
#define diameter .076
#define nb_pulse_tour 3200


void sifflet()
{
  
  int khz5 = analogRead(A1);
  Serial.println(khz5);
  if (khz5 > 600    )
  {
    MOTOR_SetSpeed(0,0);
    MOTOR_SetSpeed(1,0);
    Serial.println("STOP");
    delay(10000);
  }
}

TimedAction threadSon = TimedAction(100,sifflet);


void straight_line_func(float v1, float distance)
{
  float v2 = v1;
  float pulse_att; //pulse moteur droit
  float pulse_recu = ENCODER_Read(0);; //pulse réel du moteur gauche
 

  float kplus = .0002; //facteur de correction positif
  float kmoins = .00001; //facteur de correction negatif
  float nb_tour = .8; //nombres de tours de roues avant prise de mesure

  float t=.08;
  threadSon.check();
  ENCODER_Reset(0); //reset de l'encodeur gauche
  ENCODER_Reset(1); //reset de l'encodeur droit

  double distance_parcourue=0;
  for(int t=0; t<=10; t++)
      {
        threadSon.check();
        float vit = v1*t/10;
        MOTOR_SetSpeed(0, vit); //Set la vitesse moteur gauche à v1(.5)
        MOTOR_SetSpeed(1, vit); //Set la vitesse moteur droit à v2(.5)
        delay(10);
      }
  if (v1<0)
  {
    while(distance_parcourue >= distance)
    {
      
      delay(80); //délai .08 secondes
      threadSon.check();
      pulse_recu = ENCODER_Read(0); //désignation variable pulse recu à la lecture de l'encodeur gauche
      pulse_att = ENCODER_Read(1); //désignation variable pulse att à la lecture de l'encodeur droit
      distance_parcourue = pi*diameter*pulse_recu/3200; 
      //Serial.println(pulse_recu);
      float erreur_totale = (nb_tour*pulse_att)-pulse_recu;
      float erreur_vitesse = (pulse_recu-pulse_att)/t;

      float valeur_ajoute = (kmoins*erreur_totale)+(erreur_vitesse*kplus);
      v2 = (v1 + valeur_ajoute);
      

      //Serial.println(valeur_ajoute);
    }
  }
  else
  {
    while(distance_parcourue <= distance)
    {
      threadSon.check();
      MOTOR_SetSpeed(0, v1); //Set la vitesse moteur gauche à v1(.5)
      MOTOR_SetSpeed(1, v2); //Set la vitesse moteur droit à v2(.5)

      delay(80); //délai .08 secondes

      pulse_recu = ENCODER_Read(0); //désignation variable pulse recu à la lecture de l'encodeur gauche
      pulse_att = ENCODER_Read(1); //désignation variable pulse att à la lecture de l'encodeur droit
      distance_parcourue = pi*diameter*pulse_recu/3200; 
     // Serial.println(pulse_recu);
      float erreur_totale = (nb_tour*pulse_att)-pulse_recu;
      float erreur_vitesse = (pulse_recu-pulse_att)/t;

      float valeur_ajoute = (kmoins*erreur_totale)+(erreur_vitesse*kplus);
      v2 = (v1 + valeur_ajoute);

      //Serial.println(valeur_ajoute);
    }
  }
  for(int t=10; t>=0; t--)
      {
        float vit = v1*t/10;
        threadSon.check();
        MOTOR_SetSpeed(0, vit); //Set la vitesse moteur gauche à vit
        MOTOR_SetSpeed(1, vit); //Set la vitesse moteur droit à vit
        delay(10);
      }
  MOTOR_SetSpeed(0,0);
  MOTOR_SetSpeed(1,0);
}

void tourner(float angle,float vitesse,float rayon)
{
    ENCODER_Reset(0);
    ENCODER_Reset(1);
    float arcInt = PI * rayon / 180.0 * angle;
    float arcExt = PI * (rayon + 0.192) / 180.0 * angle;//distance entre roues (m)
    float vExt= vitesse;
    float vInt= vitesse*arcInt/arcExt;
    //Serial.println(vExt);
   // Serial.println(vInt);
    int inter = 1, ext=0;
    if (angle < 0)
    {
        inter = 0;
        ext = 1;
    }
    

    MOTOR_SetSpeed(ext, vExt);
    MOTOR_SetSpeed(inter,vInt );
    float distInt = abs(3200 *arcInt/0.25);// Encodeur tour//Diametre roue en m
    float distExt = abs(3200 *arcExt/0.25);// Encodeur tour//Diametre roue en m
    float parcouruExt;
    float parcouruInt;
    if(vitesse<0)
    {
      distInt=distInt*-1;
      distExt=distExt*-1;
      while (distExt < parcouruExt)
      { 
       delay(10);
        threadSon.check();
        parcouruExt = ENCODER_Read(ext);
        parcouruInt = ENCODER_Read(inter);
        if( distInt >= parcouruInt)
        {
         parcouruExt=distExt-1;
        }
    
     }
    }
    else
    {
      while (distExt > parcouruExt)
      { 
        delay(10);
          threadSon.check();
          parcouruExt = ENCODER_Read(ext);
          parcouruInt = ENCODER_Read(inter);
          if( distInt <= parcouruInt)
          {
          parcouruExt=distExt+1;
          }
      
      }
    }
   MOTOR_SetSpeed(0,0);
   MOTOR_SetSpeed(1,0);
   } 


void setup()
{
  BoardInit();
  //float v1;
  //float distance;

  //straight_line_func(v1, distance);
  
  while (!ROBUS_IsBumper(0)) 
  {
    delay(10);
    threadSon.check();

  }
    Serial.println("Start");
  delay(7000);
  straight_line_func(-0.4, -0.1);
 tourner(45, -0.4,0.2);

 
  Serial.begin(9600);
}


void loop()
{
  int Temps_but= 2000;
 
  unsigned long compteur = 0;

    straight_line_func(.3, .2);
    // SOFT_TIMER_Update(); // A decommenter pour utiliser des compteurs logiciels
    delay(100);// Delais pour décharger le CPU
    threadSon.check();
    straight_line_func(-.3, -.3);
    delay(100);
    
     straight_line_func(.3, .1);
     unsigned long start_time = millis();
  while(compteur <= Temps_but)
  {
   threadSon.check();
    compteur = millis() - start_time;
   delay(10);
  }
  
  //straight_line_func(.3, 0.1);  
  //tourner(-80, 0.4, 0.1);
  
  //tourner(90, 0.4, 0.86);
  //tourner(90, 0.4, 0.86);
 // tourner(90, 0.4, 0.86);
 // tourner(85, 0.4, 0.86  );
  threadSon.check();

   
}
