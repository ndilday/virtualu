using virtualu.Interfaces;
using virtualu.Widgets;

namespace virtualu.Buildings
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
#region Public Members
        public ViewMode current_mode;
        public ViewMode sub_current_mode;
#endregion

#region Private Members
        private IVga vga;
        private ISys sys;
        IUserInterface user_interface;
#endregion
        
#region Static Members
        static readonly short MODE_ADMISSION_BOX_HEIGHT = 265;
        static readonly short MODE_ADMISSION_BOX_WIDTH1 = 160;
        static readonly short MODE_ADMISSION_BOX_WIDTH2 = 400;
        static readonly short MODE_ADMISSION_BOX_WIDTH3 = 75;
        static readonly short MODE_ADMISSION_LEGEND_HEIGHT = 26;
        static readonly short MODE_ADMISSION_LEGEND_WIDTH = 400;

        static readonly int MODE_ADMISSION_BARCHART_X1 = REPORT_X1 + 10;
        static readonly int MODE_ADMISSION_BARCHART_Y1 = REPORT_Y1 + 55;
        static readonly short MODE_ADMISSION_BARCHART_HEIGHT = 210;
        static readonly short MODE_ADMISSION_BARCHART_WIDTH = 400;
        static readonly short MODE_ADMISSION_BARCHART_ROW1 = 42;
        static readonly short MODE_ADMISSION_BARCHART_ROW2 = 107;
        static readonly short MODE_ADMISSION_BARCHART_ROW3 = 172;
        static readonly short MODE_ADMISSION_BARCHART_ROW4 = 235;
        static readonly short MODE_ADMISSION_BARCHART_ROW5 = 270;
        static readonly short MODE_ADMISSION_SLIDER_WIDTH = 340;

        static readonly short MODE_STATISTICS_GRAPH_WIDTH = 220;
        static readonly short MODE_STATISTICS_GRAPH_HEIGHT = 115;
        static readonly short MODE_STATISTICS_LEGEND_HEIGHT = 26;
        static readonly short MODE_STATISTICS_LEGEND_WIDTH = 400;
        static readonly int MODE_STATISTICS_GRAPH_HORIZONTAL_DISTANCE = 
            (REPORT_X2 - REPORT_X1 - MODE_STATISTICS_GRAPH_WIDTH * 3) / 2;
        static readonly short MODE_STATISTICS_GRAPH_VERTICAL_DISTANCE = 20;
        static readonly short MODE_STATISTICS_SUB_BUTTON_X1 = 130;
        static readonly int MODE_STATISTICS_SUB_BUTTON_Y1 = REPORT_Y2 - 48;
        
        static readonly short SUB_BOTTOM_BUTTON_WIDTH = 200;
        static readonly short SUB_BOTTOM_BUTTON_HEIGHT = 23;
        static readonly short SUB_BOTTOM_BUTTON_DISTANCE = 13;

        static readonly short MODE_PRIORITY_BOX_HEIGHT = 165;
        static readonly short MODE_PRIORITY_BOX_WIDTH = 650;
        static readonly short MODE_PRIORITY_BOX_TAB1 = 40;
        static readonly short MODE_PRIORITY_BOX_TAB2 = 240;
        static readonly short MODE_PRIORITY_BOX_TAB3 = 360;
        static readonly short MODE_PRIORITY_VALUE_SLIDER_HEIGHT = 18;
        static readonly short MODE_PRIORITY_VALUE_SLIDER_WIDTH = 400;
        static readonly short MODE_PRIORITY_RADIO_BUTTON_HEIGHT = 18;
        static readonly short MODE_PRIORITY_RADIO_BUTTON_WIDTH = 350;

        static readonly short MODE_FINANCIAL_AID_BOX_HEIGHT_1 = 100;
        static readonly short MODE_FINANCIAL_AID_BOX_HEIGHT_2 = 80;
        static readonly short MODE_FINANCIAL_AID_BOX_HEIGHT_3 = 120;
        static readonly short MODE_FINANCIAL_AID_BOX_DISTANCE = 0;
        static readonly short MODE_FINANCIAL_AID_VALUE_SLIDER_HEIGHT = 18;
        static readonly short MODE_FINANCIAL_AID_VALUE_SLIDER_WIDTH = 100;
        static readonly short MODE_FINANCIAL_AID_RADIO_BUTTON_HEIGHT = 18;
        static readonly short MODE_FINANCIAL_AID_RADIO_BUTTON_WIDTH = 80;

        static readonly int REPORT_X1 = Matrix.ZOOM_X1 + 15;
        static readonly int REPORT_Y1 = Matrix.ZOOM_Y1 + 25;
        static readonly int REPORT_X2 = Matrix.ZOOM_X2 - 15;
        static readonly int REPORT_Y2 = Matrix.ZOOM_Y2 - 55;

        static readonly int BOTTOM_BUTTON_X1 = REPORT_X1 + 80;
        static readonly int BOTTOM_BUTTON_Y1 = REPORT_Y2 + 15;
        static readonly int BOTTOM_BUTTON_X2 = REPORT_X2 - 80;
        static readonly int BOTTOM_BUTTON_Y2 = REPORT_Y2 + 40;

        static readonly short BOTTOM_BUTTON_WIDTH = 125;
        static readonly short BOTTOM_BUTTON_HEIGHT = 25;
        static readonly int BOTTOM_BUTTON_DISTANCE = (BOTTOM_BUTTON_X2 - BOTTOM_BUTTON_X1 - BOTTOM_BUTTON_WIDTH * 4) / 3;

        static bool isReportInitialized = false;

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

        static GraphNonFinancialYear mode_statistics_graph_admit_rate;
        static GraphNonFinancialYear mode_statistics_graph_yield_rate;
        static GraphNonFinancialYear mode_statistics_graph_percent_minorities;
        static GraphNonFinancialYear mode_statistics_graph_avg_academic_rating;
        static GraphNonFinancialYear mode_statistics_graph_avg_extracurricular_activities_rating;
        static GraphNonFinancialYear mode_statistics_graph_avg_athletics_rating;
        static GraphNonFinancialTrimester distance_credits_graph;
        static GraphNonFinancialYear[] mode_statistics_graph = {
            mode_statistics_graph_admit_rate,
            mode_statistics_graph_yield_rate,
            mode_statistics_graph_avg_academic_rating,
            mode_statistics_graph_percent_minorities,
            mode_statistics_graph_avg_extracurricular_activities_rating,
            mode_statistics_graph_avg_athletics_rating
        };

        static string[] mode_statistics_graph_title = {
            "Distance credits demanded", "Distance credits provided",
            "Average academic rating", "Percent minorities",
            "Average extracurricular activities rating", "Average athletics rating"
        };

        static string[] mode_statistics_legend_label = {
            "Player university", "Average of peer universities"
        };

        static RadioButton mode_financial_aid_radio_button_nontraditional;
        static RadioButton mode_financial_aid_radio_button_minorities;
        static RadioButton mode_financial_aid_radio_button_athletes;
        static ValueSlider mode_financial_aid_value_slider_nontraditional;
        static ValueSlider mode_financial_aid_value_slider_need;
        static ValueSlider mode_financial_aid_value_slider_merit;
        static string[] mode_financial_aid_str = 
        {
            "Percentage of traditional undergraduates' financial need the University agrees to pay",
            "Traditional undergraduates to be offered merit aid",
            "Nontraditional students to be offered financial aid",
            "Do you give special financial aid treatment to minorities for all student levels?",
            "Do you give special financial aid treatment to athletes for all traditional undergraduates?"
        };

        static string[] mode_financial_aid_radio_button_option_str1 = 
        {
            "Yes", 
            "No"
        };
        static string[] mode_financial_aid_radio_button_option_str2 = {
            "None", 
            "Medium", 
            "High",
        };

        static string[] mode_admission_label1 = 
        {
            "Admit",
            "Yield",
            "rate"
        };

        static string[] mode_admission_label2 = 
        {
            "Traditional undergraduate",
            "Nontraditional undergraduate",
            "Master's",
            "Doctoral",
            "Distance learning"
        };

        static string[] mode_admission_label3 = 
        {
            "Matriculants",
            "Admits (not matriculating)",
            "Applicants (not admitted)"
        };

        static short[] mode_admission_bar_color = 
        {
            Color.RED,
            Color.GREEN,
            Color.BLUE
        };

        static short* mode_admission_no_scaling_bitmap;
        static short[] mode_admission_no_ar_bitmap = new short[5];
        static short[] mode_admission_no_yr_bitmap = new short[5];
        static short[] mode_admission_no_bar_bitmap = new short[5];

        static ValueSlider mode_admission_target[5];

        // begin kevin 0723
        static RadioButton mode_priority_radio_button_minorities;
        static RadioButton mode_priority_radio_button_athletes;
        static RadioButton mode_priority_radio_button_distance_learners;
        static ValueSlider mode_priority_value_slider_academic_rating;
        static ValueSlider mode_priority_value_slider_extracur_rating;
        static ValueSlider mode_priority_value_slider_athletic_rating;

        static string[] mode_priority_str = {
            "Traditional undergraduates",
            "Academic performance",
            "Extracurricular performance",
            "Athletic performance",
            "Distance learning",
            "Do you give special admission to underrepresented minorities for all student levels?",
            "Do you give special admissions treatment to athletes for all traditional undergraduates?",
            "Rating priority for traditional undergraduates",
            "Acceptance of outside distance learners in courses",
        };

        static string[] mode_priority_radio_button_option_str = 
        {
            "None", 
            "Medium", 
            "High"
        };

        static string[] mode_priority_radio_button_option_str1 = 
        {
            "Prohibit", 
            "Accept", 
            "Actively seek",
        };
