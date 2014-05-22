// Filename    : OLOG.CPP
// Description : logging class
// Owner       : Gilbert

#include <OLOG.H>
#include <STDIO.H>
#include <OFILE.H>

// -------- begin of function Log::Log ----------//
Log::Log() {
}

// -------- end of function Log::Log ----------//

Log::~Log() {
}

// -------- begin of function Log::mark_begin ----------//
void Log::mark_begin() {
    // text_buffer[0] stores the latest log
    text_buffer[0].clear();
    *(text_buffer[0].reserve(sizeof(char))) = '\0'; // put a null character in the buffer
}

// -------- end of function Log::mark_begin ----------//

// -------- begin of function Log::mark_end ----------//
void Log::mark_end() {
    // enable next line to trace hang
    // dump();

    // rotate buffers
    text_buffer[LOG_VERSION-1].clear();
    // put a null character in the buffer
    *(text_buffer[LOG_VERSION-1].reserve(sizeof(char))) = '\0';
    for(int n = LOG_VERSION-1; n > 0; --n) {
	text_buffer[n].swap(text_buffer[n-1]);
    }
}

// -------- end of function Log::mark_end ----------//

// -------- begin of function Log::mark ----------//
void Log::mark(char *msg, char *file, int line) {
    log_text = msg;
    log_file = file;
    log_line = line;

    String t(log_text);
    t += "\r\n";
    strcpy( text_buffer[0].reserve(t.len())-1, t ); // minus 1 to remove the '\0' at the end
}

// -------- end of function Log::mark ----------//

// -------- begin of function Log::mark ----------//
void Log::mark(int n , char *file, int line) {
    log_text = n;
    log_file = file;
    log_line = line;

    String t(log_text);
    t += "\r\n";
    strcpy( text_buffer[0].reserve(t.len())-1, t );
}

// -------- end of function Log::mark ----------//

// -------- begin of function Log::dump ----------//
void Log::dump() {
    // write old_buffer
    char filename[20];
    strcpy(filename, "AM_A.LOG");
    File f;
    // AM_A.LOG, AM_B.LOG ...
    for(int n = LOG_VERSION-1; n >= 0; --n, filename[3]++) {
	f.file_create(filename);
	f.file_write( text_buffer[n].queue_buf, text_buffer[n].length() );
	f.file_close();
    }
}

// -------- end of function Log::dump ----------//

// -------- begin of function Log::debug_log ----------//
void Log::debug_log(char *msg) {
    String s;
    s = msg;
    s += "\r\n";
    OutputDebugString(s);
}

void Log::debug_log(int n) {
    String s;
    s = n;
    s += "\r\n";
    OutputDebugString(s);
}

// -------- end of function Log::debug_log ----------//
