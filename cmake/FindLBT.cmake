###############################################################################
#                                                                             #
#       FindLBT.cmake,  Version: 1.0                                          #
#                                                                             #
#       Copyright (C) 2019                                                    #
#       GPL v3 License                                                        #
#                                                                             #
#       Author: Kang Jin-Wen                                                  #
#       Date: 2019-05-28                                                      #
#                                                                             #
#       Changelogs:                                                           #
#           2019-09-01 +0800 09:56 Create                                     #
#                                                                             #
###############################################################################

################################# DESCRIPTION #################################
#                                                                             #
# This module tries to find the LBT installation on your system.              #
#                                                                             #
# Usage:                                                                      #
#       find_package( LBT REQUIRED )                                          #
#                                                                             #
# It sets the following variables:                                            #
#       ${LBT_FOUND}                                                          #
#       ${LBT_PREFIX}                                                         #
#       ${LBT_INCLUDE_DIRS}                                                   #
#       ${LBT_LIBRARY_DIRS}                                                   #
#                                                                             #
###############################################################################

message(STATUS "Looking for LBT...")

# search for include path
find_path(LBT_INCLUDE_DIRS 
        NAMES
            einhard/einhard.hpp
            yaml-cpp/yaml.h
            LBT.h
        
        PATHS
            /usr
            /usr/local
            ${LBT_PREFIX}
            $ENV{LBT_PREFIX}
        
        PATH_SUFFIXES
            include
)

#search for lib path
find_path(LBT_LIBRARY_DIRS
        NAMES
            libLBT.a
        
        PATHS
            /usr
            /usr/local
            ${LBT_PREFIX}
            $ENV{LBT_PREFIX}

        PATH_SUFFIXES
            lib
            lib64
)

# set found flag
if (${LBT_INCLUDE_DIRS} MATCHES "LBT_INCLUDE_DIRS-NOTFOUND")
    set(LBT_FOUND FALSE)
else ()
    # message(STATUS "LBT include path found: ${LBT_INCLUDE_DIRS}")
    set(LBT_FOUND TRUE)
endif (${LBT_INCLUDE_DIRS} MATCHES "LBT_INCLUDE_DIRS-NOTFOUND")

if (${LBT_LIBRARY_DIRS} MATCHES "LBT_LIBRARY_DIRS-NOTFOUND")
    set(LBT_FOUND FALSE)
else ()
    # message(STAUTS "LBT lib path found: ${LBT_LIBRARY_DIRS}")
    set(LBT_FOUND TRUE)
endif (${LBT_LIBRARY_DIRS} MATCHES "LBT_LIBRARY_DIRS-NOTFOUND")

# if not found
if (${LBT_FOUND} MATCHES "FALSE")
    if (LBT_FIND_REQUIRED)
        message(FATAL_ERROR 
            "LBT static library or include not found\n"
                "\t Please install LBT and/or set LBT_PREFIX environment\n"
                "\t (use -DLBT_PREFIX)\n"    
        )
    else ()
        message(WARNING
            "LBT static library or include not found\n"
                "\t Please install LBT and/or set LBT_PREFIX environment\n"
                "\t (use -DLBT_PREFIX)\n"    
        )
    endif (LBT_FIND_REQUIRED)
else ()
    message(STATUS "LBT include: " ${LBT_INCLUDE_DIRS})
    message(STATUS "LBT library: " ${LBT_LIBRARY_DIRS})
endif (${LBT_FOUND} MATCHES "FALSE")
