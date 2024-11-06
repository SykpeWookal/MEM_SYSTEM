#include "SysConf.h"
//
// Created by 65296 on 2024/1/12.
//
//#ifndef FIR_TB_H
//#define FIR_TB_H
//
//#include "systemc.h"
//
//SC_MODULE( tb ){
//    sc_in<bool> clk;
//    sc_out<bool> rst;
//    sc_out< sc_int<16> > inp;
//    sc_in< sc_int<16> > outp;
//
//    void source();
//    void sink();
//
//    SC_CTOR( tb ){
//        SC_CTHREAD( source, clk.pos() );
//        SC_CTHREAD( sink, clk.pos() );
//    }
//
//};
//
//
//SC_MODULE( fir ){
//    sc_in<bool> clk;
//    sc_in<bool> rst;
//    sc_in< sc_int<16> > inp;
//    sc_out< sc_int<16> > outp;
//
//    void fir_main();
//
//    SC_CTOR( fir ){
//        SC_CTHREAD( fir_main, clk.pos() );
//        reset_signal_is( rst, true );
//    }
//
//};
//
//
//#endif //FIR_TB_H
SC_MODULE(testbench) {
    sc_in<bool> clk;          // Clock signal
    sc_out<bool> rst;          // Reset signal

    // Signals for CORE interface
    //sc_out<bool> ready_core[CORE_NUM];
    sc_out<bool> valid_core[CORE_NUM];
    sc_out<sc_int<3>> RorWreq_core[CORE_NUM];
    sc_out<sc_int<32>> addr_core[CORE_NUM];
    sc_out<sc_int<32>> data_core[CORE_NUM];
    sc_out<sc_int<32>> req_ts_core[CORE_NUM];

    // Signals for Cache output
    //sc_in<bool> ready_cache_to_core[CORE_NUM];
    sc_in<bool> valid_cache_to_core[CORE_NUM];
    sc_in<sc_int<32>> data_cache_to_core[CORE_NUM];


    void stimulus();

    void monitor();

    SC_CTOR(testbench) {


        SC_CTHREAD(stimulus, clk.pos());// Trigger stimulus on rising edge of clk
        //sensitive << clk.pos();

        SC_CTHREAD(monitor, clk.pos());// Monitor outputs on rising edge of clk
        //sensitive << clk.pos();
    }


};