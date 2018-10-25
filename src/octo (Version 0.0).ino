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

#include <LibRobus.h> // Essentiel pour utiliser RobUS
#include <math.h>

/* ****************************************************************************
Variables globales et defines
**************************************************************************** */
// -> defines...
// L'ensemble des fonctions y ont acces

// Facteur de multiplication pour la distance des capteurs infrarouges.
#define SCALE 28

#define LIMIT_LOW 10.0
#define LIMIT_HIGH 60.0
#define TOLERANCE 0.05
#define OFFSET 0.007

/* ****************************************************************************
Vos propres fonctions sont creees ici
**************************************************************************** */

int calculate_pulse_forward(int distance)
{
  /********
  Fonction qui calcule la valeur maximale de pulsation selon une distance fournie.
  Valeur fournie: distance (entier en centimetres).
  Valeur retournee: pulsation maximale (entier sans unites).
  ********/
  int max_pulse;
  max_pulse = int(distance*3200.0/(3.0*PI*2.54));
  return max_pulse;



void forward(int distance, float wheel_speed, int t)
{
  /********
  Fonction qui fait avancer le robot en ligne droite.
  Valeurs fournies: distance a parcourir (entier en centimetres);
                    vitesse limite (nombre decimal en pourcentage);
                    intervalle de temps (entier en millisecondes).
  Valeur retournee: aucun retour.
  ********/
  int max_pulse_forward;
  max_pulse_forward = calculate_pulse_forward(distance);

  while(ENCODER_Read(0) < max_pulse_forward)
  {
    // Demarrage des moteurs.
    MOTOR_SetSpeed(0, wheel_speed);
    MOTOR_SetSpeed(1, wheel_speed - OFFSET);
  }

  // Arret des moteurs et reinitialisation des encodeurs
  MOTOR_SetSpeed(0,0.0);
  MOTOR_SetSpeed(1,0.0);
  ENCODER_ReadReset(0);
  ENCODER_ReadReset(1);
}



float IR_distance(int id)
{
  float volts = ROBUS_ReadIR(id)*0.0048828125;  // value from sensor * (5/1024)
  float distance = SCALE*pow(volts, -1); // worked out from datasheet graph
  Serial.println(distance);   // print the distance
  return distance;
}

int ball_detection(float lecture_0, float lecture_1)
{
  if((lecture_0 <= LIMIT_LOW && lecture_1 <= LIMIT_LOW))
  {
    lecture_0 = 1000.0;
    lecture_1 = 1000.0;
  }
  else if((lecture_0 >= LIMIT_HIGH && lecture_1 >= LIMIT_HIGH))
  {
    lecture_0 = 1000.0;
    lecture_1 = 1.0;
  }

  if((1.0 - TOLERANCE <= lecture_0/lecture_1 <= 1.0 + TOLERANCE))
    return 0;
  else
    return 1;
}

void ball_capture()
{
  float lecture_0 = IR_distance(0);
  float lecture_1 = IR_distance(1);
  int object;

  if((lecture_0 <= LIMIT_LOW && lecture_1 >= LIMIT_LOW*(1.0 + TOLERANCE)))
    !!!!!BALLON CAPTURÉ!!!!!
  else
    object = ball_detection(lecture_0, lecture_1);

  if((object == 1))
  {
    forward(lecture_0, 0.6);
  }
  else((object == 0))
  {
    randomSeed();
    turn(random(90,181), 0.5, random(0,1));
  }
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
  forward(100, 0.6);
  ball_capture
}