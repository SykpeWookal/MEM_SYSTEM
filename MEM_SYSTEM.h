#include "SysConf.h"
#include "PrivateCache.h"
#include "Bus.h"
#include "TM.h"


// MEM_SYSTEM 模块，包含 caches、shared bus 和 TMmodule
//SC_MODULE(MEM_SYSTEM) {
//    Cache *caches[CORE_NUM];
//    SharedBus *sharedBus;
//    TMmodule *TM;
//
//    sc_in<bool> clk;
//    sc_in<bool> rst;
//
//    // 外部 CORE 接口信号（连接到 testbench）
//    //sc_in<bool> ready_core[CORE_NUM];
//    sc_in<bool> valid_core[CORE_NUM];
//    sc_in<sc_int<2>> RorWreq_core[CORE_NUM];
//    sc_in<sc_int<32>> addr_core[CORE_NUM];
//    sc_in<sc_int<32>> data_core[CORE_NUM];
//    sc_in<sc_int<32>> req_ts_core[CORE_NUM];
//
//    //sc_out<bool> ready_cache_to_core[CORE_NUM];
//    sc_out<bool> valid_cache_to_core[CORE_NUM];
//    sc_out<sc_int<32>> data_cache_to_core[CORE_NUM];
//
//    SC_CTOR(MEM_SYSTEM) {
//        // 实例化 caches 并连接到 CORE 和 shared bus
//        for (int i = 0; i < CORE_NUM; i++) {
//            caches[i] = new Cache(sc_gen_unique_name("cache"));
//            caches[i]->clk(clk);
//            caches[i]->rst(rst);
//
//            // 连接 testbench 到 Cache（CORE 接口）
//            //caches[i]->ready_core(ready_core[i]);
//            caches[i]->valid_core(valid_core[i]);
//            caches[i]->RorWreq_core(RorWreq_core[i]);
//            caches[i]->addr_core(addr_core[i]);
//            caches[i]->data_core(data_core[i]);
//            caches[i]->req_ts_core(req_ts_core[i]);
//
//            // 连接 Cache 输出到 testbench 输入
//            //ready_cache_to_core[i](caches[i]->ready_cache_to_core);
//            valid_cache_to_core[i](caches[i]->valid_cache_to_core);
//            data_cache_to_core[i](caches[i]->data_cache_to_core);
//        }
//
//        // 实例化 shared bus 和 time manager
//        sharedBus = new SharedBus("sharedBus");
//        sharedBus->clk(clk);
//        sharedBus->rst(rst);
//
//        TM = new TMmodule("TMmodule");
//        TM->clk(clk);
//        TM->rst(rst);
//
//        // 将 Cache 连接到 SharedBus（BUS 接口）
//        for (int i = 0; i < CORE_NUM; i++) {
//            sharedBus->valid_cache_to_bus[i](caches[i]->valid_cache_to_bus);
//            sharedBus->addr_cache_to_bus[i](caches[i]->addr_cache_to_bus);
//            sharedBus->MSG_cache_to_bus[i](caches[i]->MSG_cache_to_bus);
//            sharedBus->pts_cache_to_bus[i](caches[i]->pts_cache_to_bus);
//            sharedBus->rts_cache_to_bus[i](caches[i]->rts_cache_to_bus);
//            sharedBus->wts_cache_to_bus[i](caches[i]->wts_cache_to_bus);
//            for (int j = 0; j < LINE_SIZE; j++) {
//                sharedBus->data_line_cache_to_bus[i][j](caches[i]->data_line_cache_to_bus[j]);
//            }
//            //sharedBus->data_line_cache_to_bus[i](caches[i]->data_line_cache_to_bus);
//            sharedBus->req_ts_cache_to_bus[i](caches[i]->req_ts_cache_to_bus);
//
//            // 将 SharedBus 输出连接到 Cache 输入
//            caches[i]->valid_bus_to_cache(sharedBus->valid_bus_to_cache[i]);
//            caches[i]->addr_bus_to_cache(sharedBus->addr_bus_to_cache[i]);
//            caches[i]->MSG_bus_to_cache(sharedBus->MSG_bus_to_cache[i]);
//            caches[i]->rts_bus_to_cache(sharedBus->rts_bus_to_cache[i]);
//            caches[i]->wts_bus_to_cache(sharedBus->wts_bus_to_cache[i]);
//            for (int j = 0; j < LINE_SIZE; j++) {
//                caches[i]->data_line_bus_to_cache[j](sharedBus->data_line_bus_to_cache[i][j]);
//            }
//            //caches[i]->data_line_bus_to_cache(sharedBus->data_line_bus_to_cache[i]);
//        }
//
//        // 将 TM 连接到 SharedBus
//        sharedBus->valid_TM_to_bus(TM->valid_TM_to_bus);
//        sharedBus->coreID_TM_to_bus(TM->coreID_TM_to_bus);
//        sharedBus->MSG_TM_to_bus(TM->MSG_TM_to_bus);
//        sharedBus->addr_TM_to_bus(TM->addr_TM_to_bus);
//        for (int i = 0; i < LINE_SIZE; i++) {
//            sharedBus->data_line_TM_to_bus[i](TM->data_line_TM_to_bus[i]);
//        }
//        sharedBus->rts_TM_to_bus(TM->rts_TM_to_bus);
//        sharedBus->wts_TM_to_bus(TM->wts_TM_to_bus);
//
//        //sharedBus->data_line_TM_to_bus(TM->data_line_TM_to_bus);
//
//        TM->valid_bus_to_TM(sharedBus->valid_bus_to_TM);
//        TM->coreID_bus_to_TM(sharedBus->coreID_bus_to_TM);
//        TM->MSG_bus_to_TM(sharedBus->MSG_bus_to_TM);
//        TM->addr_bus_to_TM(sharedBus->addr_bus_to_TM);
//        for (int i = 0; i < LINE_SIZE; i++) {
//            TM->data_line_bus_to_TM[i](sharedBus->data_line_bus_to_TM[i]);
//        }
//        TM->pts_bus_to_TM(sharedBus->pts_bus_to_TM);
//        TM->rts_bus_to_TM(sharedBus->rts_bus_to_TM);
//        TM->wts_bus_to_TM(sharedBus->wts_bus_to_TM);
//
//        //TM->data_line_bus_to_TM(sharedBus->data_line_bus_to_TM);
//    }
//
//    ~MEM_SYSTEM() {
//        for (int i = 0; i < CORE_NUM; i++) delete caches[i];
//        delete sharedBus;
//        delete TM;
//    }
//};

