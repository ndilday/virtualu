using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace virtualu.Utility
{
    struct EmptyRoom
    {
        short recno;
        int deleted_game_date;
    };

    class DynArray 
    {
        public int  ele_num;                                 // the size of the whole array
        public int  block_num;                               // the number of element of each block
        public int  cur_pos;                                 // current position
        public int  last_ele;                                // last element position ( the array is not fully used )
        public int  ele_size;                                // the size of each element
        public int  sort_offset;
        public char sort_type;
        public string body_buf;                               // cur_pos and last_ele are start from 1 (not 0)

        public DynArray(int,int=DEF_DYNARRAY_BLOCK_SIZE);
        public ~DynArray();

        public void  deinit();
        public void  resize(int);

        public void  linkin(void*);
        public void  linkin_unique(void*);
        public void  linkout(int= -1);
        public void  update(void*, int= -1);
        public void  insert(void*);
        public void  insert_at(int,void*);
        public void  add_blank(int);

        public void  init_sort(int,char);
        public void  linkin_sort_scan_from_bottom(void*);

        public void* get();
        public void* get(int);
        public void* get_ptr();
        public void* get_ptr(int);
        public void  read(void*);

        public int   check_pos();

        public void  push(void*);
        public void  pop(void* =0);

        public void  start();
        public void  end();
        public int   fwd();
        public int   bkwd();

        public void  jump(int);
        public void  go(int);
        public int   recno();
        public int   size();

        public int   is_start();
        public int   is_end();

        public int   scan_whole(void*);
        public int   scan(void*,int,char,int=0);
        public int   compare(void*,int,char);

        public void  quick_sort( int(*cmpFun)(const void*, const void*) );
        public void  bubble_sort( int(*cmpFun)(const void*, const void*) );

        public void  clean_up(int* =0);
        public void  free_ptr(void*,int*);
        public void  zap(int resizeFlag=1);

        public int   write_file(File*);                      // Write current dynamic array to file
        public int   read_file(File*);                       // Read dynamic array from file
    }

    //!Another wrapper class for dynamic arrays.
    class DynArrayB : DynArray 
    {
        public EmptyRoom empty_room_array;
        public short empty_room_num;                     // rooms allocated
        public short empty_room_count;                   // rooms used
        public short reuse_interval_days;

        public DynArrayB(int,int=DEF_DYNARRAY_BLOCK_SIZE,int reuseIntervalDays=0);
        public ~DynArrayB();

        // packed_size()  is the size when the array is packed (deleted record are actually removed)
        // packed_recno() is the recno when the array is packed
        public int  packed_size()  { return size() - empty_room_count; }
        public int  packed_recno(int);                       // Given the recno unpacked, it returns the recno packed.
        public void linkin(void*);
        public void linkout(int= -1);
        public void zap();
        public int  write_file(File*);                       // Write current dynamic array to file
        public int  read_file(File*);                        // Read dynamic array from file
        public int  write_empty_room(File*);                 // Write current dynamic array to file
        public int  read_empty_room(File*);                  // Read dynamic array from file
        public int  write_ptr_array(File*, int);
        public int  read_ptr_array(File*, int, CreateEleFP);
    }
}
