#include "Bus.h"

void SharedBus::bus_arbiter() {
    cout << "start bus arbitration" << endl;

    while (true) {
        wait();
        if(rst.read()){
            arbitrationResult_coreID.write(-1);
        }
        if (BusState.read() == BUS_IDLE) {
            int min_ts = INT_MAX, min_id = -1;
            for (int i = 0; i < CORE_NUM; i++) {
                if (valid_cache_to_bus[i].read()) {
                    if (req_ts_cache_to_bus[i].read() < min_ts) {
                        min_ts = req_ts_cache_to_bus[i].read();
                        min_id = i;
                    }
                }
            }

            arbitrationResult_coreID.write(min_id);
        }
    }
}


void SharedBus::bus_rst() {
    cout << "start rst all bus's signal" << endl;

    // Reset all sc_out ports
    for (int i = 0; i < CORE_NUM; i++) {
        valid_bus_to_cache[i].write(false);
        addr_bus_to_cache[i].write(0);
        MSG_bus_to_cache[i].write(MSG_NOTHING);
        rts_bus_to_cache[i].write(0);
        wts_bus_to_cache[i].write(0);
        for (int j = 0; j < LINE_SIZE; j++) {
            data_line_bus_to_cache[i][j].write(0);
        }
    }

    // Reset TM outputs
    coreID_bus_to_TM.write(-1);
    valid_bus_to_TM.write(false);
    MSG_bus_to_TM.write(MSG_NOTHING);
    addr_bus_to_TM.write(0);
    pts_bus_to_TM.write(0);
    rts_bus_to_TM.write(0);
    wts_bus_to_TM.write(0);
    for (auto &j: data_line_bus_to_TM) {
        j.write(0);
    }

    // Reset shared signals
    BusState.write(BUS_IDLE);
    //arbitrationResult_coreID.write(-1);

    cout << "Finish rst all bus's signal!" << endl;
}


