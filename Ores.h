//Filename    : ORES.H
//Description : Header file of Object Resource

#ifndef __ORES_H
#define __ORES_H

#ifndef __OFILE_H
#include <OFILE.H>
#endif

//--------- Define class Resource ----------//

//!Implemention of a resource using the filesystem as a data source.
//!Resource files have following format and may contain many data units:
//!<int> = no. of data records in this RES file, Index area char[8] = data
//!record name <long> = pointer of corresponding data unit in this file
//!char[9] = the last index is a virtual index which is used to calculate
//!<long> the size of the last data unit, Data area <char..> = data
class Resource : public File {
public:
    int      rec_count;                           // total no. of records

private:
    enum {                                        // default buffer size : 5K
	DEF_BUF_SIZE = 5120
    };

    long     *index_buf;                          // index buffer pointer
    char     *data_buf;                           // data buffer pointer
    unsigned data_buf_size;                       // size of the data buffer

    char     init_flag;
    char     read_all;                            // read all data from resource file to memory
    char     use_common_buf;                      // use vga's buffer as data buffer or not
    int      cur_rec_no;                          // current record no

public:
    Resource()   { init_flag=0; }
    ~Resource()    { deinit(); }

    Resource(char* resFile, int readAll, int useCommonBuf=0)
	{ init_flag=0; init(resFile, readAll, useCommonBuf); }

    void  init(char* resFile, int readAll, int useCommonBuf=0);
    void  deinit();

    int   is_inited()   { return init_flag; }

    char* read(int= -1);
    File* get_file(int, int&);
};

//-------------------------------------------//
#endif
