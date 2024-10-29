#ifndef PTCCP_TM_H
#define PTCCP_TM_H

#include "SysConf.h"

struct STR_TMLine {
    sc_int<2> LineState;
    sc_int<32> rts;
    sc_int<32> wts;
    sc_int<32> owner;
    sc_int<32> data[LINE_SIZE];
};

class TimeManager {
public:
    STR_TMLine Mem[TAG_SIZE][SET_SIZE];

    //Initialize
    TimeManager() {
        for (auto &i: Mem) {
            for (auto &j: i) {
                j.LineState = STATE_Share;//start at share state
                j.rts = 1;
                j.wts = 1;
                j.owner = -1;
                for (auto &k: j.data) {
                    k = 0;
                }
            }
        }
    }
};


SC_MODULE(TMmodule) {
    //////////INPUT AND OUT PUT AREA///////////
    sc_in<bool> clk;
    sc_in<bool> rst;

    ////BUS TO TM////
    sc_in<sc_int<32>> coreID_bus_to_TM;
    sc_in<bool> valid_bus_to_TM;
    sc_in<sc_int<4>> MSG_bus_to_TM;
    sc_in<sc_int<32>> addr_bus_to_TM;
    sc_in<sc_int<32>> data_line_bus_to_TM[LINE_SIZE];
    sc_in<sc_int<32>> pts_bus_to_TM;
    sc_in<sc_int<32>> rts_bus_to_TM;
    sc_in<sc_int<32>> wts_bus_to_TM;


    ////TM TO BUS////
    sc_out<bool> valid_TM_to_bus;
    sc_out<sc_int<32>> coreID_TM_to_bus;
    sc_out<sc_int<4>> MSG_TM_to_bus;
    sc_out<sc_int<32>> addr_TM_to_bus;
    sc_out<sc_int<32>> data_line_TM_to_bus[LINE_SIZE];
    sc_out<sc_int<32>> rts_TM_to_bus;
    sc_out<sc_int<32>> wts_TM_to_bus;


    ///////////////////////////////////////////

    TimeManager TM;

    ////SHARED SIGNALS////
    sc_signal<sc_int<2>> word_addr_TM;
    sc_signal<sc_int<LINE_ADDR_LEN>> line_addr_TM;
    sc_signal<sc_int<SET_ADDR_LEN>> set_addr_TM;
    sc_signal<sc_int<TAG_ADDR_LEN>> tag_addr_TM;
    sc_signal<sc_int<UNUSED_ADDR_LEN>> unused_addr_TM;

    void split_address_TM();//analyze the addr, decomposition
    void rst_TM();

    void process_req_TM();//main func


    SC_CTOR(TMmodule) {
        cout << "constructor of TM" << endl;
        cout << "Module ports:" << endl;
        cout << "wts_TM_to_bus: " << wts_TM_to_bus.name() << endl;
        cout << "data_line_TM_to_bus: " << data_line_TM_to_bus[7].name() << endl;

        // Register SC_METHOD
        SC_CTHREAD(split_address_TM, clk.pos());
        //sensitive << addr_bus_to_TM;  // exe when addr change
        //sensitive << clk.pos();
        cout << "after constructor of split_address_TM" << endl;
//
//        SC_METHOD(rst_TM);
//        sensitive << rst.pos();  // exe when rst change

        SC_CTHREAD(process_req_TM, clk.pos());
        reset_signal_is(rst, true);
        cout << "after constructor of process_req_TM" << endl;
    }
};


#endif //PTCCP_TM_H
