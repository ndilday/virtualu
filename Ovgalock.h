// Filename    : OVGALOCK.H
// Description : header file for VgaFrontLock
// Owner       : Gilbert

#ifndef __OVGALOCK_H
#define __OVGALOCK_H

//!Used to vga_front.temp_unlock and temp_restore_lock. Mainly used by
//!ODPLAY.CPP to separate OVGA.H
class VgaFrontLock {
public:
    VgaFrontLock();
    ~VgaFrontLock();
    void  re_lock();
    void  re_unlock();
};

//!Class used for saving and restoring custome VGA palettes.
class VgaCustomPalette {
private:
    void *backup_pal;

public:
    VgaCustomPalette( char * );
    ~VgaCustomPalette();

    static int set_custom_palette(char *);

private:
    int save_palette();
    int restore_palette();
};

//!Class which preforms vga_front.temp_lock and unlock and shows cursor.
class MouseDispCount {
private:
    void *cursor_handle;
public:
    MouseDispCount();
    ~MouseDispCount();
};
#endif
