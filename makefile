###################################################
## MIPS Compilation path

MIPS_CC = mips-linux-gnu-gcc
MIPS_OBJCOPY = mips-linux-gnu-objcopy

# Turn on all warnings, and enable optimisations
MIPS_CPPFLAGS = -W -Wall -O3 -fno-builtin

# Avoid standard libraries etc. being brought in, and link statically
MIPS_LDFLAGS = -nostdlib -Wl,-melf32btsmip -march=mips1 -nostartfiles -mno-check-zero-division -Wl,--gpsize=0 -static -Wl,-Bstatic
MIPS_LDFLAGS += -Wl,--build-id=none

# Compile a c file into a MIPS object file
%.mips.o : %.c
	$(MIPS_CC) $(MIPS_CPPFLAGS) -c $< -o $@

# Link a MIPS object file and place it at the locations required in the
# spec using linker.ld
%.mips.elf : %.mips.o
	$(MIPS_CC) $(MIPS_CPPFLAGS) $(MIPS_LDFLAGS) -T src/linker.ld $< -o $@

# Extract just the binary instructions from the object file
%.mips.bin : %.mips.elf
	$(MIPS_OBJCOPY) -O binary --only-section=.text $< $@

# For example, if you have testbench/test.c, you can do:
#
# make testbench/test.mips.bin



###################################################
## Simulator

# Build the simulation binary
bin/mips_simulator : src/simulator.cpp src/instr_decoder.cpp src/instructions.cpp
	g++ -c -std=c++11 -Wall src/instr_decoder.cpp
	g++ -c -std=c++11 -Wall src/simulator.cpp
	g++ -c -std=c++11 -Wall src/instructions.cpp
	g++ simulator.o instr_decoder.o instructions.o -o bin/mips_simulator

	rm instr_decoder.o
	rm simulator.o
	rm instructions.o


# In order to comply with spec
simulator : bin/mips_simulator



###################################################
## Testbench
bin/mips_testbench : testbench/mips_testbench

	#make mips_testbench executable and move to bin/
	cp testbench/mips_testbench bin
	chmod +x bin/mips_testbench


testbench : bin/mips_testbench
	

	

	



