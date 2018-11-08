/* ****************************************************************************
Projet:   Rob Bros.
Equipe:   P-23
Auteurs:  Felix-Antoine Amyot                     amyf2502
          Gabriel Cabana                          cabg2101
          Emile Charbonneau                       chae2424
          Santiago Sebastian Cosquillo Gutierrez  coss2803
          Mathis Gagnon                           gagm2354
          Samuel Grenier                          gres2117
          Philippe Lalonde                        lalp2803
          Olivier Roy                             royo2206

Description: Fonction qui permet de deplacer un robot sur un parcours preprogramme.
Date: 04-10-2018
**************************************************************************** */

/* ****************************************************************************
Inclure les librairies de functions que vous voulez utiliser
**************************************************************************** */

#include <Arduino.h>
#include <LibRobus.h> // Essentiel pour utiliser RobUS
#include <TimedAction.h>
#include <math.h>

/* ****************************************************************************
Variables globales et defines
**************************************************************************** */
// -> defines...
// L'ensemble des fonctions y ont acces

// Distance entre les deux roues du robot.
#define DISTANCE_BETWEEN_WHEELS 18.65

// Différence de vitesse entre les deux moteurs.
#define OFFSET 0.007

// Facteur de multiplication pour la distance des capteurs infrarouges.
#define SCALE 28

/* ****************************************************************************
Vos propres fonctions sont creees ici
**************************************************************************** */

void sifflet()
{
  int khz5 = analogRead(A1);
  
  Serial.println(khz5);
  if (khz5 > 400)
  {
    MOTOR_SetSpeed(0,0);
    MOTOR_SetSpeed(1,0);
    Serial.println("STOP");
    delay(10000);
  }
}

TimedAction threadSon = TimedAction(1300,sifflet);

float IR_distance(int id)
{
  threadSon.check();
  float volts = ROBUS_ReadIR(id)*0.0048828125;  // value from sensor * (5/1024)
  float distance = SCALE*pow(volts, -1); // worked out from datasheet graph
  Serial.println(distance);   // print the distance
  return distance;
}



int calculate_pulse_forward(int distance)
{
  /********
  Fonction qui calcule la valeur maximale de pulsation selon une distance fournie.
  Valeur fournie: distance (entier en centimetres).
  Valeur retournee: pulsation maximale (entier sans unites).
  ********/

  threadSon.check();
  int max_pulse;
  max_pulse = int(distance*3200.0/(3.0*PI*2.54));
  return max_pulse;
}



void forward(int distance, float wheel_speed)
{
  /********
  Fonction qui fait avancer le robot en ligne droite.
  Valeurs fournies: distance a parcourir (entier en centimetres);
                    vitesse limite (nombre decimal en pourcentage);
                    intervalle de temps (entier en millisecondes).
  Valeur retournee: aucun retour.
  ********/

  threadSon.check();
  int max_pulse_forward = calculate_pulse_forward(distance);
  float detector_distance = IR_distance(0);

  while((ENCODER_Read(0) < max_pulse_forward && detector_distance >= 20.0))
  {
    threadSon.check();
    
    // Demarrage des moteurs.
    MOTOR_SetSpeed(0, wheel_speed);
    MOTOR_SetSpeed(1, wheel_speed + OFFSET);

    detector_distance = IR_distance(0);
  }

  // Arret des moteurs et reinitialisation des encodeurs
  MOTOR_SetSpeed(0,0.0);
  MOTOR_SetSpeed(1,0.0);
  ENCODER_ReadReset(0);
  ENCODER_ReadReset(1);
}



int calculate_pulse_turn(int degree_angle)
{
  /********
  Fonction qui calcule la valeur maximale de pulsation selon un angle fourni.
  Valeur fournie: angle (entier en degres).
  Valeur retournee: pulsation maximale (entier sans unites).
  ********/

  threadSon.check();
  int max_pulse_turn, distance;
  degree_angle %= 360;
  distance = float(DISTANCE_BETWEEN_WHEELS/2*degree_angle*PI/180.0);
  max_pulse_turn = int(distance*3200.0/(3.0*PI*2.54));
  return max_pulse_turn;
}



void turn(int degree_angle, float wheel_speed, int bool_value)
{
  /********
  Fonction qui fait tourner le robot sur lui-meme.
  Valeurs fournies: angle de rotation (entier en degres);
                    vitesse limite (nombre decimal en pourcentage);
                    valeur booleenne (entier sans unites).
  Valeur retournee: aucun retour.
  ********/

  threadSon.check();
  int max_pulse_turn;
  max_pulse_turn = calculate_pulse_turn(degree_angle);
  
  // Correction de la vitesse du moteur gauche.
  if((bool_value == 0))
    max_pulse_turn /= 1.03;

  while(ENCODER_Read(bool_value) < max_pulse_turn)
  {
    threadSon.check();

    // Demarrage des moteurs.
    if((bool_value == 1))
    {
      // Moteur de droite tourne vers l'avant (virage a gauche).
      MOTOR_SetSpeed(0, -wheel_speed);
      MOTOR_SetSpeed(1, wheel_speed);
    }
    if((bool_value == 0))
    {
      // Moteur de gauche tourne vers l'avant (virage a droite).
      MOTOR_SetSpeed(0, wheel_speed);
      MOTOR_SetSpeed(1, -wheel_speed);
    }
  }
  // Arret des moteurs et reinitialisation des encodeurs
  MOTOR_SetSpeed(0,0.0);
  MOTOR_SetSpeed(1,0.0);
  ENCODER_ReadReset(0);
  ENCODER_ReadReset(1);
}

/* ****************************************************************************
Fonctions d'initialisation (setup)
**************************************************************************** */
// -> Se fait appeler au debut du programme
// -> Se fait appeler seulement un fois
// -> Generalement on y initilise les variables globales

void setup()
{
  BoardInit();
}

/* ****************************************************************************
Fonctions de boucle infini (loop())
**************************************************************************** */
// -> Se fait appeler perpetuellement suite au "setup"

void loop()
{ 
  threadSon.check();

  int i;
  for(i = 0; i != -1; i++)
  {
    threadSon.check();
    
    i = i % 7 + 9;
    randomSeed(analogRead(i));

    int distance = random(0,241);
    float speed = random(20, 71)/100.0;
    int angle = random(0,181);
    int lead_motor = random(0,2);

    /*
    Serial.println(distance);
    Serial.println(speed);
    Serial.println(angle);
    Serial.println(lead_motor);
    */

    forward(distance, speed);
    turn(angle, speed, lead_motor);
    i++;
  }
}