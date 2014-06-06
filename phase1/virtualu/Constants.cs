using virtualu.Widgets;

namespace virtualu
{
    enum RefreshType
    {
        Repaint,
        Update,
        Special
    }

    enum Season
    {
        Spring,
        Summer,
        Autumn
    }

    public static class Constants
    {
        public const short BG_PIC_ID = 4;
        public const short TRIMESTER_PER_YEAR = 3;
        public const short MAX_DEPARTMENT = 12;
        public const short MIN_DEPARTMENT = MAX_DEPARTMENT - 4;
        public const short MAX_COURSE_DEPTH_WITHOUT_GR = 3;
        public const short DEFAULT_REUSE_INTERVAL_DAYS = 3;
        public const short DEF_DYNARRAY_BLOCK_SIZE = 30;
        public const short HISTORY_YEAR_COUNT = 8;
        public const short HISTORY_TRIMESTER_COUNT = (short)(HISTORY_YEAR_COUNT * TRIMESTER_PER_YEAR);
        public const short HISTORY_MONTH_COUNT = 48;
        public const short STUDENT_TO_FACULTY_RATIO = 14;
        public const short MAX_PROTAGONIST_ID = 34;

        public const string PREFERENCE_DB = "STUPREF";
        public const string APP_YIELD_DB = "APPYIELD";
        public const string PREFERENCE_DB2 = "STUPREF2";
        public const string TALENT_DB = "STTALENT";
    }

    public static class Color
    {
        public const short VGA_BROWN = 0x01;
        public const short VGA_YELLOW = 0x10;
        public const short VGA_RED = 0x20;
        public const short VGA_SEA_GREEN = 0x30;
        public const short VGA_LAND_GRASS = 0x50;
        public const short VGA_LAND_HILL = 0x60;
        public const short VGA_GRAY = 0x70;
        public const short VGA_OLIVE = 0x80;
        public const short VGA_PINK = 0x90;
        public const short VGA_VIOLET = 0xA0;
        public const short VGA_SOIL = 0xB0;
        public const short VGA_ORANGE = 0xC0;
        public const short VGA_GRASS_GREEN = 0xD0;
        public const short VGA_IF_COLOR = 0xE0;
        public const short VGA_SEA_BLUE = 0xF0;
        public const short BLACK = 0;
        public const short GRAY = 122;
        public const short WHITE = VGA_GRAY;
        public const short ORANGE = VGA_ORANGE + 6;
        public const short RED = VGA_RED + 4;
        public const short GREEN = VGA_SEA_GREEN + 5;
        public const short BLUE = VGA_SEA_BLUE + 5;
        public const short VIOLET = VGA_VIOLET + 4;
        public const short BROWN = VGA_BROWN + 5;
        public const short PINK = VGA_PINK + 4;
        public const short YELLOW = VGA_YELLOW + 2;
        public const short BACKGROUND = 0xFF;
        public const short TRANS = 0;
        public const short OWN_SELECT_FRAME_COLOR = YELLOW;
    }

    public static class Fonts
    {
        public static Font font_san;
        public static Font font_std;
        public static Font font_small;
        public static Font font_mid;
        public static Font font_news;
        public static Font font_casa;
        // ##### begin chwg 24/7 ###########//
        public static Font font_yellow_diamond;
        public static Font font_fdiamond;
        public static Font font_fblack;
        public static Font font_charts;
        public static Font font_chartsm;
        public static Font font_chart_red_sm;
        public static Font font_chart_green_sm;
        public static Font font_chart_blue_sm;
        public static Font font_chart_orange_sm;
        public static Font font_chart_purple_sm;
        public static Font font_redcharts;
        public static Font font_barcharts;
        public static Font font_fscore;
        public static Font font_numbers;
        public static Font font_stone;
        public static Font font_hstone;
        public static Font font_small_san;
        public static Font font_red_san;
        public static Font font_green_san;
        public static Font font_blue_san;
        public static Font font_letter;
        public static Font font_pgold;
        public static Font font_psilver;
        public static Font font_pbronze;
    }
}
