#pragma once

#ifndef BUFSOCK_EXPORTS
# if (defined _WIN32 || defined WINCE || defined __CYGWIN__)
#   if defined(BUFFEREDSOCKET_EXPORTS)
#       define BUFSOCK_EXPORTS __declspec(dllexport)
#   else
#       define BUFSOCK_EXPORTS __declspec(dllimport)
#   endif
# elif defined __GNUC__ && __GNUC__ >= 4 && (defined(BUFFEREDSOCKET_EXPORTS) || defined(__APPLE__))
#   define BUFSOCK_EXPORTS __attribute__ ((visibility ("default")))
# else
#   define BUFSOCK_EXPORTS
# endif
#endif
