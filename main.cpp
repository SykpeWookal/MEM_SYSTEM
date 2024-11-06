//#include "SysConf.h"
//#include "PrivateCache.h"
//#include "Bus.h"
//#include "TM.h"
#include "MEM_SYSTEM.h"
#include "tb.h"

//main.cpp

// SYSTEM module, instantiating MEM_SYSTEM and testbench
//SC_MODULE(SYSTEM) {
//    MEM_SYSTEM *mem_system;
//    testbench *tb;
//    sc_clock clk_sig{"clk_sig", 10, SC_NS};
//    sc_signal<bool> rst_sig;
//
//    SC_CTOR(SYSTEM) : clk_sig("clk_sig", 10, SC_NS) {
//        // Instantiate testbench
//        tb = new testbench("tb");
//        tb->clk(clk_sig);
//        tb->rst(rst_sig);
//
//        // Instantiate MEM_SYSTEM
//        mem_system = new MEM_SYSTEM("mem_system");
//        mem_system->clk(clk_sig);
//        mem_system->rst(rst_sig);
//
//        // Connect testbench to MEM_SYSTEM (CORE interface)
//        for (int i = 0; i < CORE_NUM; i++) {
//            //mem_system->ready_core[i](tb->ready_core[i]);
//            mem_system->valid_core[i](tb->valid_core[i]);
//            mem_system->RorWreq_core[i](tb->RorWreq_core[i]);
//            mem_system->addr_core[i](tb->addr_core[i]);
//            mem_system->data_core[i](tb->data_core[i]);
//            mem_system->req_ts_core[i](tb->req_ts_core[i]);
//
//            // Connect MEM_SYSTEM output to testbench input
//            //tb->ready_cache_to_core[i](mem_system->ready_cache_to_core[i]);
//            tb->valid_cache_to_core[i](mem_system->valid_cache_to_core[i]);
//            tb->data_cache_to_core[i](mem_system->data_cache_to_core[i]);
//        }
//    }
//
//    ~SYSTEM() {
//        delete tb;
//        delete mem_system;
//    }
//};



SC_MODULE(SYSTEM) {
    // 实例化testbench和MEM_SYSTEM模块
    testbench *tb;
    MEM_SYSTEM *mem_system;

    // 定义SYSTEM模块的输入输出接口（主要用于testbench和MEM_SYSTEM之间的连接）
    sc_clock clk;            // 时钟信号
    sc_signal<bool> rst;      // 复位信号

    // CORE 到 MEM_SYSTEM 的信号
    sc_signal<bool> valid_core[CORE_NUM];
    sc_signal<sc_int<3>> RorWreq_core[CORE_NUM];
    sc_signal<sc_int<32>> addr_core[CORE_NUM];
    sc_signal<sc_int<32>> data_core[CORE_NUM];
    sc_signal<sc_int<32>> req_ts_core[CORE_NUM];

    // MEM_SYSTEM 到 CORE 的信号
    sc_signal<bool> valid_cache_to_core[CORE_NUM];
    sc_signal<sc_int<32>> data_cache_to_core[CORE_NUM];

    SC_CTOR(SYSTEM) : clk("clk", 10, SC_NS) { // 时钟周期10ns
        // 实例化testbench模块并连接信号
        tb = new testbench("testbench");
        tb->clk(clk);
        tb->rst(rst);

        for (int i = 0; i < CORE_NUM; i++) {
            tb->valid_core[i](valid_core[i]);
            tb->RorWreq_core[i](RorWreq_core[i]);
            tb->addr_core[i](addr_core[i]);
            tb->data_core[i](data_core[i]);
            tb->req_ts_core[i](req_ts_core[i]);

            tb->valid_cache_to_core[i](valid_cache_to_core[i]);
            tb->data_cache_to_core[i](data_cache_to_core[i]);

        }

        // 实例化MEM_SYSTEM模块并连接信号
        mem_system = new MEM_SYSTEM("MEM_SYSTEM");
        mem_system->clk(clk);
        mem_system->rst(rst);

        for (int i = 0; i < CORE_NUM; i++) {
            mem_system->valid_core[i](valid_core[i]);
            mem_system->RorWreq_core[i](RorWreq_core[i]);
            mem_system->addr_core[i](addr_core[i]);
            mem_system->data_core[i](data_core[i]);
            mem_system->req_ts_core[i](req_ts_core[i]);

            mem_system->valid_cache_to_core[i](valid_cache_to_core[i]);
            mem_system->data_cache_to_core[i](data_cache_to_core[i]);

        }
    }

    ~SYSTEM() {
        delete tb;
        delete mem_system;
    }
};


// Main function
SYSTEM *top = nullptr;

int sc_main(int argc, char *argv[]) {
    top = new SYSTEM("top");
    sc_start();
    return 0;
}
