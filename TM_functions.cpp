#include "TM.h"


sc_int<32> max3TM(const sc_int<32> &a, const sc_int<32> &b, const sc_int<32> &c) {
    return (a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c);
}

void TMmodule::rst_TM() {
    cout << "RUN rst_TM" << endl;
    // Reset all sc_out ports
    valid_TM_to_bus.write(false);
    coreID_TM_to_bus.write(-1);
    MSG_TM_to_bus.write(MSG_NOTHING);
    addr_TM_to_bus.write(0);
    rts_TM_to_bus.write(0);
    wts_TM_to_bus.write(0);

    for (auto &i: data_line_TM_to_bus) {
        i.write(0);
    }

    // Reset all internal signals
    word_addr_TM.write(0);
    line_addr_TM.write(0);
    set_addr_TM.write(0);
    tag_addr_TM.write(0);
    unused_addr_TM.write(0);
}


void TMmodule::split_address_TM() {
    cout << "RUN split_address_TM" << endl;
    while (true) {
        //decomposition
        word_addr_TM.write(addr_bus_to_TM.read().range(1, 0));    // 2 bits for word_addr
        line_addr_TM.write(addr_bus_to_TM.read().range(1 + LINE_ADDR_LEN, 2));    // LINE_ADDR_LEN bits for line_addr
        set_addr_TM.write(
                addr_bus_to_TM.read().range(1 + LINE_ADDR_LEN + SET_ADDR_LEN,
                                            2 + LINE_ADDR_LEN));    // SET_ADDR_LEN bits for set_addr
        tag_addr_TM.write(addr_bus_to_TM.read().range(1 + LINE_ADDR_LEN + SET_ADDR_LEN + TAG_ADDR_LEN,
                                                      2 + LINE_ADDR_LEN +
                                                      SET_ADDR_LEN));   // TAG_ADDR_LEN bits for tag_addr
        unused_addr_TM.write(
                addr_bus_to_TM.read().range(31, 32 - UNUSED_ADDR_LEN)); // UNUSED_ADDR_LEN bits for unused_addr
        wait();
    }
}


