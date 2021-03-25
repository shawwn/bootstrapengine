#include "graphics_afx.h"

extern int ( *EdPrintF )( const char *fmt, ... );

// unfortunately, namespacing float.h and math.h doesn't play well 
// with 3rdParty libraries such as PhysX, so disable that for now.
//using namespace __INTERNAL_MATH_IMPL;
