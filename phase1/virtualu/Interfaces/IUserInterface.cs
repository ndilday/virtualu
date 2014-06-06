using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace virtualu.Interfaces
{
    interface IUserInterface
    {
        internal void bg_img(int p, IVgaBuffer vgaBuffer);

        void create_button_bitmap(int x, int BOTTOM_BUTTON_Y1, int p1, int p2, string p3, out short p4, out short p5, IVgaBuffer vgaBuffer);

        void brighten(int p1, int p2, int p3, int p4);

        void rect(int p1, int p2, int p3, int p4, int p5);
    }
}