void TMmodule::process_req_TM() {
    cout << "process TM req" << endl;
//    rst_TM();


    while (true) {
        wait();  // wait for clk edge
        if (rst.read()) {
            rst_TM();  // if rst == true, invoke initialize func
            continue;      // if rst, skip all other codes, do nothing
        }

        valid_TM_to_bus.write(false);
        MSG_TM_to_bus.write(MSG_NOTHING);

        switch (MSG_bus_to_TM.read()) {
            case MSG_SH_REQ: {
                if (TM.Mem[tag_addr_TM.read()][set_addr_TM.read()].LineState == STATE_Share) { //SH_REQ+in Shared state
                    TM.Mem[tag_addr_TM.read()][set_addr_TM.read()].rts = max3TM(
                            TM.Mem[tag_addr_TM.read()][set_addr_TM.read()].rts,
                            TM.Mem[tag_addr_TM.read()][set_addr_TM.read()].wts + lease, pts_bus_to_TM.read() + lease);
                    if (wts_bus_to_TM.read() == TM.Mem[tag_addr_TM.read()][set_addr_TM.read()].wts) {
                        rts_TM_to_bus.write(TM.Mem[tag_addr_TM.read()][set_addr_TM.read()].rts);
                        MSG_TM_to_bus.write(MSG_RENEW_REP);
                        valid_TM_to_bus.write(true);

                    } else {
                        wts_TM_to_bus.write(TM.Mem[tag_addr_TM.read()][set_addr_TM.read()].wts);
                        rts_TM_to_bus.write(TM.Mem[tag_addr_TM.read()][set_addr_TM.read()].rts);
                        for (int i = 0; i < LINE_SIZE; i++) {
                            data_line_TM_to_bus[i].write(TM.Mem[tag_addr_TM.read()][set_addr_TM.read()].data[i]);
                        }
                        MSG_TM_to_bus.write(MSG_SH_REP);
                        valid_TM_to_bus.write(true);
                    }

                } else if (TM.Mem[tag_addr_TM.read()][set_addr_TM.read()].LineState ==
                           STATE_Exclusive) {//SH_REQ+in Exclusive state
                    rts_TM_to_bus.write(pts_bus_to_TM.read() + lease);
                    coreID_TM_to_bus.write(TM.Mem[tag_addr_TM.read()][set_addr_TM.read()].owner);
                    addr_TM_to_bus.write(addr_bus_to_TM.read());
                    MSG_TM_to_bus.write(MSG_WB_REQ);
                    valid_TM_to_bus.write(true);

                } else { cout << "ERROR: TM receive a MSG_SH_REQ and that line in unexpected state" << endl; }
            }
                break;
            case MSG_EX_REQ: {
                if (TM.Mem[tag_addr_TM.read()][set_addr_TM.read()].LineState ==
                    STATE_Share) { //EX_REQ + in Shared state
                    cout << "Info: TM curr ReqCoreID = " + to_string(coreID_bus_to_TM.read()) << endl;
                    TM.Mem[tag_addr_TM.read()][set_addr_TM.read()].owner = coreID_bus_to_TM.read(); //update owner
                    rts_TM_to_bus.write(TM.Mem[tag_addr_TM.read()][set_addr_TM.read()].rts);
                    TM.Mem[tag_addr_TM.read()][set_addr_TM.read()].LineState = STATE_Exclusive;
                    if (wts_bus_to_TM.read() == TM.Mem[tag_addr_TM.read()][set_addr_TM.read()].wts) {
                        MSG_TM_to_bus.write(MSG_UPGRADE_REP);
                        valid_TM_to_bus.write(true);
                    } else {
                        wts_TM_to_bus.write(TM.Mem[tag_addr_TM.read()][set_addr_TM.read()].wts);
                        for (int i = 0; i < LINE_SIZE; i++) {
                            data_line_TM_to_bus[i].write(TM.Mem[tag_addr_TM.read()][set_addr_TM.read()].data[i]);
                        }
                        MSG_TM_to_bus.write(MSG_EX_REP);
                        valid_TM_to_bus.write(true);
                    }
                } else if (TM.Mem[tag_addr_TM.read()][set_addr_TM.read()].LineState ==
                           STATE_Exclusive) {//EX_REQ + in Exclusive state
                    coreID_TM_to_bus.write(TM.Mem[tag_addr_TM.read()][set_addr_TM.read()].owner);
                    addr_TM_to_bus.write(addr_bus_to_TM.read());
                    MSG_TM_to_bus.write(MSG_FLUSH_REQ);
                    valid_TM_to_bus.write(true);

                } else { cout << "ERROR: TM receive a MSG_EX_REQ and that line in unexpected state" << endl; }
            }
                break;
            case MSG_FLUSH_REP: {
                if (TM.Mem[tag_addr_TM.read()][set_addr_TM.read()].LineState != STATE_Exclusive) {
                    cout << "ERROR: TM receive a FLUSH REP in error state! not exclusive" << endl;
                }
                TM.Mem[tag_addr_TM.read()][set_addr_TM.read()].LineState = STATE_Share;
                for (int i = 0; i < LINE_SIZE; i++) {
                    TM.Mem[tag_addr_TM.read()][set_addr_TM.read()].data[i] = data_line_bus_to_TM[i].read();
                }
                TM.Mem[tag_addr_TM.read()][set_addr_TM.read()].wts = wts_bus_to_TM.read();
                TM.Mem[tag_addr_TM.read()][set_addr_TM.read()].rts = rts_bus_to_TM.read();
                valid_TM_to_bus.write(true);
            }
                break;
            case MSG_WB_REP: {
                if (TM.Mem[tag_addr_TM.read()][set_addr_TM.read()].LineState != STATE_Exclusive) {
                    cout << "ERROR: TM receive a WB REP in error state! not exclusive" << endl;
                }
                TM.Mem[tag_addr_TM.read()][set_addr_TM.read()].LineState = STATE_Share;
                for (int i = 0; i < LINE_SIZE; i++) {
                    TM.Mem[tag_addr_TM.read()][set_addr_TM.read()].data[i] = data_line_bus_to_TM[i].read();
                }
                TM.Mem[tag_addr_TM.read()][set_addr_TM.read()].wts = wts_bus_to_TM.read();
                TM.Mem[tag_addr_TM.read()][set_addr_TM.read()].rts = rts_bus_to_TM.read();
                valid_TM_to_bus.write(true);
            }
                break;
            case MSG_NOTHING: {
                MSG_TM_to_bus.write(MSG_NOTHING);
                rst_TM();
            }
                break;

            default:
                cout << "ERROR: MSG to TM type error,extra unexpected MSG" << endl;
        }

        //wait();
    }
}