#pragma once
#include "mbed.h"
#include "CANManager.h"
#include <algorithm>
#include <array>
#include <cstdint>

class receive_board : public CANReceiver{
    public:
        receive_board(CAN& can,const short start_id=0x800,const int board_num=0)
            :_can(can),_start_id(start_id),_board_num(board_num){
            // _msg_buffer.assign(_board_num,0);
            _output.resize(_board_num);
            _can.frequency(1e6);
            _can.mode(CAN::Normal);
            // data_control();
        };
        ~receive_board() override{}


        // void data_control() {
        //    _thread.start(callback(this, &receive_board::data_thread_entry));
        // }

        // //本体その2
        // void data_thread_entry(){
        //     while(true){
        //         _event_flags.wait_any(0x01);
        //         for (int id=0; id<_board_num; id++) {
        //             CANMessage local_msg;
        //             bool has_new = false;
        //             _data_mutex.lock();
        //             if (_new_data_mask & (1 << id)) {
        //                 local_msg = _msg_buffer[id];
        //                 _new_data_mask &= ~(1 << id);
        //                 has_new = true;
        //             }

        //             if(has_new){
        //                 std::copy_n(local_msg.data, 8, _output[id]);
        //             }
        //             _data_mutex.unlock();
        //         }
        //     }
        // }
        
        //本体その1
        bool handle_message(const CANMessage &msg) override{
            int id_idx = msg.id - _start_id;
            if (id_idx >= 0 && id_idx < _board_num) {
                _data_mutex.lock();
                std::copy_n(msg.data, 8, _output[id_idx].begin());
                _new_data_mask |= (1 << id_idx);
                _data_mutex.unlock();
                // _event_flags.set(0x01); 
                return true;
            }
            return false;
        }
        std::vector<array<uint8_t,8>> _output;
    private:
        CAN& _can;
        // Thread _thread;
        Mutex _data_mutex;
        // EventFlags _event_flags;
        // std::vector<CANMessage> _msg_buffer;
        uint8_t _new_data_mask;
 

       short _start_id;
       int _board_num; 
};