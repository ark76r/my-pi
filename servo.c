#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>

/*
        Simple test for servomotor SC90.
        Compile: gcc servo.c -o servo -I/usr/local/include -L/usr/local/lib -lwiringPi -lpthread
*/

/*pin number (as seen by wiringPi. 12th on odroid (6th on the right side)*/
#define SERVO 1

void setup();
void pulse(unsigned int time, unsigned int loop);

int main(int argc, char* argv[]) {
        unsigned int time = 1500;
        unsigned int loop = 10;
        unsigned int pos = 0;

        if (argc >1 ) {
                time = atoi(argv[1]);
                if (time < 500) {
                        printf("minimal time is 800 us\n");
                        exit(1);
                }
                if (time > 3000) {
                        printf("maximum time is 3000 us\n");
                        exit(1);
                }
        }
        if (argc >2) {
                loop = atoi(argv[2]);
                if (loop < 1) loop = 1;
                if (loop > 100) loop = 100;
        }

        setup();

        pulse(time, loop < 10 ? 10 : loop);

        for (pos = 500; pos < 2400; pos += 50) {
                pulse(pos, loop);
        }
        for (; pos > 0; pos -= 50) {
                pulse(pos, loop);
        }
}
/*
        pulse - send 'loop' pulses each 'time' microseconds long.
*/
void pulse(unsigned int time, unsigned int loop) 
{
        unsigned int period = 20000; /* 20 ms */

        for (int i=0; i < loop; i++) {
                digitalWrite(SERVO, 1);
                delayMicroseconds(time);
                digitalWrite(SERVO, 0);
                delayMicroseconds(period - time);
        }
}

void setup() {

        if (wiringPiSetup() != 0) {
                printf("Setup Error\n");
                exit(0);
        }

        pinMode(SERVO, OUTPUT);
        digitalWrite(SERVO, 0);
}

