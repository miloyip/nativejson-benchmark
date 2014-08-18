#include "../memorystat.h"

#ifndef NDEBUG
#define JSON_DEBUG
#endif

#define JSON_LIBRARY
#include "libjson/_internal/Source/JSONAllocator.cpp"
#include "libjson/_internal/Source/internalJSONNode.cpp"
#include "libjson/_internal/Source/JSONChildren.cpp"
#include "libjson/_internal/Source/JSONDebug.cpp"
#include "libjson/_internal/Source/JSONIterators.cpp"
#include "libjson/_internal/Source/JSONMemory.cpp"
#include "libjson/_internal/Source/JSONNode.cpp"
#include "libjson/_internal/Source/JSONNode_Mutex.cpp"
#include "libjson/_internal/Source/JSONPreparse.cpp"
#include "libjson/_internal/Source/JSONStream.cpp"
#include "libjson/_internal/Source/JSONValidator.cpp"
#include "libjson/_internal/Source/JSONWorker.cpp"
#include "libjson/_internal/Source/JSONWriter.cpp"
#include "libjson/_internal/Source/libjson.cpp"
