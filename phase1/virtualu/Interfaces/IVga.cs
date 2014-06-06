using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace virtualu.Interfaces
{
    interface IVga
    {
        internal void use_back();

        IVgaBuffer GetBackBuffer();

        void blt_buf(short p1, short p2, short p3, short p4);

        void use_front();
    }

    interface IVgaBuffer
    {
    }
}
