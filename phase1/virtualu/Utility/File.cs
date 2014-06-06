using virtualu;

namespace virtualu.Utility
{
    /// <summary>
    /// Utility Class which abstracts file operations such as opening, closing, reading, and writing.
    /// </summary>
    class File
    {
        public string file_name;

        public object file_handle;
        public int    handle_error;

        public char   allow_vary_size;                       // allow the writing size and the read size to be different

        public File()     { file_handle=null; }
        public ~File();

        public int   file_open(string, int=1, int=0);
        public int   file_create(string, int=1, int=0);
        public int file_append(string, int=1, int=0);
        public void  file_close();

        public long  file_size();
        public long  file_seek(long, int= -1);
        public long  file_pos();

        public int   file_read(object, uint);
        public int   file_write(object, uint);

        public short file_get_short();
        public int   file_put_short(short);

        public ushort file_get_unsigned_short();
        public int   file_put_unsigned_short(ushort);

        public long  file_get_long();
        public int   file_put_long(long);
    }
}
