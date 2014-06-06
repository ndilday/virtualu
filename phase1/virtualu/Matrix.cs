using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace virtualu
{
    /// <summary>
    /// An enumeration of the types of different visual objects 
    /// allowed on the display area
    /// </summary>
    public enum ObjectType
    {
        Firm,
        Plant,
        Vehicle,
        Sprite,
    }

    /// <summary>
    /// The levels of zoom the program allows
    /// </summary>
    public enum ZoomLevel
    {
        Small,
        Medium,
        Large
    }

    /// <summary>
    /// UI structure for sorting UI objects, such as bitmaps.
    /// </summary>
    struct DisplaySort 
    {
        ObjectType  object_type;
        short object_recno;
        short depth;
        short loc_x,loc_y;
        short abs_x1, abs_x2, abs_y1, abs_y2;
        char* bitmap_ptr;
    }

    /// <summary>
    /// Defines what exists at a given location
    /// </summary>
    enum LocationType{
        Empty,
        Firm,
        Road,
        RoadSub,    // the top-right, bottom-left & bottom-right of the road
        Plant,
        Sprite,
    }

    /// <summary>
    /// Structure representing the location and features of physical facilities
    /// (firms, plants, vehicles, and sprites) on campus. (Location class is deprecated).
    /// </summary>
    struct Location {
        bool loc_flag;
        LocationType    loc_type;
        short    cargo_recno;
        bool   extra_para;
        int    terrain_id;

        bool is_empty()            
        { 
            return loc_type == LocationType.Empty; 
        }

        void set_terrain(int terrainId)  
        {
            terrain_id = terrainId; 
            set_walkable_on(); 
        }

#region Sprite Animation Functions
        bool is_walkable() { return loc_flag; }
        void set_walkable_on() { loc_flag = true; }
        void set_walkable_off() { loc_flag = false; }
#endregion

#region Firm Functions
        void set_firm(int firmRecno);
        void remove_firm();

        bool is_firm() { return loc_type == LocationType.Firm; }
        int firm_recno() { return cargo_recno; }
#endregion

#region Plant Functions
        bool is_plant() { return loc_type == LocationType.Plant; }
        void set_plant(int plantId, int offsetX, int offsetY);
        void remove_plant();

        int plant_id()
        {
            return is_plant() ? extra_para : 0;
        }

        int plant_inner_x() 
        {                       // a percentage from 0% to 100%
	        return cargo_recno & 0xFF;
        }
        int   plant_inner_y() 
        {                       // a percentage from 0% to 100%
	        return cargo_recno >> 8;
        }
#endregion

#region Road Functions
        bool is_road() { return loc_type == LocationType.Road; }
        bool is_road_sub() { return loc_type == LocationType.RoadSub; }
        void set_road(int roadId);
        void set_road_sub() 
        { 
            loc_type = LocationType.RoadSub;
            set_walkable_on();
        }
        void remove_road();
        //TO? add remove_road_sub();
        int road_id() 
        {
            return is_road() ? cargo_recno : 0;
        }
#endregion

#region Sprite Functions
        void    set_sprite(int spriteRecno);
        void    remove_sprite();

        bool   has_sprite()    { return is_walkable() && extra_para; }
        bool   sprite_recno()    { return has_sprite() ? extra_para : false; }
#endregion
    }

    /// <summary>
    /// 
    /// </summary>
    struct DynArray
    {
    }

    /// <summary>
    /// Class representing the Main map of the physical layout of the campus.
    /// Handles the drawing of objects on the main map.
    /// </summary>
    class Matrix 
    {
#region Static Variables
        public static readonly short MAX_WORLD_X_LOC = 80;
        public static readonly short MAX_WORLD_Y_LOC = 80;
        public static readonly short VGA_WIDTH = 800;
        public static readonly short VGA_HEIGHT = 600;
        public static readonly short VGA_BPP = 16;

        public static readonly int MAP_X1 = (MAP_X2 - MAP_WIDTH + 1);
        public static readonly int MAP_Y1 = (MAP_Y2 - MAP_HEIGHT + 1);
        public static readonly int MAP_X2 = (VGA_WIDTH - 7);
        public static readonly int MAP_Y2 = (VGA_HEIGHT - 5);

        // 40 (MAX_WORLD_X_LOC/2) * 8 (MAP_LOC_WIDTH)  = 320
        public static readonly int MAP_WIDTH = (MAX_WORLD_X_LOC / 2) * MAP_LOC_WIDTH;
        // 40 (MAX_WORLD_Y_LOC/2) * 4 (MAP_LOC_HEIGHT) = 160
        public static readonly int MAP_HEIGHT = (MAX_WORLD_Y_LOC / 2) * MAP_LOC_HEIGHT;

        public static readonly short MAP_LOC_WIDTH = 4;
        public static readonly short MAP_LOC_HEIGHT = 2;

        public static readonly short MAP_BORDER_WIDTH = 2;
        public static readonly short MAP_BORDER_HEIGHT = 2;

        //----------- Zoom window -------------//
        public static readonly short ZOOM_X1 = 33;
        public static readonly short ZOOM_Y1 = 58;
        public static readonly short ZOOM_X2 = 769;
        public static readonly short ZOOM_Y2 = 477;

        public static readonly int ZOOM_WIDTH = (ZOOM_X2 - ZOOM_X1 + 1);
        public static readonly int ZOOM_HEIGHT = (ZOOM_Y2 - ZOOM_Y1 + 1);

        //------- 1st level zoom --------//
        public static readonly short ZOOM1_LOC_WIDTH = 32;
        public static readonly short ZOOM1_LOC_HEIGHT = 16;

        //------ 2nd level zoom ---------//
        public static readonly short ZOOM2_LOC_WIDTH = 64;
        public static readonly short ZOOM2_LOC_HEIGHT = 32;
#endregion

#region Public Fields
        public int      max_x_loc, max_y_loc;
        public Location loc_matrix;
        public int      loc_width, loc_height;

        public int      top_x_loc, top_y_loc;                // the top left location of current zoom window
        public int   abs_top_x, abs_top_y;
        public int      disp_x_loc, disp_y_loc;              // calculated in Matrix()

        public int      win_x1, win_y1, win_x2, win_y2;
        public int   win_width, win_height;

        public bool    save_image_flag;
        public bool  save_image_buf;
        public bool   is_image_buf_latest;

        public ZoomLevel    zoom_level;
#endregion

        static DynArray disp_sort_array;

#region Public Functions
        public Matrix();
        public ~Matrix();

        public void init(int winX1, int winY1, int winX2, int winY2, 
            int locWidth, int locHeight, Location locMatrix, 
            int maxXLoc, int maxYLoc, int zoomLevel, bool saveImageFlag = false);

        public virtual void  draw_all();                     // draw everything on the map
        public virtual void  draw_update();                  // called each frame, only draw portions that need updating
        public virtual int   detect()    { return 0; }

        public void reset_image_buf() { is_image_buf_latest = false; }
        public void put_center_text(int x, int y, char* str);
        public void set_top_loc(int topXLoc, int topYLoc);
        public void loc_to_abs_top_left(out int absX, out int absY, int locX, int locY);
        public void loc_to_abs_center_left(out int absX, out int absY, int locX, int locY);
        public void loc_to_abs_bottom_right(out int absX, out int absY, int locX, int locY);
        public void abs_to_loc(out int locX, out int locY, int absX, int absY);
#endregion

        protected virtual void draw_text(); // called by draw_all

#region Private Functions
        void draw_objects();
        void draw_objects_now(DynArray* dispSortArray);

        void draw_terrain(int xLoc, int yLoc, int terrainId);
        void draw_plant(int xLoc, int yLoc, int terrainId);
        void draw_bitmap(int absX, int absY, string bmpPtr);
#endregion
    }

    //----------- define class MapMatrix -----------//

    /// <summary>
    /// Class which handles the drawing and UI relating to the Campus map.
    /// </summary>
    class MapMatrix : Matrix 
    {
        public void draw_all();
        public void draw_update();

        public int detect();
        public void scroll(int scrollX, int scrollY);

        void draw_zoom_box(bool useVgaBack);
    }

    /// <summary>
    /// Class which handles the drawing and UI relating to zooming in and out on the world.
    /// </summary>
    class ZoomMatrix : Matrix 
    {
        ulong next_scroll_time;

        public ZoomMatrix();
        public void  draw_all();
        public void  draw_text();
        public void  draw_update();
        public int   detect();

        int   detect_scroll();
        int   detect_select();
    }
}
