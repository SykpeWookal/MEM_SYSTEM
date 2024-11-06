#include "PrivateCache.h"


sc_int<32> max2(const sc_int<32> &a, const sc_int<32> &b) {
    return (a > b) ? a : b;
}

sc_int<32> max3(const sc_int<32> &a, const sc_int<32> &b, const sc_int<32> &c) {
    return (a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c);
}

void Cache::initialize() {
    //rst
    CacheState.write(CACHE_IDLE);
    //cache_hit.write(false);         // rst cache_hit
    //way_hit.write(0);              // rst way_hit
//    for (auto &i: FIFO_choice) {
//        i.write(0);    // rst FIFO_choice
//    }
    //wayout_choice.write(0);         // rst wayout_choice

    // rst PCache
    pc.pts = 1;                      // rst PCache's pts
    for (auto &i: pc.Line) {
        for (auto &j: i) {
            j.LineState = STATE_Invalid;  // RST LineState
            j.tag = 0;                    // rst tag
            j.rts = 0;                    // rst rts
            j.wts = 0;                    // rst wts
            for (auto &k: j.data) { //rst data
                k = 0;
            }
        }
    }
    cout<<"Cache rst finish!"<<endl;
}


void Cache::split_address() {
    sc_int<32> addr = addr_core.read();  // decomposition input addr

    //decomposition
    word_addr.write(addr.range(1, 0));    // 2 bits for word_addr
    line_addr.write(addr.range(1 + LINE_ADDR_LEN, 2));    // LINE_ADDR_LEN bits for line_addr
    set_addr.write(
            addr.range(1 + LINE_ADDR_LEN + SET_ADDR_LEN, 2 + LINE_ADDR_LEN));    // SET_ADDR_LEN bits for set_addr
    tag_addr.write(addr.range(1 + LINE_ADDR_LEN + SET_ADDR_LEN + TAG_ADDR_LEN,
                              2 + LINE_ADDR_LEN + SET_ADDR_LEN));   // TAG_ADDR_LEN bits for tag_addr
    unused_addr.write(addr.range(31, 32 - UNUSED_ADDR_LEN)); // UNUSED_ADDR_LEN bits for unused_addr
}

void Cache::judge_if_hit() {
    cache_hit.write(false);
    for (int i = 0; i < WAY_CNT; i++) {
        if (((pc.Line[set_addr.read()][i].LineState == STATE_Exclusive) ||
             (pc.Line[set_addr.read()][i].LineState == STATE_Share && pc.Line[set_addr.read()][i].rts >= pc.pts) &&
             RorWreq_core.read() == CORE_R_REQ)
            && pc.Line[set_addr.read()][i].tag == tag_addr.read()
                ) {
            cache_hit.write(true);
            way_hit.write(i);
        }
    }
}

void Cache::choose_wayout() {
    if (CacheState.read() == CACHE_IDLE)
        wayout_choice.write(
                FIFO_choice[set_addr.read()]); //FIFO choice records each set's way NO. that will be replaced
    if (CacheState.read() == CACHE_SWAPINOK)
        FIFO_choice[set_addr.read()] =
                (FIFO_choice[set_addr.read()] + 1) % WAY_CNT;  //after swap in, have to update FIFO_choice
        //FIFO: round-robin queue
    else;
}


