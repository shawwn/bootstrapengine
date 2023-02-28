#include "editlib_afx.h"

// use Common's PrintF by default.
int ( *EdPrintF )( const char *fmt, ... ) = PrintF;
