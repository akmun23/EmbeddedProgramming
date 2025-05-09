################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1260/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/aksel/OneDrive - Syddansk Universitet/RobTek/4semester/ImbeddedProgramming/EmbeddedProgramming/Final_Assignment" --include_path="C:/Users/aksel/OneDrive - Syddansk Universitet/RobTek/4semester/ImbeddedProgramming/EmbeddedProgramming/Final_Assignment/FRT_Files" --include_path="C:/ti/ccs1260/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/include" --include_path="C:/Users/aksel/OneDrive - Syddansk Universitet/RobTek/4semester/ImbeddedProgramming/EmbeddedProgramming/Final_Assignment/FRT_Files/inc" --include_path="C:/Users/aksel/OneDrive - Syddansk Universitet/RobTek/4semester/ImbeddedProgramming/EmbeddedProgramming/Final_Assignment/FRT_Files/TivaM4" --define=ccs="ccs" --define=PART_TM4C123GH6PM -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1260/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/aksel/OneDrive - Syddansk Universitet/RobTek/4semester/ImbeddedProgramming/EmbeddedProgramming/Final_Assignment" --include_path="C:/ti/ccs1260/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/include" --include_path="C:/Users/aksel/OneDrive - Syddansk Universitet/RobTek/4semester/ImbeddedProgramming/frt10/port/TivaM4" --include_path="C:/Users/aksel/OneDrive - Syddansk Universitet/RobTek/4semester/ImbeddedProgramming/frt10/inc" --define=ccs="ccs" --define=PART_TM4C123GH6PM -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


