#pragma once
#include <PluginConfig.h>

struct Plugin;
typedef Plugin* PluginHandle;
typedef const char* PSTR;

#ifdef __cplusplus

extern "C" {
#endif
void PLUGIN_API initDir(const char* dir);
PluginHandle PLUGIN_API pluginInit();
void PLUGIN_API pluginDeinit();
void PLUGIN_API deinit();
#ifdef __cplusplus
}
#endif
