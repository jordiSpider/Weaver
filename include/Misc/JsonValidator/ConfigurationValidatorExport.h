#ifndef CONFIGURATION_VALIDATOR_EXPORT_H_
#define CONFIGURATION_VALIDATOR_EXPORT_H_

#if defined(_WIN32)
#  if defined(WEAVER_CONFIG_VALIDATOR_BUILD)
#    define WEAVER_CONFIG_VALIDATOR_API __declspec(dllexport)
#  else
#    define WEAVER_CONFIG_VALIDATOR_API __declspec(dllimport)
#  endif
#else
#  define WEAVER_CONFIG_VALIDATOR_API __attribute__((visibility("default")))
#endif

#endif
