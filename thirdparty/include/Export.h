
#ifndef JSONBOX_EXPORT_H
#define JSONBOX_EXPORT_H

#ifdef JSONBOX_STATIC_DEFINE
#  define JSONBOX_EXPORT
#  define JSONBOX_NO_EXPORT
#else
#  ifndef JSONBOX_EXPORT
#    ifdef JsonBox_EXPORTS
        /* We are building this library */
#      define JSONBOX_EXPORT 
#    else
        /* We are using this library */
#      define JSONBOX_EXPORT 
#    endif
#  endif

#  ifndef JSONBOX_NO_EXPORT
#    define JSONBOX_NO_EXPORT 
#  endif
#endif

#ifndef JSONBOX_DEPRECATED
#  define JSONBOX_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef JSONBOX_DEPRECATED_EXPORT
#  define JSONBOX_DEPRECATED_EXPORT JSONBOX_EXPORT JSONBOX_DEPRECATED
#endif

#ifndef JSONBOX_DEPRECATED_NO_EXPORT
#  define JSONBOX_DEPRECATED_NO_EXPORT JSONBOX_NO_EXPORT JSONBOX_DEPRECATED
#endif

#define DEFINE_NO_DEPRECATED 0
#if DEFINE_NO_DEPRECATED
# define JSONBOX_NO_DEPRECATED
#endif

#endif
