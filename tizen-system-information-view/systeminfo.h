#ifndef __systeminfo_H__
#define __systeminfo_H__

#include <app.h>
#include <Elementary.h>
#include <system_settings.h>
#include <efl_extension.h>
#include <dlog.h>

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "systeminfo"

#if !defined(PACKAGE)
#define PACKAGE "org.example.systeminfo"
#endif

#endif /* __systeminfo_H__ */
