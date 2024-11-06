#ifndef PRIVATECACHE_H
#define PRIVATECACHE_H

#include "SysConf.h"

struct STR_CacheLine {
    sc_int<2> LineState;
    sc_int<32> rts;
    sc_int<32> wts;
    sc_int<32> tag;
    sc_int<32> data[LINE_SIZE];
};

class PCache {
public:
    sc_int<32> pts;
    STR_CacheLine Line[SET_SIZE][WAY_CNT];

    //Initialize
    PCache() {
        pts = 1;
        for (auto &i: Line) {
            for (auto &j: i) {
                j.LineState = STATE_Invalid;
                j.rts = 0;
                j.wts = 0;
                j.tag = 0;
                for (auto &k: j.data) {
                    k = 0;
                }
            }
        }
    }
};


SC_MODULE(Cache) {
    //////////INPUT AND OUT PUT AREA///////////
    sc_in<bool> clk;
    sc_in<bool> rst;

    ////CORE TO CACHE////
    //sc_in<bool> ready_core;
    sc_in<bool> valid_core;
    sc_in<sc_int<3>> RorWreq_core;
    sc_in<sc_int<32>> addr_core;
    sc_in<sc_int<32>> data_core;
    sc_in<sc_int<32>> req_ts_core;

    ////CACHE TO CORE////
    //sc_out<bool> ready_cache_to_core;
    sc_out<bool> valid_cache_to_core;
    sc_out<sc_int<32>> data_cache_to_core;

    ////CACHE TO BUS////
    sc_out<bool> valid_cache_to_bus;
    sc_out<sc_int<32>> addr_cache_to_bus;
    sc_out<sc_int<5>> MSG_cache_to_bus;
    sc_out<sc_int<32>> pts_cache_to_bus;
    sc_out<sc_int<32>> rts_cache_to_bus;
    sc_out<sc_int<32>> wts_cache_to_bus;

    sc_out<sc_int<32>> data_line_cache_to_bus[LINE_SIZE];

    sc_out<sc_int<32>> req_ts_cache_to_bus;

    ////BUS TO CACHE////
    sc_in<bool> valid_bus_to_cache;
    sc_in<sc_int<32>> addr_bus_to_cache;
    sc_in<sc_int<32>> data_line_bus_to_cache[LINE_SIZE];

    sc_in<sc_int<5>> MSG_bus_to_cache;
    //sc_in<sc_int<32>> pts_bus_to_cache;
    sc_in<sc_int<32>> rts_bus_to_cache;
    sc_in<sc_int<32>> wts_bus_to_cache;

    ///////////////////////////////////////////

    PCache pc;

    // Decomposition addr
    sc_signal<sc_int<2>> word_addr;
    sc_signal<sc_int<LINE_ADDR_LEN>> line_addr;
    sc_signal<sc_int<SET_ADDR_LEN>> set_addr;
    sc_signal<sc_int<TAG_ADDR_LEN>> tag_addr;
    sc_signal<sc_int<UNUSED_ADDR_LEN>> unused_addr;

    //process reqs in serial, so these paras defined here
    sc_signal<bool> cache_hit;     //if the current req hit
    sc_signal<int> way_hit;        //if hit, the hit way
    //use FIFO
    sc_signal<int> FIFO_choice[SET_SIZE];
    sc_signal<int> wayout_choice;  //if had to replace, next out way(in fact, it's due to the set, here represent the set[way_hit]'s choice)

    //Cache State
    sc_signal<int> CacheState;


    void initialize(); //rst

    void split_address();//analyze the addr, decomposition to follows,位拆分，将 addr 的相应部分提取出来
    void judge_if_hit(); //judge a req hit/miss, and update cache_hit and way_hit
    void choose_wayout();

    void process_core_req();


    SC_CTOR(Cache) {
        cout << "constructor of CACHE" << endl;
//        cout << "Cache Module ports:" << endl;
//        cout << "clk: " << clk.name() << endl;
//        cout << "valid_core: " << valid_core.name() << endl;
//        cout << "valid_bus_to_cache: " << valid_bus_to_cache.name() << endl;
//        cout << "wts_bus_to_cache: " << wts_bus_to_cache.name() << endl;
//        cout << "data_cache_to_core: " << data_cache_to_core.name() << endl;


        // Register SC_METHOD
        SC_METHOD(split_address);
        sensitive << addr_core;  // exe when addr_core change

        SC_METHOD(judge_if_hit);
        sensitive << valid_core;

        SC_METHOD(choose_wayout);
        sensitive << valid_core;

        SC_CTHREAD(process_core_req, clk.pos());
        reset_signal_is(rst, true);
    }

};


#endif //PRIVATECACHE_H
