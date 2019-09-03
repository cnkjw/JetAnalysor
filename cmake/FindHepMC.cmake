########################################################
#
#    Copyright (c) 2019
#      KANG Jin-Wen
#
#    BSD 3-clause license
#
#########################################################

##################################################################
# This module tries to find the HepMC installation on your system.
# Usage:
#       FIND_PACKAGE( HepMC [REQUIRED] [COMPONENTS HepMCfio ...] )
#
# It sets the following variables:
#       ${HEPMC_FOUND}
#       ${HepMC_INCLUDE_DIRS}
#       ${HepMC_LIBRARY_DIRS}
#       ${HepMC_LIBRARIES}
#
#       ${HepMC_HEPEVT_SIZE}
#
##################################################################

message(STATUS "Looking for HepMC...")

# default hepmc common block size: 
#Set( HepMC_HEPEVT_SIZE 99990 )


## search for include path
find_path(HepMC_INCLUDE_DIRS
                NAMES
                HepMC/GenEvent.h

                PATHS
		/usr
		/usr/local
		${HEPMC_PREFIX}
                ${HEPMC_ROOT}
                ${HEPMC_ROOT_DIR}
                ${HEP_ROOT}
		$ENV{HEPMC_PREFIX}
                $ENV{HEPMC_ROOT}
                $ENV{HEPMC_ROOT_DIR}
                $ENV{HEP_ROOT}

                PATH_SUFFIXES
                include
	)
    
find_path(HepMC_LIBRARY_DIRS
                NAMES
                libHepMC.a

                PATHS
		/usr
		/usr/local
		${HEPMC_PREFIX}
                ${HEPMC_ROOT}
                ${HEPMC_ROOT_DIR}
                ${HEP_ROOT}
		$ENV{HEPMC_PREFIX}
                $ENV{HEPMC_ROOT}
                $ENV{HEPMC_ROOT_DIR}
                $ENV{HEP_ROOT}

                PATH_SUFFIXES
                lib
                lib64
	)

# set found flag
if (${HepMC_INCLUDE_DIRS} MATCHES "HepMC_INCLUDE_DIRS-NOTFOUND")
	set(HEPMC_FOUND FALSE)
else (${HepMC_INCLUDE_DIRS} MATCHES "HepMC_INCLUDE_DIRS-NOTFOUND")
	MESSAGE(STATUS "HepMC include path found: " ${HepMC_INCLUDE_DIRS} )
endif (${HepMC_INCLUDE_DIRS} MATCHES "HepMC_INCLUDE_DIRS-NOTFOUND")

## final printout

# if not found

if(${HepMC_FOUND} MATCHES "FALSE")
        if (HepMC_FIND_REQUIRED)
        	message( FATAL_ERROR
                		     "HepMC shared library or includes not found\n"
                                     "Please install HepMC and/or set HEPMC_PREFIX environment\n"
                                        "(use -DHEPMC_PREFIX)\n"
                                     )
        else (HepMC_FIND_REQUIRED)
                MESSAGE( WARNING
                		     "HepMC shared library or includes not found\n"
                                     "Please install HepMC and/or set HEPMC_PREFIX environment\n"
                                        "(use -DHEPMC_PREFIX)\n"
                                     )
        endif (HepMC_FIND_REQUIRED)


# end if not found
# if found :

else(${HepMC_FOUND} MATCHES "TRUE")
	SET(HepMC_FOUND TRUE)

# find length of hepmc common block pre-defined and use this for epos
        EXECUTE_PROCESS (COMMAND cat ${HepMC_INCLUDE_DIRS}/HepMC/HEPEVT_Wrapper.h
	                 COMMAND grep "#define HEPEVT_EntriesAllocation" 
			 COMMAND awk  "{print $3}"
			 COMMAND tr "\n" " "
			 COMMAND sed "s/ //"
			 OUTPUT_VARIABLE HepMC_HEPEVT_SIZE
	  )
	MESSAGE( INFO "-HepMC: HEPEVT_EntriesAllocation in file \"${HepMC_INCLUDE_DIRS}/HepMC/HEPEVT_Wrapper.h\" is \"${HepMC_HEPEVT_SIZE}\"" )
	MESSAGE( INFO "-HepMC: This limits the maximum number of particles per event you can have. " )
	MESSAGE( INFO "-HepMC: Change you HepMC library if you run into limitation! " )
    MESSAGE( STATUS "HepMC2 includes: " ${HepMC_INCLUDE_DIRS})
    MESSAGE( STATUS "HepMC2 library: " ${HepMC_LIBRARY_DIRS})

ENDIF(${HepMC_FOUND} MATCHES "FALSE")
