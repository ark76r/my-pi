#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>

/*
        Simple test for HC SR04 sensor.
        Compile: gcc hcsr04.c -o hcsr04 -I/usr/local/include -L/usr/local/lib -lwiringPi -lpthread
*/

#define TRIGGER 5
#define ECHO    4

void setup();

static volatile unsigned int echo_start;

void isr (void) {
        unsigned int time = micros();
        int value = digitalRead(ECHO);
        unsigned int delay = 0;

        if (value == 1) {
                /* start sygnalu echo */
                echo_start = time;
        } else {
                /* koniec sygnalu echo  */
                delay = time - echo_start;
                fprintf(stdout, " %d us - %.2f cm              \r", delay, (float)(delay)/58.0f);
                fflush(stdout);
        }
}

int main() {
        printf("Setup\n");
        setup();

        while(1) {
                delay(1000);
                fprintf(stdout, "!\r");
                fflush(stdout);
                digitalWrite(TRIGGER, 1);
                delayMicroseconds(10);
                digitalWrite(TRIGGER, 0);
        }
}

void setup() {

        if (wiringPiSetup() != 0) {
                printf("Setup Error\n");
                exit(0);
        }

        pinMode(TRIGGER, OUTPUT);
        pinMode(ECHO, INPUT);
        wiringPiISR (ECHO, INT_EDGE_BOTH, &isr);
}
