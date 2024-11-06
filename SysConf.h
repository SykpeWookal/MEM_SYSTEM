#ifndef SYSCONF_H
#define SYSCONF_H

#include "systemc.h"
#include "algorithm"

using namespace std;

//Core req type
#define CORE_NO_REQ (0)
#define CORE_R_REQ (1)
#define CORE_W_REQ (2)

//line states
#define STATE_Invalid (1)
#define STATE_Share (2)
#define STATE_Exclusive (3)

//Cache controller states
#define CACHE_IDLE (1)
#define CACHE_SWAPOUT (2)
#define CACHE_SWAPIN (3)
#define CACHE_SWAPINOK (4)

//bus controller states
#define BUS_IDLE (1)
#define BUS_TRANS_CACHE_TO_TM_FIRSTTIME (2)
#define BUS_TRANS_FLUSH_AND_WB (3)
#define BUS_TRANSOK_AND_RST (4)

//Coherence MSG type
#define MSG_NOTHING (0)
#define MSG_SH_REQ (1)
#define MSG_SH_REP (2)
#define MSG_EX_REQ (3)
#define MSG_EX_REP (4)
#define MSG_FLUSH_REQ (5)
#define MSG_FLUSH_REP (6)
#define MSG_WB_REQ (7)
#define MSG_WB_REP (8)
#define MSG_UPGRADE_REP (9)
#define MSG_RENEW_REP (10)

////define parameters,config area////
constexpr int CORE_NUM = 2;

constexpr int LINE_ADDR_LEN = 2;   // line's addr len，if==3 means each line has 2^3 word
constexpr int SET_ADDR_LEN = 3;   // set's addr len，if==3 means there are 2^3=8 sets total
constexpr int TAG_ADDR_LEN = 5;  // tag's addr len
constexpr int WAY_CNT = 1;   // Set-associative  组相连度

constexpr int lease = 10;


/////////////////////////////////



//Calculate paras
constexpr int LINE_SIZE = 1 << LINE_ADDR_LEN;   // size = 2^(addr len)
constexpr int SET_SIZE = 1 << SET_ADDR_LEN;
constexpr int TAG_SIZE = 1 << TAG_ADDR_LEN;

constexpr int UNUSED_ADDR_LEN = 32 - TAG_ADDR_LEN - SET_ADDR_LEN - LINE_ADDR_LEN - 2;


#endif //SYSCONF_H
