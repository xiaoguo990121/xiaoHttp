
#ifndef XIAOHTTP_EXPORT_H
#define XIAOHTTP_EXPORT_H

#ifdef XIAOHTTP_STATIC_DEFINE
#  define XIAOHTTP_EXPORT
#  define XIAOHTTP_NO_EXPORT
#else
#  ifndef XIAOHTTP_EXPORT
#    ifdef xiaoHttp_EXPORTS
        /* We are building this library */
#      define XIAOHTTP_EXPORT 
#    else
        /* We are using this library */
#      define XIAOHTTP_EXPORT 
#    endif
#  endif

#  ifndef XIAOHTTP_NO_EXPORT
#    define XIAOHTTP_NO_EXPORT 
#  endif
#endif

#ifndef XIAOHTTP_DEPRECATED
#  define XIAOHTTP_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef XIAOHTTP_DEPRECATED_EXPORT
#  define XIAOHTTP_DEPRECATED_EXPORT XIAOHTTP_EXPORT XIAOHTTP_DEPRECATED
#endif

#ifndef XIAOHTTP_DEPRECATED_NO_EXPORT
#  define XIAOHTTP_DEPRECATED_NO_EXPORT XIAOHTTP_NO_EXPORT XIAOHTTP_DEPRECATED
#endif

/* NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if) */
#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef XIAOHTTP_NO_DEPRECATED
#    define XIAOHTTP_NO_DEPRECATED
#  endif
#endif

#endif /* XIAOHTTP_EXPORT_H */
