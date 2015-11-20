#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/database_mysql.o \
	${OBJECTDIR}/east/value.o \
	${OBJECTDIR}/easy/class.o \
	${OBJECTDIR}/easy/module.o \
	${OBJECTDIR}/easy/util.o \
	${OBJECTDIR}/easy/value_array.o \
	${OBJECTDIR}/fruit.o


# C Compiler Flags
CFLAGS=-m64

# CC Compiler Flags
CCFLAGS=-m64 -std=c++11
CXXFLAGS=-m64 -std=c++11

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libwuf.${CND_DLIB_EXT}

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libwuf.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libwuf.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -shared -fPIC

${OBJECTDIR}/database_mysql.o: database_mysql.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/../php/include/php -I/../php/include/php/Zend -I/../php/include/php/main -I/../php/include/php/TSRM -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/database_mysql.o database_mysql.cpp

${OBJECTDIR}/east/value.o: east/value.cpp 
	${MKDIR} -p ${OBJECTDIR}/east
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/../php/include/php -I/../php/include/php/Zend -I/../php/include/php/main -I/../php/include/php/TSRM -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/east/value.o east/value.cpp

${OBJECTDIR}/easy/class.o: easy/class.cpp 
	${MKDIR} -p ${OBJECTDIR}/easy
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/../php/include/php -I/../php/include/php/Zend -I/../php/include/php/main -I/../php/include/php/TSRM -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/easy/class.o easy/class.cpp

${OBJECTDIR}/easy/module.o: easy/module.cpp 
	${MKDIR} -p ${OBJECTDIR}/easy
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/../php/include/php -I/../php/include/php/Zend -I/../php/include/php/main -I/../php/include/php/TSRM -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/easy/module.o easy/module.cpp

${OBJECTDIR}/easy/util.o: easy/util.cpp 
	${MKDIR} -p ${OBJECTDIR}/easy
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/../php/include/php -I/../php/include/php/Zend -I/../php/include/php/main -I/../php/include/php/TSRM -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/easy/util.o easy/util.cpp

${OBJECTDIR}/easy/value_array.o: easy/value_array.cpp 
	${MKDIR} -p ${OBJECTDIR}/easy
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/../php/include/php -I/../php/include/php/Zend -I/../php/include/php/main -I/../php/include/php/TSRM -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/easy/value_array.o easy/value_array.cpp

${OBJECTDIR}/fruit.o: fruit.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/../php/include/php -I/../php/include/php/Zend -I/../php/include/php/main -I/../php/include/php/TSRM -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/fruit.o fruit.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libwuf.${CND_DLIB_EXT}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
