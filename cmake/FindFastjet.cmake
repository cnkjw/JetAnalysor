# - Try to find Fastjet
# This will define:
#
#  Fastjet_FOUND
#  Fastjet_CONFIG
#  Fastjet_INCLUDE_DIR
#  Fastjet_LIBRARIES
#  Fastjet_VERSION
#  Fastjet_LIBFLAGS

########################################################
#
#    Copyright (c) 2019
#      KANG Jin-Wen
#
#    BSD 3-clause license
#
#########################################################


include(FindPackageHandleStandardArgs)
find_program(Fastjet_CONFIG NAMES Fastjet-config HINTS $ENV{FastjetSYS}/bin)
if(Fastjet_CONFIG)
   message(STATUS "Looking for Fastjet...")

   execute_process(COMMAND "${Fastjet_CONFIG}" --prefix
      OUTPUT_VARIABLE FastjetSYS OUTPUT_STRIP_TRAILING_WHITESPACE)

   execute_process(COMMAND "${Fastjet_CONFIG}" --version
      OUTPUT_VARIABLE Fastjet_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
   string(REPLACE "/" "." Fastjet_VERSION "${Fastjet_VERSION}")

   execute_process(COMMAND "${Fastjet_CONFIG}" --cxxflags
      OUTPUT_VARIABLE Fastjet_INCDIR OUTPUT_STRIP_TRAILING_WHITESPACE)
   string(REPLACE "-I" "" Fastjet_INCDIR "${Fastjet_INCDIR}")
   set(Fastjet_INCLUDE_DIR ${Fastjet_INCDIR} CACHE PATH "path to Fastjet includes")
   message(STATUS "Fastjet includes: " ${Fastjet_INCDIR})

   set(Fastjet_LIBRARIES ${FastjetSYS}/lib CACHE PATH "linker flags for Fastjet")
   message(STATUS "Fastjet library: " ${Fastjet_LIBRARIES})

   execute_process(COMMAND "${Fastjet_CONFIG}" --libs
           OUTPUT_VARIABLE Fastjet_LIBFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
   #message(STATUS "Fastjet compile options: " ${Fastjet_LIBFLAGS})

   else(${Fastjet_CONFIG} MATCHS "Fastjet_CONFIG-NOTFOUND")
      message(STATUS "Looking for Fastjet... - fastjet-config not found, please specify it with \"-DFastjet_CONFIG\"")
endif()

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Fastjet
   REQUIRED_VARS Fastjet_CONFIG Fastjet_INCLUDE_DIR Fastjet_LIBRARIES
   VERSION_VAR Fastjet_VERSION
   FAIL_MESSAGE "required Fastjet library not found"
   )

mark_as_advanced(Fastjet_CONFIG)
