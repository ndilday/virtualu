using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace virtualu
{
    enum ViewMode
    {
        Admission,
        Statistics,
        Priority,
        FinancialAid
    }

    class AdminOffice
    {
        public ViewMode current_mode;
        public ViewMode sub_current_mode;
        private static bool isReportInitialized = false;

        static ButtonGroup bottom_button_group(4);
        static short[] bottom_button_up_bitmap = new short[4];
        static short[] bottom_button_down_bitmap = new short[4];
        static string[] bottom_button_label = 
        {
            "Overview",
            "Trends",
            "Priorities",
            "Financial aid"
        };

        static ButtonGroup sub_bottom_button_group(5);
        static short[] sub_bottom_button_up_bitmap = new short[5];
        static short[] sub_bottom_button_down_bitmap = new short[5];
        static string[] sub_bottom_button_label = 
        {
            "Undergraduate : traditional",
            "Undergraduate : nontraditional",
            "Master's",
            "Doctoral",
            "Distance learning",
        };


        public AdminOffice()
        {
            init();
        }

        public ~AdminOffice()
        {
            deinit();
        }

        public void init()
        {
            for (byte i = 0; i < 4; i++) 
            {
	            bottom_button_up_bitmap[i] = 0;
	            bottom_button_down_bitmap[i] = 0;
            }
            current_mode = ViewMode.Admission;
            sub_current_mode = ViewMode.Admission;
        }

        public void deinit()
        {
            // are we going to need the deinit at all?
            // ##### begin Gilbert 28/04/2001 #####//
            bottom_button_group.deinit_buttons();
            // ##### end Gilbert 28/04/2001 #####//
            /*if (*bottom_button_up_bitmap) 
            {
	            for (int i = 0; i < 4; i++) 
                {
	                mem_del(bottom_button_up_bitmap[i]);
	                mem_del(bottom_button_down_bitmap[i]);
	                bottom_button_up_bitmap[i] = NULL;
	                bottom_button_down_bitmap[i] = NULL;
	            }
            }

            (*deinit_func_ptr[current_mode])();*/

            isReportInitialized = false;
        }

        public void report(int refreshFlag)
        {
            vga.use_back();

            // fix in version 2
            sys.is_institution_report = 1;

            if (refreshFlag == INFO_REPAINT) 
            {
                if (isReportInitialized)                        // deinit it first if it has already been initialized
                {
                    deinit();
                }

	            //------------- paint the background ----------------//
	            user_interface.bg_img(BG_PIC_ID, &vga_back);

	            //----- initialize the bottom sub-menus buttons -----//
	            if (!isReportInitialized) 
                {
	                short x = BOTTOM_BUTTON_X1;
	                for (int i = 0; i < 4; i++, x+=BOTTOM_BUTTON_WIDTH+BOTTOM_BUTTON_DISTANCE) 
                    {
		                user_interface.create_button_bitmap(x, BOTTOM_BUTTON_Y1,
						                    x+BOTTOM_BUTTON_WIDTH, BOTTOM_BUTTON_Y1+BOTTOM_BUTTON_HEIGHT,
						                    bottom_button_label[i], &bottom_button_up_bitmap[i],
						                    &bottom_button_down_bitmap[i], &vga_back);
		                bottom_button_group[i].create_bitmapW(x, BOTTOM_BUTTON_Y1,
						                      x+BOTTOM_BUTTON_WIDTH, BOTTOM_BUTTON_Y1+BOTTOM_BUTTON_HEIGHT,
						                      ((i == current_mode) ? bottom_button_down_bitmap[i] : bottom_button_up_bitmap[i])+4);
	                }
	                isReportInitialized = true;
	            }

	            //----------- paint the bottom button group ---------//
	            bottom_button_group.paint(current_mode);
            }

            //---- repaint/refresh the display of the current mode ----//
            (*report_func_ptr[current_mode])(refreshFlag);

            // Comments:	blt buffer only if repaint, because the
            //					report_x functions would handle blt_buf
            //					when refresh
            //					equivalently, in the report_x functions,
            //					only blt buffer when refresh
            // Norris 0714

            if (refreshFlag == INFO_REPAINT) 
            {
	            //------- blt the back buffer to front buffer -------//
	            vga.blt_buf(ZOOM_X1, ZOOM_Y1, ZOOM_X2, ZOOM_Y2);
            }

            //##trevor 2807
            vga.use_front();
            //##trevor 2807
        }

        public int detect()
        {
            vga.use_back();

            int rc = detect2();

            vga.use_front();

            return rc;
        }

        public int detect2()
        {
            if (!isReportInitialized)
            {
                return 0;
            }

            int bottomButtonGroupDetect;
            if ((bottomButtonGroupDetect = bottom_button_group.detect()) >= 0)
            {
                //---- deinit the modes to free memory ----//
                (*deinit_func_ptr[current_mode])();

                //-------- change display mode ------------//
                bottom_button_group[current_mode].body_ptr = (void*)(bottom_button_up_bitmap[current_mode] + 4);
                current_mode = bottomButtonGroupDetect;
                bottom_button_group[current_mode].body_ptr = (void*)(bottom_button_down_bitmap[current_mode] + 4);

                //-------- post display redraw flag -------//
                sys.redraw_all_flag = 1;

                return 1;
            }

            if ((*detect_func_ptr[current_mode])())
                return 1;

            return 0;
        }
    }
}
