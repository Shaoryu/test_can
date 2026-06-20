/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "CANManager.h"
#include "receive_board.hpp"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
//2026仕様
#define writer2
#ifdef reader26s
UnbufferedSerial pc(USBTX,USBRX,921600);
CAN can1(PD_0,PD_1,1e6);
//CAN can1(PB_5,PB_6,1e6);
receive_board rev(can1,0x100,3);
DigitalOut leds[4]={PC_0,PC_1,PC_2,PC_3};
CANMessage msg;
std::vector<array<uint8_t,8>> output;
typedef union {
    char _msg_buf[8];
    int64_t num;
}msg_conv;
msg_conv data;
int main(){
    bool ok=false;
    leds[0]=true;
    msg.id=0;
    while (true) {
        output = rev._output;
        //printf("%x\t%lld\r\n",msg.id,data.num);
        //printf("%x\t",msg.id);
        for (int i=0; i<8; i++) {
            printf("%x\t",output[0][i]);
        }
        
        int16_t raw_theta = (int16_t)((output[0][4] << 8) | output[0][5]);
        printf("%d",raw_theta);
        //if(msg.id);
        printf("\r\n");
        //can2.write(smsg);
    }
}
#endif
#ifdef writer
#define ID 0x300
CAN can(PA_11,PA_12,1e6);
CANMessage msg;

int main(){
    can.mode(CAN::Normal);
    msg.id=ID;
    msg.len=8;
    for(int i=0;i<8;i++)msg.data[i]=0xFF;
    while(1){
        can.write(msg);
        ThisThread::sleep_for(1ms);
    }
}
#endif
#ifdef writer2
#define ID 0x300
#define LOOP_RATE     5ms
CAN can(PB_12,PB_13,1e6);
DigitalOut leds[4]={PC_0,PC_1,PC_2,PC_3};
UnbufferedSerial pc(USBTX,USBRX,921600);
CANMessage msg;
int16_t speeds[4]={};
int main()
{
    leds[0]=true;
    can.mode(CAN::Normal);
    msg.id=ID;
    msg.len=8;
    for(int i=0;i<8;i++)msg.data[i]=0;

    DigitalOut led(LED1);

    while (true) {
        msg.id=ID;
        char key;
        if(pc.readable()){
            leds[2]=!leds[2];
            pc.read(&key, 1);
            printf("%c\n",key);
            switch (key) {
                case 'w':case '1':
                    speeds[0]=120;
                    break;
                case 's':case '2':
                    speeds[0]=-120;
                    break;
                case 'a':case '3':
                    speeds[3]=120;
                    break;
                case 'd':case '4':
                    speeds[3]=-120;
                    break;
                default:for(int i=0;i<4;i++)speeds[i]=0;
            }
            for(int i=0;i<4;i++){
                msg.data[i*2]=((uint16_t)speeds[i])>>8;
                msg.data[i*2+1]=((uint16_t)speeds[i])&0xFF;
            }
            for (int i=0; i<msg.len; i++) {
                printf("%x\t",msg.data[i]);
            }
        }
        for(int i=0;i<3;i++){
            can.write(msg);
            wait_us(200);
        }
        // msg.id++;
        // wait_us(200);
        // can.write(msg);
        printf("err:%3x\n",can.tderror());
        leds[1]=!leds[1];
        ThisThread::sleep_for(LOOP_RATE);
    }
}
// CAN can(PB_12,PB_13,1000000);
// CANMessage msg;
// DigitalOut leds[3]{
//     DigitalOut(PA_0),
//     DigitalOut(PA_1),
//     DigitalOut(PA_2)
// };
// int main(){
//     can.mode(CAN::Normal);
//     msg.id=0x05;
//     msg.len=8;
//     msg.data[0]=0xff;
//     while (true) {
//         if(can.write(msg))leds[0]=!leds[0];
//         leds[1]=!leds[1];
//         msg.data[0]=0;
//         ThisThread::sleep_for(10ms);
//         if(can.write(msg))leds[0]=!leds[0];
//         leds[1]=!leds[1];
//         msg.data[0]=1;
//         printf()
//         ThisThread::sleep_for(10ms);
//     }
// }
#endif


#ifdef reader
UnbufferedSerial pc(USBTX,USBRX,921600);
CAN can1(PA_11,PA_12,1000000);
//CAN can1(PB_5,PB_6,1000000);
DigitalOut leds[4]={PC_0,PC_1,PC_2,PC_3};
CANMessage msg;
EventQueue queue_control(32 * EVENTS_EVENT_SIZE);
Thread can_thread;
typedef union {
    char _msg_buf[8];
    int64_t num;
}msg_conv;
msg_conv data;
size_t size_msg=sizeof(msg.data);

void can_receive_loop() {
    can1.read(msg);
    leds[1]=!leds[1];
}
void onReceive() {
    queue_control.call(can_receive_loop);
}
int main(){
    bool ok=false;
    leds[0]=true;
    msg.id=0;
    //can1.mode(CAN::Silent);
    can_thread.start(callback(&queue_control, &EventQueue::dispatch_forever));
    can1.attach(onReceive, CAN::RxIrq);
    //can1.filter(0x101, 0x7FF, CANStandard);
    while (true) {
        memcpy(data._msg_buf, msg.data, size_msg);
        //printf("%x\t%lld\r\n",msg.id,data.num);
        if(true){
            printf("%x\t",msg.id);
            for (int i=0; i<msg.len; i++) {
                printf("%x\t",msg.data[i]);
            }
        }
        
        
        // int16_t raw_theta = (int16_t)((msg.data[4] << 8) | msg.data[5]);
        // printf("%d",raw_theta);
        //if(msg.id);
        printf("\r\n");
        //can2.write(smsg);
    }
}
#endif


// #include "mbed.h"
// #include "IncEnc_board.h"//木原のgithub

// CAN can(PA_11, PA_12, 1000000);
// UnbufferedSerial pc(USBTX, USBRX, 115200);
// IncEnc_board encoder(can, 1);

// int main() {
//     encoder.encoder_reset_all();
//     while(true) {
//         int64_t received_angle;
//         encoder.conv_data_all(&received_angle);
        
//         if(pc.readable()){
//             char key = 0;
//             pc.read(&key, 1);
//             switch(key){
//                 case 'r': 
//                     printf("Sending reset command to node 1...\r\n");
//                     encoder.encoder_reset_all();
//                     break;
//             }
//         }
//         printf("Received data: %lld\r\n", received_angle);
        
//         ThisThread::sleep_for(1ms);
//     }
// }
// #include "mbed.h"
// CAN can(PA_11,PA_12,1000000);
// CANMessage msg;
// int main(){
//     for(auto &i:msg.data)i=0xff / 2;
//     msg.len=8;
//     msg.id=0x501;
//     while (true) {
//         can.write(msg);
//         ThisThread::sleep_for(100ms);
//     }
// }