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
${OBJECTDIR}/PowerMain.o: PowerMain.c  .generated_files/flags/default/d12bc1dc6597f798e6afa51794daba9eca8ed947 .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/PowerMain.o.d 
	@${RM} ${OBJECTDIR}/PowerMain.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  PowerMain.c  -o ${OBJECTDIR}/PowerMain.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/PowerMain.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/PIC24F/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/generic/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/EEPROM.o: EEPROM.c  .generated_files/flags/default/25d4ea2121cc2ec1b3e5d6b2bfd021f8ef1771fe .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/EEPROM.o.d 
	@${RM} ${OBJECTDIR}/EEPROM.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  EEPROM.c  -o ${OBJECTDIR}/EEPROM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/EEPROM.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/PIC24F/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/generic/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/MasterComm.o: MasterComm.c  .generated_files/flags/default/4c0c021a2f7b93a668aed24f9c3d88803db28b84 .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/MasterComm.o.d 
	@${RM} ${OBJECTDIR}/MasterComm.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  MasterComm.c  -o ${OBJECTDIR}/MasterComm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/MasterComm.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/PIC24F/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/generic/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/LEDControl.o: LEDControl.c  .generated_files/flags/default/211c3e7b1082a10f46d07c5d20e35e10d02ffe3a .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/LEDControl.o.d 
	@${RM} ${OBJECTDIR}/LEDControl.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  LEDControl.c  -o ${OBJECTDIR}/LEDControl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/LEDControl.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/PIC24F/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/generic/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/ResetReport.o: ResetReport.c  .generated_files/flags/default/39aa89695632992ba19b603d9e4fad0e31379902 .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ResetReport.o.d 
	@${RM} ${OBJECTDIR}/ResetReport.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ResetReport.c  -o ${OBJECTDIR}/ResetReport.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/ResetReport.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/PIC24F/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/generic/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ConfigPragma.o: _ConfigPragma.c  .generated_files/flags/default/a4351cb51cdd5b6799ba5c19beebc1eecd787399 .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/_ConfigPragma.o.d 
	@${RM} ${OBJECTDIR}/_ConfigPragma.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  _ConfigPragma.c  -o ${OBJECTDIR}/_ConfigPragma.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ConfigPragma.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/PIC24F/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/generic/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/RTCC.o: RTCC.c  .generated_files/flags/default/76e97ccc22be4526d5a8d866157546d57c399a86 .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/RTCC.o.d 
	@${RM} ${OBJECTDIR}/RTCC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  RTCC.c  -o ${OBJECTDIR}/RTCC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/RTCC.o.d"      -g -D__DEBUG     -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/PIC24F/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/generic/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
else
${OBJECTDIR}/PowerMain.o: PowerMain.c  .generated_files/flags/default/1d8ec8c0f1c9991a50acfbf75c9dfbb1e01acd15 .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/PowerMain.o.d 
	@${RM} ${OBJECTDIR}/PowerMain.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  PowerMain.c  -o ${OBJECTDIR}/PowerMain.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/PowerMain.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/PIC24F/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/generic/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/EEPROM.o: EEPROM.c  .generated_files/flags/default/530898c60017e699fe762ac421e8e8de08c8b288 .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/EEPROM.o.d 
	@${RM} ${OBJECTDIR}/EEPROM.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  EEPROM.c  -o ${OBJECTDIR}/EEPROM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/EEPROM.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/PIC24F/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/generic/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/MasterComm.o: MasterComm.c  .generated_files/flags/default/359f982753a59e733b422b7c84999726a364e90f .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/MasterComm.o.d 
	@${RM} ${OBJECTDIR}/MasterComm.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  MasterComm.c  -o ${OBJECTDIR}/MasterComm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/MasterComm.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/PIC24F/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/generic/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/LEDControl.o: LEDControl.c  .generated_files/flags/default/400cc2752135050f1641115bb269fa8e24b4716f .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/LEDControl.o.d 
	@${RM} ${OBJECTDIR}/LEDControl.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  LEDControl.c  -o ${OBJECTDIR}/LEDControl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/LEDControl.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/PIC24F/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/generic/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/ResetReport.o: ResetReport.c  .generated_files/flags/default/c78fbf7ab72306e8bd6ed424cc552c70445cce36 .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ResetReport.o.d 
	@${RM} ${OBJECTDIR}/ResetReport.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ResetReport.c  -o ${OBJECTDIR}/ResetReport.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/ResetReport.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/PIC24F/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/generic/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ConfigPragma.o: _ConfigPragma.c  .generated_files/flags/default/152d5deb258ea347d7f77e123b47148adc6489c7 .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/_ConfigPragma.o.d 
	@${RM} ${OBJECTDIR}/_ConfigPragma.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  _ConfigPragma.c  -o ${OBJECTDIR}/_ConfigPragma.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ConfigPragma.o.d"        -g -omf=elf -DXPRJ_default=$(CND_CONF)  -no-legacy-libc  $(COMPARISON_BUILD)  -std=gnu99 -O0 -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/PIC24F/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/support/generic/h" -I"C:/PROGRA~2/Microchip/xc16/v1.40/include" -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/RTCC.o: RTCC.c  .generated_files/flags/default/9183ec83ac507462ab23982af8d4ffbf64557d1d .generated_files/flags/default/134520b63590fa964063cd5424fbfd9de18f6d5
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
