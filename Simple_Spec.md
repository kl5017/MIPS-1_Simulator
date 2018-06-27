"make simulator" - Results in binary called - 'bin/mips_simulator'

Binary file compiled from C++ source - so if following is executed;
"rm bin/mips_simulator
make simulator"
- anew binary will be compiled and included in the submission

-DONE

On start, ALL MIPS REGISTERS will be ZERO,  PC will point at FIRST INSTRUCTION in MEMORY
-DONE

UNINITIALIZED MEMORY will be ZERO, - what data structure to use?
Dont assume simulator is being executed from a certain directory. Shouldnt try open/create/write to any data files.

Testbench takes a single command line parameter - path of simulator to be tested.
Testbench output - Print CSV file; each row corresponds to exactly one execution of the simulator under test.
Each row should contain;
1.TestId- Unique ID of a certain test using ||0-9||a-z||A-Z||-||_|| - Running multiple tests shouldnt alter the order of testIds
2.Intrsuction- Identify primary instruction under test. Test should be particular to the ins under scrutiny
3.Status- Pass/Fail. Fail should only be returned if ins under test or another ins has CLEARY gone wrong.
4.Author- Login of the person who created the test
5.Message- Optional field- Gives more details of what when wrong. Single line, no commas, free form text

Testbench built using - "make testbench"
-Only needs to be an exe file, so doesnt have to be binary build from C++(could be bash script)

Temporary or working files created during execution should be in a directoroy called "test/temp"
Output files of the testbench should be created in "test/output"

*The Binary is not allowed to modify it's own code, or attempt to execute code outside the executable memory.

When a simulated program reads from address ADDR_GETC, the simulator should
-Block until a character is available (e.g. if a key needs to be pressed)
-Return the 8-bit extended to 32-bits as the result of the memory read.
-If there are no more characters (EOF), the memory read should return -1.
-When a simulated program writes to address ADDR_PUTC, the simulator should write the character to stdout. If the write fails, the appropriate Error should be signalled.

Exception Codes
These exceptions should not occur due to bugs within the simulator
-Arithmetic Exception(-10): Any Arithmetic problem;overflow, /zero etc
-Memory Exception(-11): Any memory problem: Address out of range, writing to read only memory, reading from an address that cannot be read, executing an address that cannot be executed etc
-Invalid Instruction(-12): Binary tries to execute a memory location that doesnt contain a valid instruction.

When any of these errors are encountered, the simulator should terminate immediately with the exit code using std::exit.

Error Codes
-Internal Error(-20)- The simulator has failed due to some unknown error.
-IO Error(-21)- the simulator encountered a read/write||input/output error.


Memory map - Offsets
0x20000000 |  0x4000000  | ADDR_DATA
0x30000000 |        0x4  | ADDR_GETC 
0x30000004 |        0x4  | ADDR_PUTC


Instructions of Interest:
Code	Meaning	Complexity
ADD	Add (with overflow)	2 XX
ADDI	Add immediate (with overflow)	2 XX
ADDIU	Add immediate unsigned (no overflow)	2 XX
ADDU	Add unsigned (no overflow)	1 X
AND	Bitwise and	1 X
ANDI	Bitwise and immediate	2 XX
BEQ	Branch on equal	3 XXX
BGEZ	Branch on greater than or equal to zero	3 XXX
BGEZAL	Branch on non-negative (>=0) and link	4 XXXX
BGTZ	Branch on greater than zero	3 XXX
BLEZ	Branch on less than or equal to zero	3 XXX
BLTZ	Branch on less than zero	3 XXX
BLTZAL	Branch on less than zero and link	4 XXXX
BNE	Branch on not equal	3 XXX
DIV	Divide	4 XXXX
DIVU	Divide unsigned	4 XXXX
J	Jump	3 XXX
JALR	Jump and link register	4 XXXX
JAL	Jump and link	4 XXXX
JR	Jump register	1 X
LB	Load byte	3 XXX
LBU	Load byte unsigned	3 XXX
LH	Load half-word	3 XXX
LHU	Load half-word unsigned	3 XXX
LUI	Load upper immediate	2 XX
LW	Load word	2 XX
LWL	Load word left	5 XXXXX
LWR	Load word right	5 XXXXX
MFHI	Move from HI	3 XXX
MFLO	Move from LO	3 XXX
MTHI	Move to HI	3 XXX
MTLO	Move to LO	3 XXX
MULT	Multiply	4 XXXX
MULTU	Multiply unsigned	4 XXXX
OR	Bitwise or	1 X
ORI	Bitwise or immediate	2 XX
SB	Store byte	3 XXX
SH	Store half-word	3 XXX
SLL	Shift left logical	2 XX
SLLV	Shift left logical variable	3 XXX
SLT	Set on less than (signed)	2 XX
SLTI	Set on less than immediate (signed)	3 XXX
SLTIU	Set on less than immediate unsigned	3 XXX
SLTU	Set on less than unsigned	1 X
SRA	Shift right arithmetic	2 XX
SRAV	Shift right arithmetic	2 XX
SRL	Shift right logical	2 XX
SRLV	Shift right logical variable	3 XXX
SUB	Subtract	2 XX
SUBU	Subtract unsigned	1 X
SW	Store word	2 XX
XOR	Bitwise exclusive or	1 X
XORI	Bitwise exclusive or immediate	2 XX
