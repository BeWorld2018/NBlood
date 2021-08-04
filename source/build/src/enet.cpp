#include "compat.h" // for UNREFERENCED_PARAMETER

#define ENET_IMPLEMENTATION
#ifndef NETCODE_DISABLE
#ifdef __MORPHOS__
#include <enet/enet.h>
#else
#include "enet.h"
#endif
#endif
