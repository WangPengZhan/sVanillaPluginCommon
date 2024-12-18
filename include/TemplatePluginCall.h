#pragma once
#include <PluginConfig.h>

struct Plugin;
typedef Plugin *PluginHandle;
typedef const char *PSTR;

#ifdef __cplusplus

extern "C"
{
#endif
    PluginHandle PLUGIN_API pluginInit();
    void PLUGIN_API pluginDeinit();
#ifdef __cplusplus
}
#endif