void SharedBus::process_req_bus() {
    cout << "start bus process" << endl;
    bus_rst();

    while (true) {
        wait();
//        cout << endl;
        cout    << "Bus: BusState = "+ to_string(BusState.read())
                << " Bus: arbitrationResult_coreID = " + to_string(arbitrationResult_coreID.read())
                << " Bus: valid_cache_to_bus0 = " + to_string(valid_cache_to_bus[0].read())
                << " Bus: valid_cache_to_bus1 = " + to_string(valid_cache_to_bus[1].read())
                << " Bus: pts_bus_to_TM = " + to_string(pts_bus_to_TM.read())
                << " Bus: rts_bus_to_TM = " + to_string(rts_bus_to_TM.read())
                << " Bus: wts_bus_to_TM = " + to_string(wts_bus_to_TM.read())
                << endl;


        if (rst.read()) {
            bus_rst();  // if rst == true, invoke initialize func
            continue;      // if rst, skip all other codes, do nothing
        }

        switch (BusState) {
            case BUS_IDLE: {
                if (arbitrationResult_coreID.read() != -1) {
                    cout << "Info: Bus arbitration result: coreID = " + to_string(arbitrationResult_coreID.read())
                         << endl;
                    coreID_bus_to_TM.write(arbitrationResult_coreID.read());
                    valid_bus_to_TM.write(
                            valid_cache_to_bus[arbitrationResult_coreID.read()].read());
                    MSG_bus_to_TM.write(
                            MSG_cache_to_bus[arbitrationResult_coreID.read()].read());
                    addr_bus_to_TM.write(
                            addr_cache_to_bus[arbitrationResult_coreID.read()].read());
                    pts_bus_to_TM.write(
                            pts_cache_to_bus[arbitrationResult_coreID.read()].read());
                    rts_bus_to_TM.write(
                            rts_cache_to_bus[arbitrationResult_coreID.read()].read());
                    wts_bus_to_TM.write(
                            wts_cache_to_bus[arbitrationResult_coreID.read()].read());
                    for (int i = 0; i < LINE_SIZE; i++) {
                        data_line_bus_to_TM[i].write(
                                data_line_cache_to_bus[arbitrationResult_coreID.read()][i].read());
                    }
                    BusState.write(BUS_TRANS_CACHE_TO_TM_FIRSTTIME);
                } else {
                    BusState.write(BUS_IDLE);
                }
            }
                break;
            case BUS_TRANS_CACHE_TO_TM_FIRSTTIME: {
//                if(MSG_bus_to_TM.read()==MSG_FLUSH_REP || MSG_bus_to_TM.read()==MSG_WB_REP){ //finish
//
//                }

                if (valid_TM_to_bus.read()) {
                    if (MSG_TM_to_bus.read() == MSG_SH_REP || MSG_TM_to_bus.read() == MSG_EX_REP ||
                        MSG_TM_to_bus.read() == MSG_UPGRADE_REP ||
                        MSG_TM_to_bus.read() == MSG_RENEW_REP) { //not involve third cache, finish
                        valid_bus_to_cache[arbitrationResult_coreID.read()].write(valid_TM_to_bus.read());
                        addr_bus_to_cache[arbitrationResult_coreID.read()].write(addr_TM_to_bus.read());
                        MSG_bus_to_cache[arbitrationResult_coreID.read()].write(MSG_TM_to_bus.read());
                        //pts_bus_to_cache[arbitrationResult_coreID.read()].write(pts);
                        rts_bus_to_cache[arbitrationResult_coreID.read()].write(rts_TM_to_bus.read());
                        wts_bus_to_cache[arbitrationResult_coreID.read()].write(wts_TM_to_bus.read());
                        for (int i = 0; i < LINE_SIZE; i++) {
                            data_line_bus_to_cache[arbitrationResult_coreID.read()][i].write(
                                    data_line_TM_to_bus[i].read());
                        }
                        BusState.write(BUS_TRANSOK_AND_RST);
                    } else if (MSG_TM_to_bus.read() == MSG_FLUSH_REQ ||
                               MSG_TM_to_bus.read() == MSG_WB_REQ) { //another cache has to write back or flush first
                        valid_bus_to_cache[coreID_TM_to_bus.read()].write(valid_TM_to_bus.read());
                        rts_bus_to_cache[coreID_TM_to_bus.read()].write(rts_TM_to_bus.read());
                        MSG_bus_to_cache[coreID_TM_to_bus.read()].write(MSG_TM_to_bus.read());

                        MSG_bus_to_TM.write(MSG_cache_to_bus[coreID_TM_to_bus.read()].read());
                        rts_bus_to_TM.write(
                                rts_cache_to_bus[coreID_TM_to_bus.read()].read());
                        wts_bus_to_TM.write(
                                wts_cache_to_bus[coreID_TM_to_bus.read()].read());
                        for (int i = 0; i < LINE_SIZE; i++) {
                            data_line_bus_to_TM[i].write(
                                    data_line_cache_to_bus[coreID_TM_to_bus.read()][i].read());
                        }

                        BusState.write(BUS_TRANS_FLUSH_AND_WB);

                    } else {
                        cout << "ERROR: bus received unexpected MSG form TM" << endl;
                    }
                }
            }
                break;
            case BUS_TRANS_FLUSH_AND_WB: {
                if (valid_cache_to_bus[coreID_TM_to_bus.read()].read() &&
                    (MSG_bus_to_TM.read() == MSG_WB_REP || MSG_bus_to_TM.read() == MSG_FLUSH_REP)) {
                    wait();
                    //do these again
                    coreID_bus_to_TM.write(arbitrationResult_coreID.read());
                    valid_bus_to_TM.write(
                            valid_cache_to_bus[arbitrationResult_coreID.read()].read());
                    MSG_bus_to_TM.write(
                            MSG_cache_to_bus[arbitrationResult_coreID.read()].read());
                    addr_bus_to_TM.write(
                            addr_cache_to_bus[arbitrationResult_coreID.read()].read());
                    pts_bus_to_TM.write(
                            pts_cache_to_bus[arbitrationResult_coreID.read()].read());
                    rts_bus_to_TM.write(
                            rts_cache_to_bus[arbitrationResult_coreID.read()].read());
                    wts_bus_to_TM.write(
                            wts_cache_to_bus[arbitrationResult_coreID.read()].read());
                    for (int i = 0; i < LINE_SIZE; i++) {
                        data_line_bus_to_TM[i].write(
                                data_line_cache_to_bus[arbitrationResult_coreID.read()][i].read());
                    }
                    BusState.write(BUS_TRANS_CACHE_TO_TM_FIRSTTIME);
                }
            }
                break;
            case BUS_TRANSOK_AND_RST: {
                bus_rst();
            }
                break;

            default:
                cout << "ERROR: bus in a unexpected state" << endl;
        }

    }
}