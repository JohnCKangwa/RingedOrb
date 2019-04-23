#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <queue>
#include <assert.h>
#include <unordered_map>

#include "roErrorHandling.h"
#include "roStringUtilities.h"


#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) {x->Release(); x = nullptr;}
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x) { delete[] x; x = nullptr;}
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) {delete x; x = nullptr;}
#endif

#ifndef CB256_ALIGN
#define CB256_ALIGN struct alignas(256) 
#endif 

#ifndef RO_INLINE
#define RO_INLINE inline
#endif 



