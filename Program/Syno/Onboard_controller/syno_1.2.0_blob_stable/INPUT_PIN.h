/*
 * THIS DEFINATION IS FOR INPUT PIN
 */

int RC_1_PWM, RC_2_PWM, RC_3_PWM, RC_4_PWM, RC_5_PWM ;

int RC_1_SUM,RC_2_SUM,RC_3_SUM,RC_4_SUM,RC_5_SUM;

int RC_1_OUTPUT;
int RC_2_OUTPUT;
int RC_3_OUTPUT;
int RC_4_OUTPUT;
int RC_5_OUTPUT;

#define BAT_VOLTAGE_PIN A0
#define SONAR 45

unsigned long RC_TIMER = 0;

bool failSafe = 1;
bool lostFrame = 1;
