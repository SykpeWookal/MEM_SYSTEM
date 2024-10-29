//#include "SysConf.h"
//#include "PrivateCache.h"
//#include "Bus.h"
//#include "TM.h"
#include "MEM_SYSTEM.h"
#include "tb.h"

//main.cpp

// SYSTEM module, instantiating MEM_SYSTEM and testbench
SC_MODULE(SYSTEM) {
    MEM_SYSTEM *mem_system;
    testbench *tb;
    sc_clock clk_sig{"clk_sig", 10, SC_NS};
    sc_signal<bool> rst_sig;

    SC_CTOR(SYSTEM) : clk_sig("clk_sig", 10, SC_NS) {
        // Instantiate testbench
        tb = new testbench("tb");
        tb->clk(clk_sig);
        tb->rst(rst_sig);

        // Instantiate MEM_SYSTEM
        mem_system = new MEM_SYSTEM("mem_system");
        mem_system->clk(clk_sig);
        mem_system->rst(rst_sig);

        // Connect testbench to MEM_SYSTEM (CORE interface)
        for (int i = 0; i < CORE_NUM; i++) {
            //mem_system->ready_core[i](tb->ready_core[i]);
            mem_system->valid_core[i](tb->valid_core[i]);
            mem_system->RorWreq_core[i](tb->RorWreq_core[i]);
            mem_system->addr_core[i](tb->addr_core[i]);
            mem_system->data_core[i](tb->data_core[i]);
            mem_system->req_ts_core[i](tb->req_ts_core[i]);

            // Connect MEM_SYSTEM output to testbench input
            //tb->ready_cache_to_core[i](mem_system->ready_cache_to_core[i]);
            tb->valid_cache_to_core[i](mem_system->valid_cache_to_core[i]);
            tb->data_cache_to_core[i](mem_system->data_cache_to_core[i]);
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