SC_MODULE(MEM_SYSTEM) {
    // 定义Cache、Bus、TM模块
    Cache *caches[CORE_NUM];
    SharedBus *sharedBus;
    TMmodule *TM;

    // 定义MEM_SYSTEM模块的输入输出接口（连接到testbench的CORE接口）
    sc_in<bool> clk;
    sc_in<bool> rst;

    // CORE 到 MEM_SYSTEM 的信号
    sc_in<bool> valid_core[CORE_NUM];
    sc_in<sc_int<3>> RorWreq_core[CORE_NUM];
    sc_in<sc_int<32>> addr_core[CORE_NUM];
    sc_in<sc_int<32>> data_core[CORE_NUM];
    sc_in<sc_int<32>> req_ts_core[CORE_NUM];

    // MEM_SYSTEM 到 CORE 的信号
    sc_out<bool> valid_cache_to_core[CORE_NUM];
    sc_out<sc_int<32>> data_cache_to_core[CORE_NUM];

    // 中间信号：用于连接Cache和SharedBus
    sc_signal<bool> valid_cache_to_bus[CORE_NUM];
    sc_signal<sc_int<32>> addr_cache_to_bus[CORE_NUM];
    sc_signal<sc_int<5>> MSG_cache_to_bus[CORE_NUM];
    sc_signal<sc_int<32>> pts_cache_to_bus[CORE_NUM];
    sc_signal<sc_int<32>> rts_cache_to_bus[CORE_NUM];
    sc_signal<sc_int<32>> wts_cache_to_bus[CORE_NUM];
    sc_signal<sc_int<32>> data_line_cache_to_bus[CORE_NUM][LINE_SIZE];
    sc_signal<sc_int<32>> req_ts_cache_to_bus[CORE_NUM];

    sc_signal<bool> valid_bus_to_cache[CORE_NUM];
    sc_signal<sc_int<32>> addr_bus_to_cache[CORE_NUM];
    sc_signal<sc_int<5>> MSG_bus_to_cache[CORE_NUM];
    sc_signal<sc_int<32>> rts_bus_to_cache[CORE_NUM];
    sc_signal<sc_int<32>> wts_bus_to_cache[CORE_NUM];
    sc_signal<sc_int<32>> data_line_bus_to_cache[CORE_NUM][LINE_SIZE];

    // 中间信号：用于连接SharedBus和TMmodule
    sc_signal<sc_int<32>> coreID_TM_to_bus;
    sc_signal<bool> valid_TM_to_bus;
    sc_signal<sc_int<5>> MSG_TM_to_bus;
    sc_signal<sc_int<32>> addr_TM_to_bus;
    sc_signal<sc_int<32>> data_line_TM_to_bus[LINE_SIZE];
    sc_signal<sc_int<32>> rts_TM_to_bus;
    sc_signal<sc_int<32>> wts_TM_to_bus;

    sc_signal<sc_int<32>> coreID_bus_to_TM;
    sc_signal<bool> valid_bus_to_TM;
    sc_signal<sc_int<5>> MSG_bus_to_TM;
    sc_signal<sc_int<32>> addr_bus_to_TM;
    sc_signal<sc_int<32>> pts_bus_to_TM;
    sc_signal<sc_int<32>> rts_bus_to_TM;
    sc_signal<sc_int<32>> wts_bus_to_TM;
    sc_signal<sc_int<32>> data_line_bus_to_TM[LINE_SIZE];

    SC_CTOR(MEM_SYSTEM) {
        // 实例化 caches 并连接到 SharedBus 和 MEM_SYSTEM
        for (int i = 0; i < CORE_NUM; i++) {
            caches[i] = new Cache(sc_gen_unique_name("cache"));
            caches[i]->clk(clk);
            caches[i]->rst(rst);

            // 连接 CORE 接口到 Cache
            caches[i]->valid_core(valid_core[i]);
            caches[i]->RorWreq_core(RorWreq_core[i]);
            caches[i]->addr_core(addr_core[i]);
            caches[i]->data_core(data_core[i]);
            caches[i]->req_ts_core(req_ts_core[i]);

            // 连接 Cache 到 CORE 接口
//            valid_cache_to_core[i](caches[i]->valid_cache_to_core);
//            data_cache_to_core[i](caches[i]->data_cache_to_core);

            caches[i]->valid_cache_to_core(valid_cache_to_core[i]);
            caches[i]->data_cache_to_core(data_cache_to_core[i]);

            // 连接 Cache 到 SharedBus
            caches[i]->valid_cache_to_bus(valid_cache_to_bus[i]);
            caches[i]->addr_cache_to_bus(addr_cache_to_bus[i]);
            caches[i]->MSG_cache_to_bus(MSG_cache_to_bus[i]);
            caches[i]->pts_cache_to_bus(pts_cache_to_bus[i]);
            caches[i]->rts_cache_to_bus(rts_cache_to_bus[i]);
            caches[i]->wts_cache_to_bus(wts_cache_to_bus[i]);
            caches[i]->req_ts_cache_to_bus(req_ts_cache_to_bus[i]);
            for (int j = 0; j < LINE_SIZE; j++) {
                caches[i]->data_line_cache_to_bus[j](data_line_cache_to_bus[i][j]);
            }

            // 连接 SharedBus 到 Cache
            caches[i]->valid_bus_to_cache(valid_bus_to_cache[i]);
            caches[i]->addr_bus_to_cache(addr_bus_to_cache[i]);
            caches[i]->MSG_bus_to_cache(MSG_bus_to_cache[i]);
            caches[i]->rts_bus_to_cache(rts_bus_to_cache[i]);
            caches[i]->wts_bus_to_cache(wts_bus_to_cache[i]);
            for (int j = 0; j < LINE_SIZE; j++) {
                caches[i]->data_line_bus_to_cache[j](data_line_bus_to_cache[i][j]);
            }
        }

        // 实例化 SharedBus 并连接到 TMmodule
        sharedBus = new SharedBus("sharedBus");
        sharedBus->clk(clk);
        sharedBus->rst(rst);

        for (int i = 0; i < CORE_NUM; i++) {
            sharedBus->valid_cache_to_bus[i](valid_cache_to_bus[i]);
            sharedBus->addr_cache_to_bus[i](addr_cache_to_bus[i]);
            sharedBus->MSG_cache_to_bus[i](MSG_cache_to_bus[i]);
            sharedBus->pts_cache_to_bus[i](pts_cache_to_bus[i]);
            sharedBus->rts_cache_to_bus[i](rts_cache_to_bus[i]);
            sharedBus->wts_cache_to_bus[i](wts_cache_to_bus[i]);
            sharedBus->req_ts_cache_to_bus[i](req_ts_cache_to_bus[i]);
            for (int j = 0; j < LINE_SIZE; j++) {
                sharedBus->data_line_cache_to_bus[i][j](data_line_cache_to_bus[i][j]);
            }

            sharedBus->valid_bus_to_cache[i](valid_bus_to_cache[i]);
            sharedBus->addr_bus_to_cache[i](addr_bus_to_cache[i]);
            sharedBus->MSG_bus_to_cache[i](MSG_bus_to_cache[i]);
            sharedBus->rts_bus_to_cache[i](rts_bus_to_cache[i]);
            sharedBus->wts_bus_to_cache[i](wts_bus_to_cache[i]);
            for (int j = 0; j < LINE_SIZE; j++) {
                sharedBus->data_line_bus_to_cache[i][j](data_line_bus_to_cache[i][j]);
            }
        }

        // 连接 SharedBus 和 TMmodule
        sharedBus->coreID_TM_to_bus(coreID_TM_to_bus);
        sharedBus->valid_TM_to_bus(valid_TM_to_bus);
        sharedBus->MSG_TM_to_bus(MSG_TM_to_bus);
        sharedBus->addr_TM_to_bus(addr_TM_to_bus);
        sharedBus->rts_TM_to_bus(rts_TM_to_bus);
        sharedBus->wts_TM_to_bus(wts_TM_to_bus);
        for (int i = 0; i < LINE_SIZE; i++) {
            sharedBus->data_line_TM_to_bus[i](data_line_TM_to_bus[i]);
        }

        sharedBus->coreID_bus_to_TM(coreID_bus_to_TM);
        sharedBus->valid_bus_to_TM(valid_bus_to_TM);
        sharedBus->MSG_bus_to_TM(MSG_bus_to_TM);
        sharedBus->addr_bus_to_TM(addr_bus_to_TM);
        sharedBus->pts_bus_to_TM(pts_bus_to_TM);
        sharedBus->rts_bus_to_TM(rts_bus_to_TM);
        sharedBus->wts_bus_to_TM(wts_bus_to_TM);
        for (int i = 0; i < LINE_SIZE; i++) {
            sharedBus->data_line_bus_to_TM[i](data_line_bus_to_TM[i]);
        }

        // 实例化 TMmodule
        TM = new TMmodule("TMmodule");
        TM->clk(clk);
        TM->rst(rst);

        TM->coreID_bus_to_TM(coreID_bus_to_TM);
        TM->valid_bus_to_TM(valid_bus_to_TM);
        TM->MSG_bus_to_TM(MSG_bus_to_TM);
        TM->addr_bus_to_TM(addr_bus_to_TM);
        TM->pts_bus_to_TM(pts_bus_to_TM);
        TM->rts_bus_to_TM(rts_bus_to_TM);
        TM->wts_bus_to_TM(wts_bus_to_TM);
        for (int i = 0; i < LINE_SIZE; i++) {
            TM->data_line_bus_to_TM[i](data_line_bus_to_TM[i]);
        }

        TM->coreID_TM_to_bus(coreID_TM_to_bus);
        TM->valid_TM_to_bus(valid_TM_to_bus);
        TM->MSG_TM_to_bus(MSG_TM_to_bus);
        TM->addr_TM_to_bus(addr_TM_to_bus);
        TM->rts_TM_to_bus(rts_TM_to_bus);
        TM->wts_TM_to_bus(wts_TM_to_bus);
        for (int i = 0; i < LINE_SIZE; i++) {
            TM->data_line_TM_to_bus[i](data_line_TM_to_bus[i]);
        }
    }

    ~MEM_SYSTEM() {
        for (int i = 0; i < CORE_NUM; i++) {
            delete caches[i];
        }
        delete sharedBus;
        delete TM;
    }
};