#include "mbed.h"
#include "pins.h"
#include "motor.h"
#include "RGBLed.hpp"

Serial pc(USBTX, USBRX);
Serial com(COMTX, COMRX);

RGBLed led1(LED1R, LED1G, LED1B);
RGBLed led2(LED2R, LED2G, LED2B);

char buf[16];
bool serialData = false;
int serialCount = 0;

#define NUMBER_OF_MOTORS 4

Motor motors[NUMBER_OF_MOTORS] = {
    Motor(&pc, M0_PWM, M0_DIR1, M0_DIR2, M0_FAULT, M0_ENCA, M0_ENCB),
    Motor(&pc, M1_PWM, M1_DIR1, M1_DIR2, M1_FAULT, M1_ENCA, M1_ENCB),
    Motor(&pc, M2_PWM, M2_DIR1, M2_DIR2, M2_FAULT, M2_ENCA, M2_ENCB),
    Motor(&pc, M3_PWM, M3_DIR1, M3_DIR2, M3_FAULT, M3_ENCA, M3_ENCB)
};

PwmOut m0(M0_PWM);
PwmOut m1(M1_PWM);
PwmOut m2(M2_PWM);
PwmOut m3(M3_PWM);
PwmOut pwm0(PWM0);
PwmOut pwm1(PWM1);

void serialInterrupt();
void parseCommad(char *command);

int main() {
    while (1)
    {
        float x;
        pc.scanf("%f", &x);
        pc.printf("got: %f\n", x);
        m0 = m1 = m2 = m3 = pwm0 = pwm1 = x;
    }

    for (int i = 0; i < NUMBER_OF_MOTORS; i++) {
        motors[i].init();
    }

    pc.printf("Start\n");

    pc.attach(&serialInterrupt);
    int count = 0;
    while(1) {
        if (count % 20 == 0) {
            for (int i = 0; i < NUMBER_OF_MOTORS; i++) {
                pc.printf("s%d:%d\n", i, motors[i].getSpeed());
            }
        }
        if (serialData) {
            char temp[16];
            memcpy(temp, buf, 16);
            memset(buf, 0, 16);
            serialData = false;
            parseCommad(temp);
        }
        //motors[0].pid(motor0Ticks);
        //motor0Ticks = 0;
        wait_ms(50);
        count++;

        //pc.printf("buf: %s\n", buf);
        //pc.printf("Loop\n");
    }
}

void serialInterrupt(){
    while(pc.readable()) {
        buf[serialCount] = pc.getc();
        serialCount++;
    }
    if (buf[serialCount - 1] == '\n') {
        serialData = true;
        serialCount = 0;
    }
}

void parseCommad (char *command) {
    if (command[0] == 's' && command[1] == 'd') {
        int16_t speed = atoi(command + 2);
        motors[0].pid_on = 1;
        motors[0].setSpeed(speed);
    }
    if (command[0] == 's') {
        for (int i = 0; i < NUMBER_OF_MOTORS; i++) {
            pc.printf("s%d:%d\n", i, motors[i].getSpeed());
        }
    } else if (command[0] == 'w' && command[1] == 'l') {
        int16_t speed = atoi(command + 2);
        motors[0].pid_on = 0;
        if (speed < 0) motors[0].backward(-1*speed/255.0);
        else motors[0].forward(speed/255.0);
    } else if (command[0] == 'p' && command[1] == 'p') {
        uint8_t pGain = atoi(command + 2);
        motors[0].pgain = pGain;
    } else if (command[0] == 'p' && command[1] == 'i') {
        uint8_t iGain = atoi(command + 2);
        motors[0].igain = iGain;
    } else if (command[0] == 'p' && command[1] == 'd') {
        uint8_t dGain = atoi(command + 2);
        motors[0].dgain = dGain;
    } else if (command[0] == 'p') {
        char gain[20];
        motors[0].getPIDGain(gain);
        pc.printf("%s\n", gain);
    }
}
