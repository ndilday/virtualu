using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace virtualu.Utility
{
    //!
    public struct DbfHeader 
    {
	    public char dbf_id;
	    public char[] last_update = new char[3];
	    public long last_rec;
	    public ushort data_offset;
	    public ushort rec_size;
    }

    //!
    public struct DbfRec 
    {
	    char[] field_name = new char[11];
	    char field_type;
	    long[] dummy = new long[4];
	    object len_info;
	    char[] filler = new char[14];
    }

    class Database : File
    {

        DbfHeader dbf_header;
        string     rec_buf;
        long      cur_recno;
        long      last_read_recno;

        string     dbf_buf;                           // buffer for reading in the whole dbf
        char      dbf_buf_allocated;                  // whether we allocated the buffer or only take from extern pointer

        public static Database open_db(string dbName);

        public Database(string = null, int=0);
        public ~Database();

        public void  open(string, int=0);
        public void  open_from_buf(string);

        public string read(long=0);
        public void  go(long recNo)    { cur_recno=recNo; }
        public void  close();

        public int  rec_count()       { return dbf_header.last_rec; }
        public int  recno()           { return cur_recno; }
    }
}