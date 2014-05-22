#include <ODEBUGX.H>
#include <OMISC.H>

//------------- begin of function DebugX::DebugX -------------//
DebugX::DebugX() {
    debug_session = m.is_file_exist("DEBUG.SYS");
    int debug = 0;
}

//------------- end of function DebugX::DebugX -------------//

//----------- begin of function DebugX::is_debug() -----------//
int DebugX::is_debug() {
    return debug_session;
}

//----------- end of function DebugX::is_debug() -----------//