#endregion

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
            bottom_button_group.deinit_buttons();
            isReportInitialized = false;
        }

        public void report(RefreshType refreshFlag)
        {
            vga.use_back();

            // fix in version 2
            sys.is_institution_report = 1;

            if (refreshFlag == RefreshType.Repaint) 
            {
                if (isReportInitialized)                        // deinit it first if it has already been initialized
                {
                    deinit();
                }

	            //------------- paint the background ----------------//
	            user_interface.bg_img(Constants.BG_PIC_ID, vga.GetBackBuffer());

	            //----- initialize the bottom sub-menus buttons -----//
	            if (!isReportInitialized) 
                {
	                int x = BOTTOM_BUTTON_X1;
	                for (int i = 0; i < 4; i++) 
                    {
                        x += BOTTOM_BUTTON_WIDTH + BOTTOM_BUTTON_DISTANCE;
		                user_interface.create_button_bitmap(x, BOTTOM_BUTTON_Y1,
						                    x+BOTTOM_BUTTON_WIDTH, BOTTOM_BUTTON_Y1+BOTTOM_BUTTON_HEIGHT,
						                    bottom_button_label[i], out bottom_button_up_bitmap[i],
						                    out bottom_button_down_bitmap[i], vga.GetBackBuffer());
		                bottom_button_group.button_array[i].create_bitmapW(x, BOTTOM_BUTTON_Y1,
						                      x+BOTTOM_BUTTON_WIDTH, BOTTOM_BUTTON_Y1+BOTTOM_BUTTON_HEIGHT,
						                      ((i == (int)current_mode) ? bottom_button_down_bitmap[i] : bottom_button_up_bitmap[i])+4);
	                }
	                isReportInitialized = true;
	            }

	            //----------- paint the bottom button group ---------//
	            bottom_button_group.paint(current_mode);
            }

            //---- repaint/refresh the display of the current mode ----//
            (*report_func_ptr[current_mode])(refreshFlag);
            switch(current_mode)
            {
                case ViewMode.Admission:
                    report_admission(refreshFlag);
                    break;
                case ViewMode.FinancialAid:
                    break;
                case ViewMode.Priority:
                    break;
                case ViewMode.Statistics:
                    break;
            }

            // blt buffer only if repaint, because the report_x functions would handle blt_buf
            // when refresh equivalently, in the report_x functions, only blt buffer when refresh

            if (refreshFlag == RefreshType.Repaint) 
            {
	            //------- blt the back buffer to front buffer -------//
	            vga.blt_buf(Matrix.ZOOM_X1, Matrix.ZOOM_Y1, Matrix.ZOOM_X2, Matrix.ZOOM_Y2);
            }

            vga.use_front();
        }

        public bool detect()
        {
            vga.use_back();

            bool rc = detect2();

            vga.use_front();

            return rc;
        }

        public bool detect2()
        {
            if (!isReportInitialized)
            {
                return false;
            }

            ViewMode bottomButtonGroupDetect;
            if ((bottomButtonGroupDetect = bottom_button_group.detect()) >= 0)
            {
                //---- deinit the modes to free memory ----//
                (*deinit_func_ptr[current_mode])();

                //-------- change display mode ------------//
                bottom_button_group[current_mode].body_ptr = (void*)(bottom_button_up_bitmap[current_mode] + 4);
                current_mode = bottomButtonGroupDetect;
                bottom_button_group[current_mode].body_ptr = (void*)(bottom_button_down_bitmap[current_mode] + 4);

                //-------- post display redraw flag -------//
                sys.redraw_all_flag = true;

                return true;
            }

            if ((*detect_func_ptr[current_mode])())
                return true;

            return false;
        }
    
        void report_admission(RefreshType refreshFlag) 
        {
            // begin Kevin 0716

            //temp * start
            static int ar_tug=10,yr_tug=24;
            static int ar_ug=20,yr_ug=42;
            static int ar_master=15,yr_master=23;
            static int ar_doc=12,yr_doc=23;
            static int ar_dist=12,yr_dist=23;
            static int bar[5][3];

            // ##### Begin Marco ##### //
            static int initial_ar_tug=10,initial_yr_tug=24;
            static int initial_ar_ug=20,initial_yr_ug=42;
            static int initial_ar_master=15,initial_yr_master=23;
            static int initial_ar_doc=12,initial_yr_doc=23;
            static int initial_ar_dist=12,initial_yr_dist=23;
            // ##### End Marco ###### //

            // top title
            user_interface.brighten(REPORT_X1+150,REPORT_Y1-18,REPORT_X1+575,REPORT_Y1+5);
            user_interface.rect(REPORT_X1+150,REPORT_Y1-18,REPORT_X1+575,REPORT_Y1+5,1);
            Fonts.font_charts.put(REPORT_X1+220,REPORT_Y1-12,"Student Admissions Statistics and Targets");

            //	Communicate with Sim Engine's Output
            ar_tug=(int)(100*enroll_res.offers_rate[0]);
            yr_tug=(int)(100*enroll_res.cur_yield_rate[0]);
            ar_ug=(int)(100*enroll_res.offers_rate[1]);
            yr_ug=(int)(100*enroll_res.cur_yield_rate[1]);
            ar_master=(int)(100*enroll_res.offers_rate[2]);
            yr_master=(int)(100*enroll_res.cur_yield_rate[2]);
            ar_doc=(int)(100*enroll_res.offers_rate[3]);
            yr_doc=(int)(100*enroll_res.cur_yield_rate[3]);
            ar_dist=(int)(100*enroll_res.offers_rate[4]);
            yr_dist=(int)(100*enroll_res.cur_yield_rate[4]);

            // ##### Begin Marco ##### //
            initial_ar_tug=(int)(100*enroll_res.initial_offers_rate[0]);
            initial_yr_tug=(int)(100*enroll_res.initial_cur_yield_rate[0]);
            initial_ar_ug=(int)(100*enroll_res.initial_offers_rate[1]);
            initial_yr_ug=(int)(100*enroll_res.initial_cur_yield_rate[1]);
            initial_ar_master=(int)(100*enroll_res.initial_offers_rate[2]);
            initial_yr_master=(int)(100*enroll_res.initial_cur_yield_rate[2]);
            initial_ar_doc=(int)(100*enroll_res.initial_offers_rate[3]);
            initial_yr_doc=(int)(100*enroll_res.initial_cur_yield_rate[3]);
            initial_ar_dist=(int)(100*enroll_res.initial_offers_rate[4]);
            initial_yr_dist=(int)(100*enroll_res.initial_cur_yield_rate[4]);
            // ##### End Marco ##### //

            bar[0][0]=enroll_res.total_matrics[UG_TRADITION];
            bar[1][0]=enroll_res.total_matrics[UG_NONTRADITION];
            bar[2][0]=enroll_res.total_matrics[MASTER];
            bar[3][0]=enroll_res.total_matrics[DOCTOR];
            bar[4][0]=enroll_res.total_matrics[DISTANCE_LEARN];

            bar[0][1]=enroll_res.total_offers[UG_TRADITION]-bar[0][0];
            bar[1][1]=enroll_res.total_offers[UG_NONTRADITION]-bar[1][0];
            bar[2][1]=enroll_res.total_offers[MASTER]-bar[2][0];
            bar[3][1]=enroll_res.total_offers[DOCTOR]-bar[3][0];
            bar[4][1]=enroll_res.total_offers[DISTANCE_LEARN]-bar[4][0];

            bar[0][2]=enroll_res.total_applications[UG_TRADITION]-bar[0][1]-bar[0][0];
            bar[1][2]=enroll_res.total_applications[UG_NONTRADITION]-bar[1][1]-bar[1][0];
            bar[2][2]=enroll_res.total_applications[MASTER]-bar[2][1]-bar[2][0];
            bar[3][2]=enroll_res.total_applications[DOCTOR]-bar[3][1]-bar[3][0];
            bar[4][2]=enroll_res.total_applications[DISTANCE_LEARN]-bar[4][1]-bar[4][0];

            //## chea begin 020999 make the target_student_intake the same 4 every year
            sliderVar1=enroll_res.target_enrollment[0];
            sliderVar2=enroll_res.target_enrollment[1];
            sliderVar3=enroll_res.target_enrollment[2];

            /*
              finance.tradU_target_student_intake = enroll_res.target_student_intake[0];
              finance.nontradU_target_student_intake = enroll_res.target_student_intake[1];
              finance.master_target_student_intake = =enroll_res.target_student_intake[2];
              finance.dl_target_student_intake = enroll_res.target_student_intake[4];
            */
            //## chea begin 020999 make the target_student_intake the same 4 every year

            if (refreshFlag == INFO_REPAINT) {
	        //--------- draw the brightened boxes ------------//
	        user_interface.brighten(REPORT_X1, REPORT_Y1+30,
				        REPORT_X2,
				        REPORT_Y1+81+MODE_ADMISSION_BOX_HEIGHT);
	        user_interface.rect(REPORT_X1, REPORT_Y1+25,
			            REPORT_X2,
			            REPORT_Y1+55+MODE_ADMISSION_BOX_HEIGHT);
	        user_interface.rect(REPORT_X1+MODE_ADMISSION_BOX_WIDTH1,
			            REPORT_Y1+25,
			            REPORT_X1+MODE_ADMISSION_BOX_WIDTH1+MODE_ADMISSION_BOX_WIDTH2,
			            REPORT_Y1+55+MODE_ADMISSION_BOX_HEIGHT);
	        user_interface.rect(REPORT_X1+MODE_ADMISSION_BOX_WIDTH1+MODE_ADMISSION_BOX_WIDTH2-3,
			            REPORT_Y1+25,
			            REPORT_X1+MODE_ADMISSION_BOX_WIDTH1+MODE_ADMISSION_BOX_WIDTH2+MODE_ADMISSION_BOX_WIDTH3,
			            REPORT_Y1+55+MODE_ADMISSION_BOX_HEIGHT);
	        user_interface.rect(
	            REPORT_X1,
	            REPORT_Y1+25,
	            REPORT_X2,
	            REPORT_Y1+81+MODE_ADMISSION_BOX_HEIGHT);

	        if (mode_init_flag != MODE_ADMISSION) {
	            //--------- initialization of   -----------//
	            mode_init_flag = MODE_ADMISSION;

	            if(mode_admission_no_scaling_bitmap) {
		        mem_del(mode_admission_no_scaling_bitmap);
		        mode_admission_no_scaling_bitmap=NULL;
	            }

	            for (int i = 0; i < 5; i++) {
		        if(mode_admission_no_ar_bitmap[i]) {
		            mem_del(mode_admission_no_ar_bitmap[i]);
		            mode_admission_no_ar_bitmap[i]=NULL;
		        }

		        if(mode_admission_no_yr_bitmap[i]) {
		            mem_del(mode_admission_no_yr_bitmap[i]);
		            mode_admission_no_yr_bitmap[i]=NULL;
		        }

		        if(mode_admission_no_bar_bitmap[i]) {
		            mem_del(mode_admission_no_bar_bitmap[i]);
		            mode_admission_no_bar_bitmap[i]=NULL;
		        }
	            }

	            mode_admission_no_ar_bitmap[0]
		        =vga_back.save_area((REPORT_X2)-150,
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1,
				            (REPORT_X2)-75,
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1+font_chartsm.max_font_height,
				            mode_admission_no_ar_bitmap[0]);
	            mode_admission_no_ar_bitmap[1]
		        =vga_back.save_area((REPORT_X2)-150,
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2,
				            (REPORT_X2)-75,
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2+font_chartsm.max_font_height,
				            mode_admission_no_ar_bitmap[1]);
	            mode_admission_no_ar_bitmap[2]
		        =vga_back.save_area((REPORT_X2)-150,
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3,
				            (REPORT_X2)-75,
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3+font_chartsm.max_font_height,
				            mode_admission_no_ar_bitmap[2]);
	            mode_admission_no_ar_bitmap[3]
		        =vga_back.save_area((REPORT_X2)-150,
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4,
				            (REPORT_X2)-75,
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4+font_chartsm.max_font_height,
				            mode_admission_no_ar_bitmap[3]);
	            mode_admission_no_ar_bitmap[4]
		        =vga_back.save_area((REPORT_X2)-150,
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW5,
				            (REPORT_X2)-75,
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW5+font_chartsm.max_font_height,
				            mode_admission_no_ar_bitmap[4]);

	            mode_admission_no_yr_bitmap[0]
		        =vga_back.save_area((REPORT_X2)-75,
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1,
				            (REPORT_X2),
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1+font_chartsm.max_font_height,
				            mode_admission_no_yr_bitmap[0]);
	            mode_admission_no_yr_bitmap[1]
		        =vga_back.save_area((REPORT_X2)-75,
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2,
				            (REPORT_X2),
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2+font_chartsm.max_font_height,
				            mode_admission_no_yr_bitmap[1]);
	            mode_admission_no_yr_bitmap[2]
		        =vga_back.save_area((REPORT_X2)-75,
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3,
				            (REPORT_X2),
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3+font_chartsm.max_font_height,
				            mode_admission_no_yr_bitmap[2]);
	            mode_admission_no_yr_bitmap[3]
		        =vga_back.save_area((REPORT_X2)-75,
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4,
				            (REPORT_X2),
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4+font_chartsm.max_font_height,
				            mode_admission_no_yr_bitmap[3]);
	            mode_admission_no_yr_bitmap[4]
		        =vga_back.save_area((REPORT_X2)-75,
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW5,
				            (REPORT_X2),
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW5+font_chartsm.max_font_height,
				            mode_admission_no_yr_bitmap[4]);

	            mode_admission_no_bar_bitmap[0]
		        =vga_back.save_area(
		            (REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1,
		            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1,
		            (REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1+MODE_ADMISSION_BOX_WIDTH2,
		            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1+14,
		            mode_admission_no_bar_bitmap[0]);
	            mode_admission_no_bar_bitmap[1]
		        =vga_back.save_area(
		            (REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1,
		            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2,
		            (REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1+MODE_ADMISSION_BOX_WIDTH2,
		            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2+14,
		            mode_admission_no_bar_bitmap[1]);
	            mode_admission_no_bar_bitmap[2]
		        =vga_back.save_area(
		            (REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1,
		            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3,
		            (REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1+MODE_ADMISSION_BOX_WIDTH2,
		            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3+14,
		            mode_admission_no_bar_bitmap[2]);
	            mode_admission_no_bar_bitmap[3]
		        =vga_back.save_area(
		            (REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1,
		            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4,
		            (REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1+MODE_ADMISSION_BOX_WIDTH2,
		            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4+14,
		            mode_admission_no_bar_bitmap[3]);
	            mode_admission_no_bar_bitmap[4]
		        =vga_back.save_area(
		            (REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1,
		            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW5,
		            (REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1+MODE_ADMISSION_BOX_WIDTH2,
		            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW5+14,
		            mode_admission_no_bar_bitmap[4]);

	            mode_admission_target[0].init(
		        REPORT_X1+MODE_ADMISSION_BOX_WIDTH1+20,
		        REPORT_Y1+MODE_ADMISSION_BARCHART_ROW1+19,
		        MODE_ADMISSION_SLIDER_WIDTH,
		        MODE_FINANCIAL_AID_VALUE_SLIDER_HEIGHT-1,
		        0.8*enroll_res.new_year_student_count[0], 1.2*enroll_res.new_year_student_count[0], &sliderVar1, 1, V_BROWN, 1);

	            mode_admission_target[1].init(
		        REPORT_X1+MODE_ADMISSION_BOX_WIDTH1+20,
		        REPORT_Y1+MODE_ADMISSION_BARCHART_ROW2+19,
		        MODE_ADMISSION_SLIDER_WIDTH,
		        MODE_FINANCIAL_AID_VALUE_SLIDER_HEIGHT-1,
		        0.8*enroll_res.new_year_student_count[1], 1.2*enroll_res.new_year_student_count[1], &sliderVar2, 1, V_BROWN, 1);

	            mode_admission_target[2].init(
		        REPORT_X1+MODE_ADMISSION_BOX_WIDTH1+20,
		        REPORT_Y1+MODE_ADMISSION_BARCHART_ROW3+19,
		        MODE_ADMISSION_SLIDER_WIDTH,
		        MODE_FINANCIAL_AID_VALUE_SLIDER_HEIGHT-1,
		        0.8*enroll_res.new_year_student_count[2], 1.2*enroll_res.new_year_student_count[2], &sliderVar3, 1, V_BROWN, 1);

	            mode_admission_target[0].refresh();
	            mode_admission_target[1].refresh();
	            mode_admission_target[2].refresh();

	        }
	        else {
	            //-------- repaint -----------//
	        }

	        // -------- display the strings ----------- //
	        font_chartsm.put(
	            (REPORT_X2)-425,
	            (REPORT_Y1)+11,
	            "Number of students");
	        font_chartsm.put(
	            (REPORT_X2)-125,
	            (REPORT_Y1)-1,
	            mode_admission_label1[0]);
	        font_chartsm.put(
	            (REPORT_X2)-50,
	            (REPORT_Y1)-1,
	            mode_admission_label1[1]);
	        font_chartsm.put(
	            (REPORT_X2)-120,
	            (REPORT_Y1)+11,
	            mode_admission_label1[2]);
	        font_chartsm.put(
	            (REPORT_X2)-50,
	            (REPORT_Y1)+11,
	            mode_admission_label1[2]);

	        font_chartsm.put_paragraph(
	            (REPORT_X1)+80,
	            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1-10,
	            (REPORT_X1)+180,
	            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1+30,
	            mode_admission_label2[0],-1);

	        font_chartsm.put_paragraph(
	            (REPORT_X1)+80,
	            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2-15,
	            (REPORT_X1)+180,
	            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2+25,
	            mode_admission_label2[1],-1);
	        font_chartsm.put(
	            (REPORT_X1)+110,
	            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3,
	            mode_admission_label2[2]);
	        font_chartsm.put(
	            (REPORT_X1)+107,
	            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4,
	            mode_admission_label2[3]);

	        font_chart_red_sm.put(
	            (REPORT_X1)+60,
	            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1+20,
	            "Target enrollment");
	        font_chart_red_sm.put(
	            (REPORT_X1)+60,
	            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2+20,
	            "Target enrollment");
	        font_chart_red_sm.put(
	            (REPORT_X1)+60,
	            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3+20,
	            "Target enrollment");

	        font_chartsm.put(
	            145,
	            REPORT_Y1+78+MODE_ADMISSION_BOX_HEIGHT-font_chartsm.max_font_height,
	            mode_admission_label3[0]);
	        font_chartsm.put(
	            43+(REPORT_X2-REPORT_X1)/3+4,
	            REPORT_Y1+78+MODE_ADMISSION_BOX_HEIGHT-font_chartsm.max_font_height,
	            mode_admission_label3[1]);
	        font_chartsm.put(
	            43+2*(REPORT_X2-REPORT_X1)/3+4,
	            REPORT_Y1+78+MODE_ADMISSION_BOX_HEIGHT-font_chartsm.max_font_height,
	            mode_admission_label3[2]);

	        user_interface.bar(
	            127,
	            REPORT_Y1+77+MODE_ADMISSION_BOX_HEIGHT-font_chartsm.max_font_height,
	            139,
	            REPORT_Y1+89+MODE_ADMISSION_BOX_HEIGHT-font_chartsm.max_font_height,
	            mode_admission_bar_color[0]);

	        user_interface.bar(
	            25+(REPORT_X2-REPORT_X1)/3+4,
	            REPORT_Y1+77+MODE_ADMISSION_BOX_HEIGHT-font_chartsm.max_font_height,
	            25+(REPORT_X2-REPORT_X1)/3+16,
	            REPORT_Y1+89+MODE_ADMISSION_BOX_HEIGHT-font_chartsm.max_font_height,
	            mode_admission_bar_color[1]);

	        user_interface.bar(
	            25+2*(REPORT_X2-REPORT_X1)/3+4,
	            REPORT_Y1+77+MODE_ADMISSION_BOX_HEIGHT-font_chartsm.max_font_height,
	            25+2*(REPORT_X2-REPORT_X1)/3+16,
	            REPORT_Y1+89+MODE_ADMISSION_BOX_HEIGHT-font_chartsm.max_font_height,
	            mode_admission_bar_color[2]);
            }
            else {                                          // refreshFlag == INFO_REFRESH
	        //		sliderVar1=enroll_res.total_matrics[UG_TRADITION];
	        //		sliderVar2=enroll_res.total_matrics[UG_NONTRADITION];
	        //		sliderVar3=enroll_res.total_matrics[MASTER];
	        //		sliderVar5=enroll_res.total_matrics[DISTANCE_LEARN];

	        if(sliderVar1>mode_admission_target[0].max_val)
	            sliderVar1=mode_admission_target[0].max_val;
	        if(sliderVar2>mode_admission_target[1].max_val)
	            sliderVar2=mode_admission_target[1].max_val;
	        if(sliderVar3>mode_admission_target[2].max_val)
	            sliderVar3=mode_admission_target[2].max_val;

	        int row_tab=0;
	        for(int i=0;i<4;i++)
	            {
		        if(i==0)
		            row_tab = MODE_ADMISSION_BARCHART_ROW1;
		        else if (i==1)
		            row_tab = MODE_ADMISSION_BARCHART_ROW2;
		        else if (i==2)
		            row_tab = MODE_ADMISSION_BARCHART_ROW3;

		        if(i!=3) {

		            //## chea 040999 set a ribbon to last year target
		            int update_last_year_tar = enroll_res.last_year_target_student_intake[i];
		            if(update_last_year_tar >= mode_admission_target[i].max_val)
			        update_last_year_tar = mode_admission_target[i].max_val;
		            if(update_last_year_tar <= mode_admission_target[i].min_val)
			        update_last_year_tar = 0;

		            int update_target_pos  = (short)((((float)mode_admission_target[i].value_slider_x2 - (float)mode_admission_target[i].value_slider_x1) / (float)mode_admission_target[i].max_val)*update_last_year_tar);

		            mode_admission_target[i].set_target(
			        mode_admission_target[i].value_slider_x1 + update_target_pos -1,
			        REPORT_Y1+row_tab+16,
			        MODE_FINANCIAL_AID_VALUE_SLIDER_HEIGHT +8,
			        1, V_RED,V_RED);

		            /*
		              //## 080999 set target to this year

		              int update_year_tar = enroll_res.target_student_intake[i];
		              int update_target_pos  = (short)((((float)mode_admission_target[i].value_slider_x2 - (float)mode_admission_target[i].value_slider_x1) / (float)mode_admission_target[i].max_val)*update_year_tar);
		              mode_admission_target[i].init_target(
		              mode_admission_target[i].value_slider_x1 + update_target_pos -1,
		              REPORT_Y1+row_tab+16,
		              MODE_FINANCIAL_AID_VALUE_SLIDER_HEIGHT+8,
		              1, V_RED,V_RED);
		            */

		            vga_back.rest_area(mode_admission_no_ar_bitmap[i], 0, 0);
		            vga_back.rest_area(mode_admission_no_yr_bitmap[i], 0, 0);
		            vga_back.rest_area(mode_admission_no_bar_bitmap[i], 0, 0);

		            mode_admission_target[i].refresh();

		        }
	            }
            }

            // ---- Draw the statistics ---- //
            font_chartsm.center_put((REPORT_X2)-150,
			            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1,
			            (REPORT_X2)-75,
			            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1+font_chartsm.max_font_height,
			            m.format(ar_tug, 3));
            font_chartsm.center_put((REPORT_X2)-75,
			            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1,
			            (REPORT_X2),
			            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1+font_chartsm.max_font_height,
			            m.format(yr_tug, 3));

            font_chartsm.center_put((REPORT_X2)-150,
			            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2,
			            (REPORT_X2)-75,
			            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2+font_chartsm.max_font_height,
			            m.format(ar_ug, 3));
            font_chartsm.center_put((REPORT_X2)-75,
			            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2,
			            (REPORT_X2),
			            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2+font_chartsm.max_font_height,
			            m.format(yr_ug, 3));

            font_chartsm.center_put((REPORT_X2)-150,
			            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3,
			            (REPORT_X2)-75,
			            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3+font_chartsm.max_font_height,
			            m.format(ar_master, 3));
            font_chartsm.center_put((REPORT_X2)-75,
			            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3,
			            (REPORT_X2),
			            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3+font_chartsm.max_font_height,
			            m.format(yr_master, 3));

            font_chartsm.center_put((REPORT_X2)-150,
			            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4,
			            (REPORT_X2)-75,
			            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4+font_chartsm.max_font_height,
			            m.format(ar_doc, 3));
            font_chartsm.center_put((REPORT_X2)-75,
			            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4,
			            (REPORT_X2),
			            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4+font_chartsm.max_font_height,
			            m.format(yr_doc, 3));

            // ##### Begin Marco ##### //
            font_chart_purple_sm.center_put((REPORT_X2)-150,
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1+font_chart_purple_sm.max_font_height+3,
				            (REPORT_X2)-75,
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1+font_chart_purple_sm.max_font_height*2,
				            m.format(initial_ar_tug, 3));
            font_chart_purple_sm.center_put((REPORT_X2)-75,
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1+font_chart_purple_sm.max_font_height+3,
				            (REPORT_X2),
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1+font_chart_purple_sm.max_font_height*2,
				            m.format(initial_yr_tug, 3));

            font_chart_purple_sm.center_put((REPORT_X2)-150,
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2+font_chart_purple_sm.max_font_height+3,
				            (REPORT_X2)-75,
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2+font_chart_purple_sm.max_font_height*2,
				            m.format(initial_ar_ug, 3));
            font_chart_purple_sm.center_put((REPORT_X2)-75,
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2+font_chart_purple_sm.max_font_height+3,
				            (REPORT_X2),
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2+font_chart_purple_sm.max_font_height*2,
				            m.format(initial_yr_ug, 3));

            font_chart_purple_sm.center_put((REPORT_X2)-150,
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3+font_chart_purple_sm.max_font_height+3,
				            (REPORT_X2)-75,
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3+font_chart_purple_sm.max_font_height*2,
				            m.format(initial_ar_master, 3));
            font_chart_purple_sm.center_put((REPORT_X2)-75,
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3+font_chart_purple_sm.max_font_height+3,
				            (REPORT_X2),
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3+font_chart_purple_sm.max_font_height*2,
				            m.format(initial_yr_master, 3));

            font_chart_purple_sm.center_put((REPORT_X2)-150,
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4+font_chart_purple_sm.max_font_height+3,
				            (REPORT_X2)-75,
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4+font_chart_purple_sm.max_font_height*2,
				            m.format(initial_ar_doc, 3));
            font_chart_purple_sm.center_put((REPORT_X2)-75,
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4+font_chart_purple_sm.max_font_height+3,
				            (REPORT_X2),
				            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4+font_chart_purple_sm.max_font_height*2,
				            m.format(initial_yr_doc, 3));

            /*		Kenneth --- Remove initial value
		        font_chart_purple_sm.center_put((REPORT_X2)-150,
		        (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW5+font_chart_purple_sm.max_font_height+3,
		        (REPORT_X2)-75,
		        (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW5+font_chart_purple_sm.max_font_height*2,
		        m.format(initial_ar_dist, 3));
		        font_chart_purple_sm.center_put((REPORT_X2)-75,
		        (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW5+font_chart_purple_sm.max_font_height+3,
		        (REPORT_X2),
		        (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW5+font_chart_purple_sm.max_font_height*2,
		        m.format(initial_yr_dist, 3));
            */
            // ##### End Marco ##### //

            // ---- Draw bars ---- //
            int series_x1 = (REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1+10;
            int series_x2 = (REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1+MODE_ADMISSION_BOX_WIDTH2-10;
            int total,lastpos,newpos,barlen;
            int i;

            // BAR 1 //
            for(i=0,total=0;i<3;i++) {
	        total+=bar[0][i];
            }

            //##chwg0525
            if(total==0)total=1;

            for(i=0,lastpos=series_x1;i<3;i++) {
	        barlen=bar[0][i]*(series_x2-series_x1)/total;
	        if(barlen<=0)barlen=0;
	        newpos=lastpos+barlen;
	        user_interface.bar(
	            lastpos,
	            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1,
	            newpos,
	            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1+13,
	            mode_admission_bar_color[i]);
	        if(barlen>20)
	            font_barcharts.center_put(
		        lastpos,
		        (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1,
		        newpos,
		        (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1+13,
		        m.format(bar[0][i],4)
		        );
	        lastpos=newpos;
            }

            // BAR 2 //
            for(i=0,total=0;i<3;i++) {
	        total+=bar[1][i];
            }

            //##chwg0525
            if(total==0)total=1;

            for(i=0,lastpos=series_x1;i<3;i++) {
	        barlen=bar[1][i]*(series_x2-series_x1)/total;
	        if(barlen<=0)barlen=0;
	        newpos=lastpos+barlen;
	        user_interface.bar(
	            lastpos,
	            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2,
	            newpos,
	            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2+13,
	            mode_admission_bar_color[i]);
	        if(barlen>20)
	            font_barcharts.center_put(
		        lastpos,
		        (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2,
		        newpos,
		        (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2+13,
		        m.format(bar[1][i],4)
		        );
	        lastpos=newpos;
            }

            // BAR 3 //
            for(i=0,total=0;i<3;i++) {
	        total+=bar[2][i];
            }

            //##chwg0525
            if(total==0)total=1;

            for(i=0,lastpos=series_x1;i<3;i++) {
	        barlen=bar[2][i]*(series_x2-series_x1)/total;
	        if(barlen<=0)barlen=0;
	        newpos=lastpos+barlen;
	        user_interface.bar(
	            lastpos,
	            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3,
	            newpos,
	            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3+13,
	            mode_admission_bar_color[i]);
	        if(barlen>20)
	            font_barcharts.center_put(
		        lastpos,
		        (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3,
		        newpos,
		        (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3+13,
		        m.format(bar[2][i],4)
		        );
	        lastpos=newpos;
            }

            // BAR 4 //
            for(i=0,total=0;i<3;i++) {
	        total+=bar[3][i];
            }

            //##chwg0525
            if(total==0)total=1;

            for(i=0,lastpos=series_x1;i<3;i++) {
	        barlen=bar[3][i]*(series_x2-series_x1)/total;
	        if(barlen<=0)barlen=0;
	        newpos=lastpos+barlen;
	        user_interface.bar(
	            lastpos,
	            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4,
	            newpos,
	            (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4+13,
	            mode_admission_bar_color[i]);
	        if(barlen>20)
	            font_barcharts.center_put(
		        lastpos,
		        (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4,
		        newpos,
		        (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4+13,
		        m.format(bar[3][i],4)
		        );
	        lastpos=newpos;
            }

            // blit the statics
            vga.blt_buf((REPORT_X2)-150,
		        (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1,
		        (REPORT_X2),
		        (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1+font_chartsm.max_font_height);
            vga.blt_buf((REPORT_X2)-150,
		        (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2,
		        (REPORT_X2),
		        (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2+font_chartsm.max_font_height);
            vga.blt_buf((REPORT_X2)-150,
		        (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3,
		        (REPORT_X2),
		        (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3+font_chartsm.max_font_height);
            vga.blt_buf((REPORT_X2)-150,
		        (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4,
		        (REPORT_X2),
		        (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4+font_chartsm.max_font_height);

            // blit the bars
            vga.blt_buf((REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1,
		        (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1,
		        (REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1+MODE_ADMISSION_BOX_WIDTH2,
		        (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW1+14);
            vga.blt_buf((REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1,
		        (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2,
		        (REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1+MODE_ADMISSION_BOX_WIDTH2,
		        (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW2+14);
            vga.blt_buf((REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1,
		        (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3,
		        (REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1+MODE_ADMISSION_BOX_WIDTH2,
		        (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW3+14);
            vga.blt_buf((REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1,
		        (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4,
		        (REPORT_X1)+MODE_ADMISSION_BOX_WIDTH1+MODE_ADMISSION_BOX_WIDTH2,
		        (REPORT_Y1)+MODE_ADMISSION_BARCHART_ROW4+14);
            // end Kevin 0716
        }
    }
}
