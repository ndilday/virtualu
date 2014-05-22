// Filename    : OVOLUME.H
// Description : audio volume unit

#ifndef __OVOLUME_H
#define __OVOLUME_H

class DsVolume;
class AbsVolume;
class RelVolume;
class PosVolume;

//!System class for controlling the volume.
class DsVolume {
public:
    long  ds_vol;                                 // -10,000 to 0 (DSBVOLUME_MIN to DSBVOLUME_MAX)
    long  ds_pan;                                 // -10,000 to 10,000

public:
    DsVolume(long dsVol, long dsPan);
    DsVolume(AbsVolume &);
    DsVolume(RelVolume &);
};

//!Audio class representing master volume
class AbsVolume {
public:
    long  abs_vol;
    long  ds_pan;

public:
    AbsVolume(long absVol, long dsPan);
    AbsVolume(DsVolume &);
};

//!Interface for controlling the audio volume using a relative scale
//!(1-100).
class RelVolume {
public:
    long  rel_vol;                                // 0 to 100
    long  ds_pan;                                 // -10,000 to 10,000

public:
    RelVolume() {}
    RelVolume(long relVol, long dsPan);
    RelVolume(PosVolume &);
    RelVolume(PosVolume &, int drop, int limit);
};

//!Position of volume. Structure for maintaining UI to audio volume
//!controls.
class PosVolume {
public:
    long  x;
    long  y;

public:
    PosVolume(long relLocX, long relLocY);
};

extern RelVolume DEF_REL_VOLUME;
#endif
