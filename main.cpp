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
#define reader
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
// Blinking rate in milliseconds
#define BLINKING_RATE     20ms
CAN can(PB_12,PB_13,1000000);
CANMessage msg;

UnbufferedSerial pc(USBTX,USBRX,921600);
int main()
{
    // Initialise the digital pin LED1 as an output
    can.mode(CAN::Normal);
    msg.id=0x310;
    msg.len=8;
    for(int i=0;i<8;i++)msg.data[i]=0;

    DigitalOut led(LED1);


    while (true) {
        char key;
        if(pc.readable()){
            pc.read(&key, 1);
            printf("%c\r\n",key);
            if(key>='0'&&key<='9'){
                for(int i=0;i<4;i++)
                    msg.data[3]=(key-'0')*28;
                //msg.data[6]=0x0;
                msg.data[7]=0x0;
                //motor_controller.control_tracon(control_mode ,setpoint,( key-'0')*max_deg/10+offset_deg , 1);
                //setpoint[0] =( key-'0')*max_deg/10+offset_deg;
                //control_mode[0] = abs(now_ang[0]/19-setpoint[0])>1800?TRACON:ANGLE;
            }else if(key=='j'){
                for(int i=0;i<4;i++)
                    msg.data[3]=0xff;
                msg.data[7]=0x0;
            }
            else if(key=='h'){
                for(int i=0;i<4;i++)
                    msg.data[3]=0x0;
                msg.data[7]=0x20;
            }
            else if(key=='z'){
                for(int i=0;i<4;i++)
                    msg.data[3]=0x0;
                msg.data[6]=0x0;
                msg.data[7]=0x0;
            }
            else if(key=='x'){
                for(int i=0;i<4;i++)
                    msg.data[3]=0x0;
                msg.data[6]=0x5;
                msg.data[7]=0x0;
            }
            //can.write(msg);
        }
        can.write(msg);
            led = !led;
        ThisThread::sleep_for(BLINKING_RATE);
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
CAN can1(PB_12,PB_13,1000000);
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
        printf("%x\t",msg.id);
        for (int i=0; i<msg.len; i++) {
            printf("%x\t",msg.data[i]);
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