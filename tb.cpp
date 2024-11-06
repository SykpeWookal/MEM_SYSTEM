//
// Created by 65296 on 2024/1/12.
//
#include "tb.h"


// Stimulus generation
void testbench::stimulus() {
    cout << endl;
    cout << "simulation start, t=0" <<endl;
    wait();  // Wait before starting
    cout << endl;
    cout << "simulation t=1" <<endl;
    // Initialize signals
    for (int i = 0; i < CORE_NUM; i++) {
        //ready_core[i] = false;
        valid_core[i].write(false);
        RorWreq_core[i].write(CORE_NO_REQ);
        addr_core[i].write(0);
        data_core[i].write(0);
        req_ts_core[i].write(INT_MAX);
    }
    
    // Apply reset
    rst.write(true);
    wait();
    cout << endl;
    cout << "simulation t=2" <<endl;
    wait();
    cout << endl;
    cout << "simulation t=3" <<endl;
    rst.write(false);

    // Example test case: Core 0 write request

    addr_core[0].write(0x1000 + 1 * 0x10);  // Unique address for each core
    data_core[0].write(0xA0 + 1);           // Example data
    RorWreq_core[0].write(CORE_W_REQ);                // Write request
    valid_core[0].write(true);               // Set valid signal
    req_ts_core[0].write(3);

    for (int i = 0; i < 50; i++) {
        wait();// Wait for the next clock cycle
        cout << endl;
        cout << "simulation t="+ to_string((i+4)) <<endl;
    }

    valid_core[0].write(false);               // Set valid signal

    // Example test case: Core 1 read request

    addr_core[1].write(0x2000 + 1 * 0x10);  // Unique address for each core
    RorWreq_core[1].write(0);                // Read request
    valid_core[1].write(true);               // Set valid signal
    for (int i = 0; i < 50; i++) {
        wait();// Wait for the next clock cycle
        cout << endl;
        cout << "simulation t="+ to_string((i+54)) <<endl;
    }
    valid_core[1].write(false);              // Clear valid signal
    wait();                    // Wait for the next clock cycle
    cout << endl;
    cout << "simulation t=104" <<endl;

    // Finish the simulation after some time
    wait(100, SC_NS);
    sc_stop();
}


// Monitor the outputs
void testbench::monitor() {
    while (true) {
        wait();  // Wait for the next clock cycle
        for (int i = 0; i < CORE_NUM; i++) {
            if (valid_cache_to_core[i]) {
                cout << "Time: " << sc_time_stamp()
                     << " Core " << i
                     << " received data: " << data_cache_to_core
                     << endl;
            }
        }
    }
}




//void tb::source() {
//    //restart
//    inp.write(0);
//    rst.write(true);
//    wait();
//    rst.write(false);
//    wait();
//
//    sc_int<16> tmp;
//    //send stimulus to FIR
//    for (int i = 0; i < 64; i++) {
//        if (i > 23 && i < 29)
//            tmp = 256;
//        else
//            tmp = 0;
//        inp.write(tmp);
//        wait();
//    }
//}
//
//void tb::sink() {
//    sc_int<16> indata;
//
//    //read output coming from DUT
//    for (int i = 0; i < 64; i++) {
//        indata = outp.read();
//        wait();
//        cout << i << ":" << indata.to_int() << endl;
//    }
//    //end simulation
//    sc_stop();
//}
//
////coefficients for each FIR
//const sc_int<8> coef[5] = {
//        18,
//        77,
//        107,
//        77,
//        18
//};
//
////FIR Main thread
//void fir::fir_main(){
//    sc_int<16> taps[5];
//
//    //reset
//    outp.write(0);
//    wait();
//
//
//    while ( true ){
//        for(int i = 4; i>0; i--) {
//            taps[i] = taps[i - 1];
//        }
//        taps[0] = inp.read();
//
//        sc_int<16> val;
//        for(int i = 0;i<5;i++){
//            val+=coef[i]*taps[i];
//        }
//        outp.write(val);
//        wait();
//    }
//
//}