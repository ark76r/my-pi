/*
        servo2.c control servo with PWM on the odroid c2.

        compile: gcc -o servo2 servo2.c

        Connection from SG90:  brown - GND, red +5V, orange: PWM0, pin 33 
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define PWM_CTRL "/sys/devices/platform/pwm-ctrl/"
#define FREQ "freq0"
#define ENABLE "enable0"
#define DUTY "duty0"


#define MED 77
#define MAX 124
#define MIN 30

void echo(char *filename, int value)
{
        FILE *f = fopen(filename, "w");
        if (f == NULL) {
                perror(filename);
                exit(1);
        }
        printf("echo %d > %s\n", value, filename);
        fprintf(f, "%d\n", value);
        fclose(f);
}


/*
        Convert angle into values for PWM
        angle is 0 for neutral position
                and +/- 90 for min/max
*/
unsigned int calculate(int angle) 
{
        int val = MAX - MIN;
        val = val * ((angle+90.0f)/180.0f);
        val += MIN;
        printf("%d -> %d\n", angle, val);
        return val;

}


int main(int argc, char *argv[])
{
        char freq[255];
        char enable[255];
        char duty[255];

        int angle = 0;

        strcpy(freq, PWM_CTRL);
        strcat(freq, FREQ);

        strcpy(enable, PWM_CTRL);
        strcat(enable, ENABLE);

        strcpy(duty, PWM_CTRL);
        strcat(duty, DUTY);
/*
// Setup PWM: 50Hz.
        echo(enable, 0);
        sleep(1);
        echo(freq, 50);
        sleep(1);
        echo(enable, 1);
        sleep(1);
*/

        if (argc > 1) {
                angle = atoi(argv[1]);
                if (angle < -90) angle = -90;
                if (angle > 90) angle = 90;
        }

        echo(duty, calculate(angle));

        return 0;
}
