#ifndef __COMBUF_H
#define __COMBUF_H

//!Data buffer.
class CommonBuf {
public:
    //------ Define common data buffer size  ------//
    enum {                                        // keep a 256K common buffer for all temporary memory storage like reading files temporarily into memory
	COMMON_DATA_BUF_SIZE = 256*1024
    };

public:
    char*   common_data_buf;

public:
    CommonBuf();
    ~CommonBuf();
};

extern CommonBuf common_buf;
#endif