void Cache::process_core_req() {
    cout << "pros core req" << endl;
    initialize();
    int find_bus_req_way;
    sc_int<SET_ADDR_LEN> mem_side_set_addr;//not a signal, update immediately
    sc_int<TAG_ADDR_LEN> mem_side_tag_addr;
    while (true) {
        wait();  // wait for clk edge
        cout    << "Cache: rst = "+ to_string(rst.read())
                << " Cache: valid_core = " + to_string(valid_core.read())
                << " Cache: RorWreq_core = " + to_string(RorWreq_core.read())
                << " Cache: addr_core = " + to_string(addr_core.read())
                << " Cache: req_ts_core = " + to_string(req_ts_core.read())
                << " Cache: CacheState = " + to_string(CacheState.read())
                << endl;
        cout    << "Cache to bus: valid_cache_to_bus = "+ to_string(valid_cache_to_bus.read())
                << " Cache to bus: addr_cache_to_bus = " + to_string(addr_cache_to_bus.read())
                << " Cache to bus: MSG_cache_to_bus = " + to_string(MSG_cache_to_bus.read())
                << " Cache to bus: pts_cache_to_bus = " + to_string(pts_cache_to_bus.read())
                << " Cache to bus: rts_cache_to_bus = " + to_string(rts_cache_to_bus.read())
                << " Cache to bus: wts_cache_to_bus = " + to_string(wts_cache_to_bus.read())
                << endl;
        if (rst.read()) {
            initialize();  // if rst == true, invoke initialize func
            continue;      // if rst, skip all other codes, do nothing
        }

        req_ts_cache_to_bus.write(req_ts_core);//always equal to input ts, only effect when valid in high level
        //initial valid, only in special condition valid can be high level
        valid_cache_to_bus.write(false);
        valid_cache_to_core.write(false);

        mem_side_set_addr = addr_bus_to_cache.read().range(1 + LINE_ADDR_LEN + SET_ADDR_LEN,
                                                           2 + LINE_ADDR_LEN);//update mem side addr
        mem_side_tag_addr = addr_bus_to_cache.read().range(1 + LINE_ADDR_LEN + SET_ADDR_LEN + TAG_ADDR_LEN,
                                                           2 + LINE_ADDR_LEN +
                                                           SET_ADDR_LEN);   // TAG_ADDR_LEN bits for tag_addr

        ////////////main logic,first deal with other core's flush or wb req,then deal own req////////////////////////////////
        if ((MSG_bus_to_cache.read() == MSG_FLUSH_REQ || MSG_bus_to_cache.read() == MSG_WB_REQ) &&
            valid_bus_to_cache.read() && CacheState.read() != CACHE_SWAPINOK) {  //deal with other core's req first
            CacheState.write(
                    CACHE_IDLE);//if response for other core, need recover the state of own reqs. after finish other core's req, do own req again
            find_bus_req_way = -1;
            for (int i = 0; i < WAY_CNT; i++) {
                if (pc.Line[mem_side_set_addr][i].tag == tag_addr.read() &&
                    pc.Line[mem_side_set_addr][i].LineState == STATE_Exclusive) {
                    find_bus_req_way = i;
                }
            }
            if (find_bus_req_way == -1) { cout << "ERROR: bus send a flush or wb req to a non exclusive line" << endl; }
            if (MSG_bus_to_cache.read() == MSG_FLUSH_REQ) {
                pc.Line[mem_side_set_addr][find_bus_req_way].LineState = STATE_Invalid;
                addr_cache_to_bus.write(addr_bus_to_cache.read());//addr
                wts_cache_to_bus.write(pc.Line[mem_side_set_addr][find_bus_req_way].wts);//wts
                rts_cache_to_bus.write(pc.Line[mem_side_set_addr][find_bus_req_way].rts);//rts
                MSG_cache_to_bus.write(MSG_FLUSH_REP);
                for (int i = 0; i < LINE_SIZE; i++) {
                    data_line_cache_to_bus[i].write(pc.Line[mem_side_set_addr][find_bus_req_way].data[i]);
                }
                valid_cache_to_bus.write(true);
            } else if (MSG_bus_to_cache.read() == MSG_WB_REQ) {
                pc.Line[mem_side_set_addr][find_bus_req_way].LineState = STATE_Share;
                pc.Line[mem_side_set_addr][find_bus_req_way].rts = max3(
                        pc.Line[mem_side_set_addr][find_bus_req_way].rts,
                        pc.Line[mem_side_set_addr][find_bus_req_way].wts +
                        lease,
                        rts_bus_to_cache.read());
                wts_cache_to_bus.write(pc.Line[mem_side_set_addr][find_bus_req_way].wts);
                rts_cache_to_bus.write(pc.Line[mem_side_set_addr][find_bus_req_way].rts);
                MSG_cache_to_bus.write(MSG_WB_REP);
                for (int i = 0; i < LINE_SIZE; i++) {
                    data_line_cache_to_bus[i].write(pc.Line[mem_side_set_addr][find_bus_req_way].data[i]);
                }
                valid_cache_to_bus.write(true);

            } else {
                cout << "dealing other core's req IF error" << endl;
            }
            /////here start processing own reqs////
        } else { //then deal with own req
            switch (CacheState) {
                case CACHE_IDLE: {
                    if (cache_hit.read()) { //if hit
                        if (RorWreq_core.read() == CORE_R_REQ) { //hit and a read
                            if (pc.Line[set_addr.read()][way_hit].LineState == STATE_Share) { //hit, shared, pts<=rts
                                pc.pts = max2(pc.pts, pc.Line[set_addr.read()][way_hit].wts);
                            }
                            if (pc.Line[set_addr.read()][way_hit].LineState ==
                                STATE_Exclusive) {//hit, Exclusive
                                pc.pts = max2(pc.pts, pc.Line[set_addr.read()][way_hit].wts);
                                pc.Line[set_addr.read()][way_hit].wts = max2(pc.pts,
                                                                             pc.Line[set_addr.read()][way_hit].rts);
                            }
                            data_cache_to_core.write(
                                    pc.Line[set_addr.read()][way_hit].data[line_addr.read()]); //output read data to core
                        }
                        if (RorWreq_core.read() == CORE_W_REQ) { //hit and a write req, must in Exclusive state
                            pc.Line[set_addr.read()][way_hit].data[line_addr.read()] = data_core.read();
                            pc.pts = max2(pc.pts, pc.Line[set_addr.read()][way_hit].rts + 1);
                            pc.Line[set_addr.read()][way_hit].wts = pc.pts;
                            pc.Line[set_addr.read()][way_hit].rts = pc.pts;
                            //pc.Line[set_addr.read()][way_hit].LineState = STATE_Exclusive;
                        }
                        valid_cache_to_core.write(true); //finish! write valid 1 cycle true to core
                    } else {   //if miss
                        if (RorWreq_core.read() !=
                            CORE_NO_REQ) { //miss and truly have a req, so have to swap out a line, if dirty, write it back first
                            req_ts_cache_to_bus.write(req_ts_core.read());
                            addr_cache_to_bus.write(addr_core);
                            if (pc.Line[set_addr.read()][wayout_choice].LineState ==
                                STATE_Exclusive) {//judge wayout_choice way dirty?/clean?
                                //exclusive and eviction
                                ////////////////HERE  mem req generation////////
                                pc.Line[set_addr.read()][wayout_choice].LineState = STATE_Invalid;
                                MSG_cache_to_bus.write(MSG_FLUSH_REP);
                                wts_cache_to_bus.write(pc.Line[set_addr.read()][wayout_choice].wts);
                                rts_cache_to_bus.write(pc.Line[set_addr.read()][wayout_choice].rts);
                                addr_cache_to_bus.write(addr_core);
                                for (int i = 0; i < LINE_SIZE; i++) {
                                    data_line_cache_to_bus[i].write(pc.Line[set_addr.read()][wayout_choice].data[i]);
                                }

                                CacheState.write(CACHE_SWAPOUT);
                            } else { // miss and in I/S state
                                switch (RorWreq_core.read()) {  //miss, load/store
                                    case CORE_R_REQ: {
                                        valid_cache_to_bus.write(true);
                                        if (pc.Line[set_addr.read()][way_hit].LineState == STATE_Invalid) {
                                            MSG_cache_to_bus.write(MSG_SH_REQ);
                                            wts_cache_to_bus.write(0);
                                            pts_cache_to_bus.write(pc.pts);
                                        } else if (pc.Line[set_addr.read()][way_hit].LineState == STATE_Share) {
                                            MSG_cache_to_bus.write(MSG_SH_REQ);
                                            wts_cache_to_bus.write(pc.Line[set_addr.read()][way_hit].wts);
                                            pts_cache_to_bus.write(pc.pts);
                                        } else {
                                            cout << "req miss and in I/S state case CORE_R_REQ if error" << endl;
                                        }


                                    }
                                        break;
                                    case CORE_W_REQ: {
                                        valid_cache_to_bus.write(true);
                                        if (pc.Line[set_addr.read()][way_hit].LineState == STATE_Invalid) {
                                            MSG_cache_to_bus.write(MSG_EX_REQ);
                                            wts_cache_to_bus.write(0);
                                        } else if (pc.Line[set_addr.read()][way_hit].LineState == STATE_Share) {
                                            MSG_cache_to_bus.write(MSG_EX_REQ);
                                            wts_cache_to_bus.write(pc.Line[set_addr.read()][way_hit].wts);
                                        } else {
                                            cout << "req miss and in I/S state case CORE_W_REQ if error" << endl;
                                        }
                                    }
                                        break;
                                    default:
                                        cout << "req miss and in I/S state SWITCH error" << endl;
                                }
                                CacheState.write(CACHE_SWAPIN);
                            }
                        }
                    }
                }
                    break;

                case CACHE_SWAPOUT: {
                    valid_cache_to_bus.write(
                            true); //this state need generate a bus req, so valid cache to bus has to be high

                    if (valid_bus_to_cache.read()) {
                        CacheState.write(CACHE_IDLE);//After swap out, the line trans to I state. reprocess the req.
                    }
                }
                    break;

                case CACHE_SWAPIN: {
                    valid_cache_to_bus.write(true); //here need to generate a req on bus, so let the valid be true
                    if (valid_bus_to_cache.read()) {
                        switch (MSG_bus_to_cache.read()) {
                            case MSG_SH_REP: {
                                pc.Line[set_addr.read()][wayout_choice].wts = wts_bus_to_cache.read();
                                pc.Line[set_addr.read()][wayout_choice].rts = rts_bus_to_cache.read();
                                pc.Line[set_addr.read()][wayout_choice].LineState = STATE_Share;
                                CacheState.write(CACHE_SWAPINOK);
                            }
                                break;
                            case MSG_EX_REP: {
                                pc.Line[set_addr.read()][wayout_choice].wts = wts_bus_to_cache.read();
                                pc.Line[set_addr.read()][wayout_choice].rts = rts_bus_to_cache.read();
                                pc.Line[set_addr.read()][wayout_choice].LineState = STATE_Exclusive;
                                CacheState.write(CACHE_SWAPINOK);
                            }
                                break;
                            case MSG_RENEW_REP: {
                                pc.Line[set_addr.read()][wayout_choice].rts = rts_bus_to_cache.read();
                                CacheState.write(CACHE_IDLE);
                            }
                                break;
                            case MSG_UPGRADE_REP: {
                                pc.Line[set_addr.read()][wayout_choice].rts = rts_bus_to_cache.read();
                                pc.Line[set_addr.read()][wayout_choice].LineState = STATE_Exclusive;
                                CacheState.write(CACHE_IDLE);
                            }
                                break;
                            default:
                                cout << "ERROR: response SH,EX,RENEW,UPG REP msg error" << endl;
                        }
                        //CacheState.write(CACHE_SWAPINOK);
                    }
                }
                    break;

                case CACHE_SWAPINOK: {// last cycle swap in a new line, here write the line to cache and update tag and state
                    for (int i = 0; i < LINE_SIZE; i++) {
                        pc.Line[set_addr.read()][wayout_choice].data[i] = data_line_bus_to_cache[i].read();
                    }
                    pc.Line[set_addr.read()][wayout_choice].tag = tag_addr.read();
                    //pc.Line[set_addr.read()][wayout_choice].LineState = STATE_Share;
                    CacheState.write(CACHE_IDLE);
                }
                    break;

                default:
                    cout << "main process of dealing own req SWITCH error" << endl;
            }
        }

        //wait();
    }
}

