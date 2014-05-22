#include <OCOMBUF.H>
#include <ALL.H>

//----------- CommonBuf::CommonBuf() -----------------//
CommonBuf::CommonBuf() {
    common_data_buf = mem_add(COMMON_DATA_BUF_SIZE);
}

//----------- CommonBuf::CommonBuf() -----------------//

//----------- CommonBuf::~CommonBuf() ----------------//
CommonBuf::~CommonBuf() {
    mem_del(common_data_buf);
}

//----------- CommonBuf::~CommonBuf() ----------------//
