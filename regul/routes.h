
#if 1
#include "FS.h"
#include <LittleFS.h>
#define FSUSED LittleFS
#else
#include "FSUSED.h"
#define FSUSED FSUSED
#endif

void setup_http_routes(AsyncWebServer* server);
String processor(const String& var);
