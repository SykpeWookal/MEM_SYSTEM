#ifndef PTCCP_BUS_H
#define PTCCP_BUS_H

#include "SysConf.h"


SC_MODULE(SharedBus) {
    //////////INPUT AND OUT PUT AREA///////////
    sc_in<bool> clk;
    sc_in<bool> rst;

    ////CACHES TO BUS////
    sc_in<bool> valid_cache_to_bus[CORE_NUM];
    sc_in<sc_int<32>> addr_cache_to_bus[CORE_NUM];
    sc_in<sc_int<5>> MSG_cache_to_bus[CORE_NUM];
    sc_in<sc_int<32>> pts_cache_to_bus[CORE_NUM];
    sc_in<sc_int<32>> rts_cache_to_bus[CORE_NUM];
    sc_in<sc_int<32>> wts_cache_to_bus[CORE_NUM];
    sc_in<sc_int<32>> data_line_cache_to_bus[CORE_NUM][LINE_SIZE];

    sc_in<sc_int<32>> req_ts_cache_to_bus[CORE_NUM];

    ////BUS TO CACHES////
    sc_out<bool> valid_bus_to_cache[CORE_NUM];
    sc_out<sc_int<32>> addr_bus_to_cache[CORE_NUM];
    sc_out<sc_int<5>> MSG_bus_to_cache[CORE_NUM];
    //sc_out<sc_int<32>> pts_bus_to_cache[CORE_NUM];
    sc_out<sc_int<32>> rts_bus_to_cache[CORE_NUM];
    sc_out<sc_int<32>> wts_bus_to_cache[CORE_NUM];
    sc_out<sc_int<32>> data_line_bus_to_cache[CORE_NUM][LINE_SIZE];

    ////TM TO BUS////
    sc_in<sc_int<32>> coreID_TM_to_bus;
    sc_in<bool> valid_TM_to_bus;
    sc_in<sc_int<5>> MSG_TM_to_bus;
    sc_in<sc_int<32>> addr_TM_to_bus;
    sc_in<sc_int<32>> rts_TM_to_bus;
    sc_in<sc_int<32>> wts_TM_to_bus;
    sc_in<sc_int<32>> data_line_TM_to_bus[LINE_SIZE];

    ////BUS TO TM////
    sc_out<sc_int<32>> coreID_bus_to_TM;
    sc_out<bool> valid_bus_to_TM;
    sc_out<sc_int<5>> MSG_bus_to_TM;
    sc_out<sc_int<32>> addr_bus_to_TM;
    sc_out<sc_int<32>> pts_bus_to_TM;
    sc_out<sc_int<32>> rts_bus_to_TM;
    sc_out<sc_int<32>> wts_bus_to_TM;
    sc_out<sc_int<32>> data_line_bus_to_TM[LINE_SIZE];

    ///////////////////////////////////////////


    ////SHARED SIGNALS////
    //Bus State
    sc_signal<int> BusState;

    sc_signal<int> arbitrationResult_coreID;

    void bus_arbiter();

    void bus_rst();

    void process_req_bus();//main func


    SC_CTOR(SharedBus) {
        cout << "constructor of bus" << endl;

        // Register
        SC_CTHREAD(bus_arbiter, clk.pos());
        reset_signal_is(rst, true);
        cout << "after constructor bus_arbiter" << endl;
        SC_CTHREAD(process_req_bus, clk.pos());
        reset_signal_is(rst, true);
        cout << "after constructor process_req_bus" << endl;
    }
};


#endif //PTCCP_BUS_H
