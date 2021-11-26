#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/EMMS_Code_CommandBoard.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/EMMS_Code_CommandBoard.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS
SUB_IMAGE_ADDRESS_COMMAND=--image-address $(SUB_IMAGE_ADDRESS)
else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=PowerMain.c EEPROM.c MasterComm.c LEDControl.c ResetReport.c _ConfigPragma.c RTCC.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/PowerMain.o ${OBJECTDIR}/EEPROM.o ${OBJECTDIR}/MasterComm.o ${OBJECTDIR}/LEDControl.o ${OBJECTDIR}/ResetReport.o ${OBJECTDIR}/_ConfigPragma.o ${OBJECTDIR}/RTCC.o
POSSIBLE_DEPFILES=${OBJECTDIR}/PowerMain.o.d ${OBJECTDIR}/EEPROM.o.d ${OBJECTDIR}/MasterComm.o.d ${OBJECTDIR}/LEDControl.o.d ${OBJECTDIR}/ResetReport.o.d ${OBJECTDIR}/_ConfigPragma.o.d ${OBJECTDIR}/RTCC.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/PowerMain.o ${OBJECTDIR}/EEPROM.o ${OBJECTDIR}/MasterComm.o ${OBJECTDIR}/LEDControl.o ${OBJECTDIR}/ResetReport.o ${OBJECTDIR}/_ConfigPragma.o ${OBJECTDIR}/RTCC.o

# Source Files
SOURCEFILES=PowerMain.c EEPROM.c MasterComm.c LEDControl.c ResetReport.c _ConfigPragma.c RTCC.c



CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/EMMS_Code_CommandBoard.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=24FV32KA302
MP_LINKER_FILE_OPTION=,--script=p24FV32KA302.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/PowerMain.o: PowerMain.c  .generated_files/flags/default/77c9f45d7870d43afefa980abb0e3968d50b2383 .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/PowerMain.o.d 
	@${RM} ${OBJECTDIR}/PowerMain.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  PowerMain.c  -o ${OBJECTDIR}/PowerMain.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/PowerMain.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/PIC24F/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/generic/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/EEPROM.o: EEPROM.c  .generated_files/flags/default/1ccf93c9f65c569cde8dcddca6876339672b114a .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/EEPROM.o.d 
	@${RM} ${OBJECTDIR}/EEPROM.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  EEPROM.c  -o ${OBJECTDIR}/EEPROM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/EEPROM.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/PIC24F/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/generic/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/MasterComm.o: MasterComm.c  .generated_files/flags/default/8e357bf1f0df26b6f15da9ab50d59e5a8513e3fd .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/MasterComm.o.d 
	@${RM} ${OBJECTDIR}/MasterComm.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  MasterComm.c  -o ${OBJECTDIR}/MasterComm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/MasterComm.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/PIC24F/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/generic/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/LEDControl.o: LEDControl.c  .generated_files/flags/default/15fd3fe24b7f4887188101c04504adb088b57315 .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/LEDControl.o.d 
	@${RM} ${OBJECTDIR}/LEDControl.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  LEDControl.c  -o ${OBJECTDIR}/LEDControl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/LEDControl.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/PIC24F/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/generic/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/ResetReport.o: ResetReport.c  .generated_files/flags/default/8b979295b26489ab22078ed5743cb38932b3808a .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ResetReport.o.d 
	@${RM} ${OBJECTDIR}/ResetReport.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ResetReport.c  -o ${OBJECTDIR}/ResetReport.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/ResetReport.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/PIC24F/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/generic/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ConfigPragma.o: _ConfigPragma.c  .generated_files/flags/default/49c4615fa15dcd0fa21c7d67039c06ffc03f3124 .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/_ConfigPragma.o.d 
	@${RM} ${OBJECTDIR}/_ConfigPragma.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  _ConfigPragma.c  -o ${OBJECTDIR}/_ConfigPragma.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ConfigPragma.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/PIC24F/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/generic/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/RTCC.o: RTCC.c  .generated_files/flags/default/9cf0086b29d40d9a203252e17cb92ef1ad399ce3 .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/RTCC.o.d 
	@${RM} ${OBJECTDIR}/RTCC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  RTCC.c  -o ${OBJECTDIR}/RTCC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/RTCC.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/PIC24F/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/generic/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
else
${OBJECTDIR}/PowerMain.o: PowerMain.c  .generated_files/flags/default/3204335b863fa534d633e656b5ae08d017870295 .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/PowerMain.o.d 
	@${RM} ${OBJECTDIR}/PowerMain.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  PowerMain.c  -o ${OBJECTDIR}/PowerMain.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/PowerMain.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/PIC24F/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/generic/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/EEPROM.o: EEPROM.c  .generated_files/flags/default/9c89edbc799bae998bcd991cbad0163a0a706e3f .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/EEPROM.o.d 
	@${RM} ${OBJECTDIR}/EEPROM.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  EEPROM.c  -o ${OBJECTDIR}/EEPROM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/EEPROM.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/PIC24F/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/generic/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/MasterComm.o: MasterComm.c  .generated_files/flags/default/e028918f614b2d11efd26ae166163e2f164101c4 .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/MasterComm.o.d 
	@${RM} ${OBJECTDIR}/MasterComm.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  MasterComm.c  -o ${OBJECTDIR}/MasterComm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/MasterComm.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/PIC24F/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/generic/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/LEDControl.o: LEDControl.c  .generated_files/flags/default/3bd64f7eaa8d68cc04f2472cea7a196a8001c773 .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/LEDControl.o.d 
	@${RM} ${OBJECTDIR}/LEDControl.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  LEDControl.c  -o ${OBJECTDIR}/LEDControl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/LEDControl.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/PIC24F/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/generic/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/ResetReport.o: ResetReport.c  .generated_files/flags/default/b956152c8e2c7a5c1c49c2f357f5f50b241c6395 .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ResetReport.o.d 
	@${RM} ${OBJECTDIR}/ResetReport.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ResetReport.c  -o ${OBJECTDIR}/ResetReport.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/ResetReport.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/PIC24F/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/generic/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ConfigPragma.o: _ConfigPragma.c  .generated_files/flags/default/9041f6c1b0248d93e025eef6c114346292d0e6e9 .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/_ConfigPragma.o.d 
	@${RM} ${OBJECTDIR}/_ConfigPragma.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  _ConfigPragma.c  -o ${OBJECTDIR}/_ConfigPragma.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ConfigPragma.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/PIC24F/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/generic/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/RTCC.o: RTCC.c  .generated_files/flags/default/9c2adae4a3850b2f4476e491137a6096a49fb2fe .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/RTCC.o.d 
	@${RM} ${OBJECTDIR}/RTCC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  RTCC.c  -o ${OBJECTDIR}/RTCC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/RTCC.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/PIC24F/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/generic/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/EMMS_Code_CommandBoard.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/EMMS_Code_CommandBoard.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG=__DEBUG   -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mreserve=data@0x800:0x81F -mreserve=data@0x820:0x821 -mreserve=data@0x822:0x823 -mreserve=data@0x824:0x825 -mreserve=data@0x826:0x84F   -Wl,,,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D__DEBUG=__DEBUG,,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST)  -mdfp="${DFP_DIR}/xc16" 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/EMMS_Code_CommandBoard.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/EMMS_Code_CommandBoard.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -Wl,,,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST)  -mdfp="${DFP_DIR}/xc16" 
	${MP_CC_DIR}\\xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/EMMS_Code_CommandBoard.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf   -mdfp="${DFP_DIR}/xc16" 
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
