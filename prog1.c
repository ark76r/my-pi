#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <unistd.h>
#include <time.h>

void servo(int);
int setup_gpio();

int main(int argc, char** argv)
{
  int fd, status, again;
  struct pollfd pollfd;
  char movement = 0;

  again = 0;
  do {
    fd = open("/sys/class/gpio/gpio17/value", O_RDONLY);
    if (fd < 0) {
      if (again) {
        perror("Cannot open gpio17/value");
        exit(1);
      }
      again++;
      setup_gpio(1);
    }
  } while (fd < 0);

  setup_gpio(0);

  pollfd.fd = fd;
  pollfd.events = POLLPRI;

  servo(90);

  while(1) {
    status = poll(&pollfd, 1, 5 * 1000);

    if (status < 0) {
      perror("Error in poll");
      exit(1);
    }

    if (status == 0) {
      printf("timeout... (%s)\n", movement ? "up": "down");
      if (!movement) {
        servo(90); /* after timeout set servo to right) */
      }

    } else if (status > 0 && pollfd.revents & POLLPRI) {

      char buf[32];
      int count = 0;


      count = read(fd, buf, sizeof(buf));
      lseek(fd, 0, SEEK_SET);

      if (count > 0) {
        time_t timer;
        char buffer[26];
        struct tm* tm_info;

        time(&timer);
        tm_info = localtime(&timer);

        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);

        printf("%s: %c \n", buffer, buf[0]);

        if (buf[0] == '0') {
            movement = 0;
        } else {
          servo(0); /* up up*/
          movement = 1;
        }
      } else {
        perror("in read");
        exit(1);
      }
    } /* pollfd.revents */
  } /* while (1)*/
   close(fd);
   printf("The end\n");
}

void echo_to_file(char *text, char *file) {
  FILE *f = fopen(file, "w");
  if (!f) { perror(file); exit(1);}
  fputs(text, f);
  fclose(f);
}

void servo(int angle) {
    char buf[1024];
    int number;

    if (angle < -90) angle = -90;
    if (angle > 90) angle = 90;
    number = 1500 - (angle * 1000/90);
    printf(">>> servo(%d) -> %d \n", angle, number);
    sprintf(buf, "s 24 %d mils 500 s 24 0", number);
    echo_to_file(buf, "/dev/pigpio");
}

/*
Setup IRQ (and polling) on GPIO17
*/
int setup_gpio(int doexport) {
  puts("Setup ");
  if (doexport) {
    echo_to_file("17", "/sys/class/gpio/export");
    sleep(1);
  }
  echo_to_file("in", "/sys/class/gpio/gpio17/direction");
  echo_to_file("both", "/sys/class/gpio/gpio17/edge");

}
