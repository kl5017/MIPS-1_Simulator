#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <fstream>
#include <sys/stat.h>

#include "instr_def.hpp"
#include "instructions.hpp"


/*Design decision
running on ubuntu so
in case of error, exit without deleting heap ~1GB
OS cleans memory after process terminated
*/

//Memory map - Offsets
	//0x00000000 | 		  0x4  | ADDR_NULL
//0x00000004 |   0xFFFFFFC | EMPTY
	//0x10000000 |  0x1000000  | ADDR_INSTR
//0x11000000 |  0xF000000  | EMPTY
	//0x20000000 |  0x4000000  | ADDR_DATA
//0x24000000 |  0xC000000  | EMPTY
	//0x30000000 |        0x4  | ADDR_GETC 
	//0x30000004 |        0x4  | ADDR_PUTC
//0x30000008 |  0xCFFFFFF8 | EMPTY

struct state_t{

	uint8_t* RAM;
	uint32_t* reg;
	uint32_t PC;
	uint32_t PC_MAX;

	bool jump;
	bool Jdelayed;
	uint32_t PC_j;

	bool branch;
	bool Bdelayed;
	uint16_t bOffset;
	uint32_t PC_b;

	bool overflow;

	uint32_t HI;
	uint32_t LO;
	

};


void simulate(state_t* state);
void exe_instr(state_t* state, instrC* curr_decode);
void advance_PC(state_t* state);

uint32_t signExtend8(uint8_t x);
bool getSign18(uint32_t offset);	// true if b'18 is 1
bool getSign16(uint16_t offset);	// true if b'16 is 1
uint32_t get_effA(uint32_t rs, uint16_t offset);	// for load store address calc

// use to get word from 4 bytes
uint32_t get_word(uint8_t B0, uint8_t B1, uint8_t B2, uint8_t B3);
	// B0 is MSB of word - big endian

// sim I/O
uint32_t GET_C();
void PUT_C(uint8_t ADDR_PUTC);



int main(int argc, char *argv[]){

	state_t initial_state;

// get initial state
	uint8_t* ADDR_DATA = new uint8_t[0x30000008]();	// RAM
	// all elements init to 0 on declaration

//--REGISTERS-------------------------------------------------------------------
	// all registers will be zero at startup
	uint32_t* reg = new uint32_t[32]();
	
//------------------------------------------------------------------------------

//--PROGRAM COUNTER-------------------------------------------------------------
	initial_state.PC = 0x10000000;	// increment by +1 - word stored
						// PC_MAX - 0x10000000 = amount of instrucitons in program
						// PC theoritical max = 0x11000000

//------------------------------------------------------------------------------


//--PROGRAM MEMORY--------------------------------------------------------------
	// Binary passed in as command line parameter
	std::ifstream binary (argv[1], std::ios::in|std::ios::binary);
	if(!binary.is_open()){
		std::cerr << "could not open binary" << std::endl;
		delete[] ADDR_DATA;
		exit(EXIT_FAILURE);
	}
	//get file size
	long long bfile_size = 0;
	struct stat results;
		if( stat(argv[1], &results) ==0 ){
			bfile_size = results.st_size;
		}
		else{
			std::cerr << "could not get binary file size" << std::endl;
			delete[] ADDR_DATA;
			exit(EXIT_FAILURE);
		}

	binary.seekg(0);
	// read in from binary into RAM -ROM section
	uint32_t PC_MAX = 0x10000000; // counter for ROM 
	for(int i =0; i < bfile_size; i += 1){	
		char tmp_byte[1];
		binary.read (tmp_byte, 1);	// read in 1 byte into tmp_word

		// convert char to unint8_t
		uint8_t bin_byte = tmp_byte[0];

		// load into program ROM
		if(PC_MAX > 0x11000000){
			std::cerr << "ROM limit reached" << std::endl;
			exit(-11);					// too many instructions
		}
		else{
			ADDR_DATA[PC_MAX] = bin_byte;
			PC_MAX ++;
		}		
	}
	
	// PC_MAX now set 
	// PC at 0x10000004

	/* Testing for binary read in
	for(uint32_t i=0x10000000; i < PC_MAX; i++){
		std::cout <<  std::hex << ADDR_DATA[i];
	} */
//------------------------------------------------------------------------------
	
	initial_state.RAM = ADDR_DATA;		// pointer
	initial_state.reg = reg;			// pointer
	// initial_state.PC already set
	initial_state.PC_MAX = PC_MAX;					// obtained from file read in

	state_t* curr_state = NULL;

	curr_state = &initial_state;		// use this pointer to pass state of CPU around

//==RUN PROGRAM=================================================================
//Internal error (-20) : the simulator has failed due to some unknown error
//std::exit(-20);
// this occurs only during instruction execution
// if error occurs before or after during sim running, system_error not catched
	try{
		simulate(curr_state);
	}
	catch (const std::system_error& e) {
    	std::cerr << e.what() << '\n';
    	std::cerr << e.code() << '\n';
		std::exit(-20);
	}
//------------------------------------------------------------------------------

	delete[] ADDR_DATA;

//--CHECK FOR SPECIAL EXIT------------------------------------------------------

	uint8_t $two = curr_state->reg[2] & 0xFF;
	// shift prog_mem[2] low 8 into $two
	std::exit($two);
	

}


// SIMULATE ROM instructions
void simulate(state_t* state){
// control path for sim
	bool run = true;			// becomes false when PC_MAX reached
	uint32_t curr_word = 0;			// current instruction
	instrC* decodeP = NULL;

	state->jump = false;	// init to jump not set
	state->branch = false;
	state->Jdelayed = false;
	state->Bdelayed = false;
	state->overflow = false;
	state->HI = 0;
	state->LO = 0;

	while(run){
	// check instruction is from valid mem location (-11)
		if(! (state->PC >= 0x10000000 && state->PC <= 0x11000000) ){
			std::cerr << "PC out of range: " << std::hex << state->PC << std::endl;
			std::exit(-11);	//mem exception	
		}
	// fetch - big endian - PC points to MSB
		curr_word = get_word(state->RAM[state->PC], state->RAM[state->PC+1], state->RAM[state->PC+2], state->RAM[state->PC+3]);
	
	//decode
		//pass in curr_word
		// control signals returned - insdie struct - refer to instr_def.hpp
		instrC curr_decode = decode_instr(curr_word);
		decodeP = &curr_decode;
		

	//run
		exe_instr(state, decodeP);
		//make sure reg 0 stays 0
		state->reg[0] = 0;
		
		// check if reached end of program
		
	//increment PC and branch detection
		advance_PC(state);

		if(state->PC == 0){
				run = false;		
			}

	//end
	} // end while loop

}



void exe_instr(state_t* state, instrC* curr_decode){
// returns 0 on no errors
// Jumps and branches change state->PC_j and state->bOffset, state->PC_b

	// linear search for correct instruction to call 
	switch(curr_decode->instr_code) {
		case 69:
			{
				std::exit(-12); //invalid instruction
			}
		case 0:		//ADDU
			{	
				state->reg[curr_decode->RD] = addU(state->reg[curr_decode->RS], state->reg[curr_decode->RT]);
			}	break;
		case 1:		//AND
			{
				state->reg[curr_decode->RD] = mAnd(state->reg[curr_decode->RS], state->reg[curr_decode->RT]);
			}	break;
		case 2:		// JR
			{	
				jr(state->reg[curr_decode->RS], state->jump );
				if(state->jump){		// jump set
					state->PC_j = state->reg[curr_decode->RS];
				}
				else{
					std::cerr << "JR address invalid" << std::endl;				
					std::exit(-11);	
				}	// memory exception
			}	break;
		case 3:		// OR
			{
				state->reg[curr_decode->RD] = mOr(state->reg[curr_decode->RS], state->reg[curr_decode->RT]);
			}	break;
		case 4:		// SLTU
			{
				state->reg[curr_decode->RD] = sltU(state->reg[curr_decode->RS], state->reg[curr_decode->RT]);
			}	break;
		case 5:		// SUBU
			{
				state->reg[curr_decode->RD] = subU(state->reg[curr_decode->RS], state->reg[curr_decode->RT]);
			}	break;
		case 6:		// XOR
			{
				state->reg[curr_decode->RD] = mXor(state->reg[curr_decode->RS], state->reg[curr_decode->RT]);
			}	break;
		case 7:		// ADD
			{
				uint32_t temp = state->reg[curr_decode->RD];
				state->reg[curr_decode->RD] = add(state->reg[curr_decode->RS], state->reg[curr_decode->RT], state->overflow);
				if(state->overflow){
					state->reg[curr_decode->RD] = temp;
					std::exit(-10);
				}
				
			}	break;
		case 8:		//ADDI
			{
				uint32_t temp = state->reg[curr_decode->RT];
				state->reg[curr_decode->RT] = addI(state->reg[curr_decode->RS], curr_decode->imd_const, state->overflow);
				if(state->overflow){
					state->reg[curr_decode->RT] = temp;
					std::exit(-10);
				}
			}	break;
		case 9:		//ADDIU
			{
				state->reg[curr_decode->RT] = addIU(state->reg[curr_decode->RS], curr_decode->imd_const);
			}	break;
		case 10:	//ANDI
			{
				state->reg[curr_decode->RT] = mAndI(state->reg[curr_decode->RS], curr_decode->imd_const);
			}	break;
		case 11:	//LUI
			{
				state->reg[curr_decode->RT] = lui(curr_decode->imd_const);
			}	break;
		case 12:	//LW
			{

				// effA = effective address
				uint32_t effA = get_effA(state->reg[curr_decode->RS], curr_decode->imd_const);
				// check allignment
				if( (effA & 0x2) != 0 ){	// 2 lsb != 0
					std::cerr << "LW address invalid" << std::endl;
					std::exit(-11);				// mem exception
				} 
				// check if ROM, RAM
				if( (effA >= 0x10000000 && effA < 0x11000000) || (effA >= 0x20000000 && effA < 0x24000000) ){ 
					// effA points to MSB
					uint32_t tempW;
					tempW = (state->RAM[effA] << 24) & 0xFF000000;	// get MSB
					tempW += (state->RAM[effA+1] << 16) & 0x00FF0000;
					tempW += (state->RAM[effA+2] << 8) & 0x0000FF00;
					tempW += state->RAM[effA+3] & 0x000000FF; // get LSB
					state->reg[curr_decode->RT] = tempW;
				}
				else if(effA == 0x30000000){
					state->reg[curr_decode->RT] = GET_C();	// get char from std in
				} 
				else{
					std::cerr << "LW address invalid" << std::endl;	
					std::exit(-11);	// address excepetion
				}

			}	break;
		case 13:	//ORI
			{
				state->reg[curr_decode->RT] = mOrI(state->reg[curr_decode->RS], curr_decode->imd_const);
			}	break;
		case 14:	//SLL
			{
				state->reg[curr_decode->RD] = sll(state->reg[curr_decode->RT], curr_decode->shift);
			}	break;
		case 15:	//SLT
			{
				state->reg[curr_decode->RD] = slt(state->reg[curr_decode->RS], state->reg[curr_decode->RT]);
			}	break;
		case 16:	//SRA
			{
				state->reg[curr_decode->RD] = sra(state->reg[curr_decode->RT], curr_decode->shift);
			}	break;	
		case 17:	//SRAV
			{
				state->reg[curr_decode->RD] = srav(state->reg[curr_decode->RS], state->reg[curr_decode->RT] );
			}	break;	
		case 18:	//SRL
			{
				state->reg[curr_decode->RD] = srl(state->reg[curr_decode->RT], curr_decode->shift);
			}	break;
		case 19:	//SUB
			{
				uint32_t temp = state->reg[curr_decode->RD];
				state->reg[curr_decode->RD] = sub(state->reg[curr_decode->RS], state->reg[curr_decode->RT], state->overflow);
				if(state->overflow){
					state->reg[curr_decode->RD] = temp;
					std::exit(-10);
				}
			}	break;
		case 20:	//SW
			{

			// effA = effective address
				uint32_t effA = get_effA(state->reg[curr_decode->RS], curr_decode->imd_const);
				// check allignment
				if( (effA & 0x2) != 0 ){	// 2 lsb != 0
					std::cerr << "SW address invalid" << std::endl;
					std::exit(-11);				// mem exception
				} 
				// check if RAM
				if( (effA >= 0x20000000 && effA < 0x24000000) ){ 
					// effA points to MSB
					state->RAM[effA]   =  ( state->reg[curr_decode->RT] >> 24) & 0xFF;	// put MSB
					state->RAM[effA+1] =  ( state->reg[curr_decode->RT] >> 16) & 0xFF;
					state->RAM[effA+2] =  ( state->reg[curr_decode->RT] >> 8) & 0xFF;
					state->RAM[effA+3] =  ( state->reg[curr_decode->RT] ) & 0xFF;		// put LSB
				}
				else if(effA == 0x30000004){
					uint8_t putB = state->reg[curr_decode->RT] & 0xFF;	//LSB
					PUT_C(putB);	// write LSB of RT to std out
				} 
				else{	
					std::cerr << "SW address invalid:" << std::hex << effA << std::endl;
					std::exit(-11);	// address excepetion
				}

			}	break;
		case 21:	//XORI
			{
				state->reg[curr_decode->RT] = mXorI(state->reg[curr_decode->RS], curr_decode->imd_const);
			}	break;
		case 22:	//BEQ
			{
				// set bool branch
				state->branch = beq(state->reg[curr_decode->RS], state->reg[curr_decode->RT]);
				if(state->branch){
					state->bOffset = curr_decode->imd_const;
				}	// bOffset set
			}	break;
		case 23:	//BGEZ
			{
				// set bool branch
				state->branch = bgez(state->reg[curr_decode->RS]);
				if(state->branch){
					state->bOffset = curr_decode->imd_const;
				}	// bOffset set
			}	break;
		case 24:	//BLTZ
			{
				// set bool branch
				state->branch = bltz(state->reg[curr_decode->RS]);
				if(state->branch){
					state->bOffset = curr_decode->imd_const;
				}	// bOffset set
			}	break;
		case 25:	//BGTZ
			{
				// set bool branch
				state->branch = bgtz(state->reg[curr_decode->RS]);
				if(state->branch){
					state->bOffset = curr_decode->imd_const;
				}	// bOffset set
			}	break;
		case 26:	//BLEZ
			{
				// set bool branch
				state->branch = blez(state->reg[curr_decode->RS]);
				if(state->branch){
					state->bOffset = curr_decode->imd_const;
				}	// bOffset set
			}	break;
		case 27:	//BNE
			{
				// set bool branch
				state->branch = bne(state->reg[curr_decode->RS], state->reg[curr_decode->RT]);
				if(state->branch){
					state->bOffset = curr_decode->imd_const;
				}	// bOffset set
			}	break;
		case 28:	//J
			{
				state->jump = true;
				uint32_t PCtmp = (state->PC + 4) & 0xF0000000;
				PCtmp += ( ( curr_decode->j_memory << 2 ) & 0x0FFFFFFF );
				state->PC_j = PCtmp;
			}	break;	
		case 29:	//LB
			{
				
				// effA = effective address
				uint32_t effA = get_effA(state->reg[curr_decode->RS], curr_decode->imd_const);
				// check if ROM, RAM
				if( (effA >= 0x10000000 && effA < 0x11000000) || (effA >= 0x20000000 && effA < 0x24000000) ){ 
					// effA points to byte
					uint32_t tempR = signExtend8(state->RAM[effA]);	// sign extend to 32b
					state->reg[curr_decode->RT] = tempR;
				}
				else if(effA == 0x30000000){
					uint8_t tempB = 0xFF & GET_C(); // GET_C returns 32b
					uint32_t tempR = signExtend8(tempB);
					state->reg[curr_decode->RT] = tempR;	// get char from std in
				} 
				else{	
					std::cerr << "LB address invalid" << std::endl;
					std::exit(-11);	// address excepetion
				}

			}	break;
		case 30:	//LBU
			{
				
				// effA = effective address
				uint32_t effA = get_effA(state->reg[curr_decode->RS], curr_decode->imd_const);
				// check if ROM, RAM
				if( (effA >= 0x10000000 && effA < 0x11000000) || (effA >= 0x20000000 && effA < 0x24000000) ){ 
					// effA points to byte
					uint32_t tempR = 0x000000FF & state->RAM[effA];	// sign extend to 32b
					state->reg[curr_decode->RT] = tempR;
				}
				else if(effA == 0x30000000){
					uint32_t tempR = 0x000000FF & GET_C(); // GET_C returns 32b
					state->reg[curr_decode->RT] = tempR;	// get char from std in
				} 
				else{	
					std::cerr << "LBU address invalid" << std::endl;
					std::exit(-11);	// address excepetion
				}
			
			}	break;
		case 31:	//LH
			{

				// effA = effective address
				uint32_t effA = get_effA(state->reg[curr_decode->RS], curr_decode->imd_const);
				// check allignment
				if( (effA & 0x1) != 0 ){	// lsb != 0
					std::cerr << "LH address invalid" << std::endl;
					std::exit(-11);				// mem exception
				} 
				// check if ROM, RAM
				if( (effA >= 0x10000000 && effA < 0x11000000) || (effA >= 0x20000000 && effA < 0x24000000) ){ 
					// effA points to MSB
					uint16_t tempHW;
					tempHW = (state->RAM[effA] << 8) & 0xFF00;	// first byte, higher
					tempHW += state->RAM[effA+1] & 0x00FF;	// lower byte
					uint32_t tempR = signExtend16(tempHW);	// sign extend
					state->reg[curr_decode->RT] = tempR;
				}
				else if(effA == 0x30000000){
					state->reg[curr_decode->RT] = GET_C();	// get char from std in
					// GET_C adheres to sign extend behaviour of LH
				} 
				else{
					std::cerr << "LH address invalid" << std::endl;	
					std::exit(-11);	// address excepetion
				}		
						
			}	break;
		case 32:	//LHU	
			{

				// effA = effective address
				uint32_t effA = get_effA(state->reg[curr_decode->RS], curr_decode->imd_const);
				// check allignment
				if( (effA & 0x1) != 0 ){	// lsb != 0
					std::cerr << "LHU address invalid" << std::endl;
					std::exit(-11);				// mem exception
				} 
				// check if ROM, RAM
				if( (effA >= 0x10000000 && effA < 0x11000000) || (effA >= 0x20000000 && effA < 0x24000000) ){ 
					// effA points to MSB
					uint32_t tempR;
					tempR = (state->RAM[effA] << 8) & 0x0000FF00;	// first byte, higher
					tempR += state->RAM[effA+1] & 0x000000FF;	// lower byte, // zero extended
					state->reg[curr_decode->RT] = tempR;
				}
				else if(effA == 0x30000000){
					uint32_t tempR = 0x0000FFFF & GET_C();	// zero extend
					state->reg[curr_decode->RT] = tempR;	// get char from std in
				} 
				else{	
					std::cerr << "LHU address invalid" << std::endl;
					std::exit(-11);	// address excepetion
				}
	
			}	break;
		case 33:	//MFHI
			{
				state->reg[curr_decode->RD] = state->HI;
			}	break;	
		case 34:	//MFLO
			{
				state->reg[curr_decode->RD] = state->LO;
			}	break;	
		case 35:
			{
				state->HI = state->reg[curr_decode->RS];
			}	break;	//MTHI
		case 36:	//MTLO
			{
				state->LO = state->reg[curr_decode->RS];
			}	break;	
		case 37:	//SB
			{

				// effA = effective address
				uint32_t effA = get_effA(state->reg[curr_decode->RS], curr_decode->imd_const);
				// check if RAM
				if( (effA >= 0x20000000 && effA < 0x24000000) ){ 
					// effA points to byte to be written to
					uint8_t tempB = 0xFF & state->reg[curr_decode->RT];
					state->RAM[effA] = tempB;
				}
				else if(effA == 0x30000007){
					uint8_t tempB = 0xFF & state->reg[curr_decode->RT];	// LSB
					PUT_C(tempB);	
				} 
				else{	
					std::cerr << "SB address invalid" << std::endl;
					std::exit(-11);	// address excepetion
				}

			}	break;
		case 38:	//SH
			{

				// effA = effective address
				uint32_t effA = get_effA(state->reg[curr_decode->RS], curr_decode->imd_const);
				// check allignment
				if( (effA & 0x1) != 0 ){	// lsb != 0
					std::cerr << "SH address invalid" << std::endl;
					std::exit(-11);				// mem exception
				} 
				// check if RAM
				if( (effA >= 0x20000000 && effA < 0x24000000) ){ 
					// effA points to byte to be written to
					uint16_t tempHW = 0xFFFF & state->reg[curr_decode->RT];
					uint8_t tempUB = (tempHW >> 8) & 0xFF;
					uint8_t tempLB = tempHW & 0xFF;
					state->RAM[effA] = tempUB;
					state->RAM[effA+1] = tempLB;
				}
				else if(effA == 0x30000006){
					uint8_t tempB = 0xFF & state->reg[curr_decode->RT];	// LSB
					PUT_C(tempB);	
				} 
				else{	
					std::cerr << "SH address invalid" << std::endl;
					std::exit(-11);	// address excepetion
				}

			}	break;
		case 39:	//SLLV
			{
				state->reg[curr_decode->RD] = sllv(state->reg[curr_decode->RS], state->reg[curr_decode->RT]);
			}	break;	
		case 40:	//SLTI
			{
				state->reg[curr_decode->RT] = sltI(state->reg[curr_decode->RS], curr_decode->imd_const);
			}	break;
		case 41:	//SLTIU
			{
				state->reg[curr_decode->RT] = sltIU(state->reg[curr_decode->RS], curr_decode->imd_const);
			}	break;
		case 42:	//SRLV
			{
				state->reg[curr_decode->RD] = srlv(state->reg[curr_decode->RS], state->reg[curr_decode->RT]);
			}	break;	
		case 43:	//BGEZAL
			{
				state->branch = bgezal(state->reg[curr_decode->RS]);
				if(state->branch){
					//link
					state->reg[31] = state->PC + 8;
					//set bOffset
					state->bOffset = curr_decode->imd_const;
				}
			}	break;	
		case 44:	//BLTZAL
			{
				state->branch = bltzal(state->reg[curr_decode->RS]);
				if(state->branch){
					//link
					state->reg[31] = state->PC + 8;

					state->bOffset = curr_decode->imd_const;
				}
			}	break;	
		case 45:	//DIV
			{
				double_uint32 tmp;
				tmp = div(state->reg[curr_decode->RS], state->reg[curr_decode->RT]);
				state->HI = tmp.hi;
				state->LO = tmp.lo;
			}	break;	
		case 46:	//DIVU
			{
				double_uint32 tmp;
				tmp = divU(state->reg[curr_decode->RS], state->reg[curr_decode->RT]);
				state->HI = tmp.hi;
				state->LO = tmp.lo;
			}	break;	
		case 47:	//JALR
			{
				//code for JR, if valid Jump, contents differ
				jr(state->reg[curr_decode->RS], state->jump );
				if(state->jump){		// jump set
					state->reg[curr_decode->RD] = state->PC + 8;	// return link first to RD
				
					state->PC_j = state->reg[curr_decode->RS];	// set PC_j
				}
				else{
					std::cerr << "JR address invalid" << std::endl;				
					std::exit(-11);	
				}	// memory exception

			}	break;	
		case 48:	//JAL
			{
				state->jump = true;
				//link
				state->reg[31] = state->PC + 8;
				//set jump addr
				uint32_t PCtmp = (state->PC + 4) & 0xF0000000;
				PCtmp += ( ( curr_decode->j_memory << 2 ) & 0x0FFFFFFF );
				state->PC_j = PCtmp;
			}	break;	
		case 49:	//MULT
			{
				double_uint32 tmp;
				tmp = mult(state->reg[curr_decode->RS], state->reg[curr_decode->RT]);
				state->HI = tmp.hi;
				state->LO = tmp.lo;
			}	break;	
		case 50:	//MULTU
			{
				double_uint32 tmp;
				tmp = multU(state->reg[curr_decode->RS], state->reg[curr_decode->RT]);
				state->HI = tmp.hi;
				state->LO = tmp.lo;
			}	break;
		case 51:	//LWL
			{
				// get effective address
				uint32_t effA = get_effA(state->reg[curr_decode->RS], curr_decode->imd_const);

				uint32_t pos = effA & 0x3; 	// gets byte num

				// check if effA in ROM, RAM
				if( (effA-pos >= 0x10000000 && effA+pos < 0x11000000) || (effA-pos >= 0x20000000 && effA+pos < 0x24000000) ){ 
					//if pos = 0 : load next 4 bytes in RT		//pos point MSB W
					//if pos = 1 : load next 3 into 3MSB of RT	//pos point 3LSB W
					//if pos = 2 : load next 2 into 2MSB of RT	//pos point 2LSB W
					//if pos = 3 : load next B into MSB of RT	
					uint32_t tempR = state->reg[curr_decode->RT];

					if(pos == 0){
						tempR = (state->RAM[effA] << 24) & 0xFF000000;	// get MSB
						tempR += (state->RAM[effA+1] << 16) & 0x00FF0000;
						tempR += (state->RAM[effA+2] << 8) & 0x0000FF00;
						tempR += state->RAM[effA+3] & 0x000000FF; // get LSB
					}
					else if(pos == 1){
						tempR = (state->RAM[effA] << 24) & 0xFF000000;	
						tempR += (state->RAM[effA+1] << 16) & 0x00FF0000;
						tempR += (state->RAM[effA+2] << 8) & 0x0000FF00;	
					}
					else if(pos == 2){
						tempR = (state->RAM[effA] << 24) & 0xFF000000;	
						tempR += (state->RAM[effA+1] << 16) & 0x00FF0000;
					}
					else if(pos == 3){
						tempR = (state->RAM[effA] << 24) & 0xFF000000;
					}
		
					state->reg[curr_decode->RT] = tempR;

				}
				else if(effA == 0x30000000){		// aligned
					state->reg[curr_decode->RT] = GET_C();	// get char from std in

				} 
				else{
					std::cerr << "LWL address invalid" << std::endl;	
					std::exit(-11);	// address excepetion
				}

			}	break;	
		case 52:	//LWR
			{
				// get effective address
				uint32_t effA = get_effA(state->reg[curr_decode->RS], curr_decode->imd_const);

				uint32_t pos = effA & 0x3; 	// gets byte num

				// check if effA in ROM, RAM or GETC
				if( (effA-pos >= 0x10000000 && effA+pos < 0x11000000) || (effA-pos >= 0x20000000 && effA+pos < 0x24000000) ){ 
						
					uint32_t tempR = state->reg[curr_decode->RT];

					if(pos == 0){
						tempR = state->RAM[effA] & 0x000000FF; // get MSB into LSB
					}
					else if(pos == 1){	
						tempR = state->RAM[effA] & 0x00FF0000;
						tempR += (state->RAM[effA-1] << 8) & 0x0000FF00;	
					}
					else if(pos == 2){
						tempR = state->RAM[effA] & 0x00FF0000;
						tempR += (state->RAM[effA-1] << 8) & 0x0000FF00;
						tempR += (state->RAM[effA-2] << 16) & 0x00FF0000;
					}
					else if(pos == 0){
						tempR = state->RAM[effA] & 0x00FF0000;
						tempR += (state->RAM[effA-1] << 8) & 0x0000FF00;
						tempR += (state->RAM[effA-2] << 16) & 0x00FF0000;
						tempR += (state->RAM[effA-3] << 24) & 0xFF000000;
					}
		
					state->reg[curr_decode->RT] = tempR;

				}
				else if(effA == 0x30000000){		// aligned
					state->reg[curr_decode->RT] = GET_C();	// get char from std in

				} 
				else{
					std::cerr << "LWR address invalid" << std::endl;	
					std::exit(-11);	// address excepetion
				}

			}	break;	

		default: 	
			std::exit(-20);		//oh no, whats gone wrong...
	}

}




void advance_PC(state_t* state){
		// check if jump;
		if(state->jump){	// curr instr is a jump
			state->Jdelayed = true;
			state->jump = false;
			state->PC += 4;		// run delayed instr
		}
		else if(state->Jdelayed){
			state->PC = state->PC_j;		// take jump after one cylce delay
			state->Jdelayed = false;
		}
		// check if branch
		else if(state->branch){		// curr instr is a branch
			state->Bdelayed = true;
			state->branch = false;
			state->PC += 4;		// run delayed instruction

			uint32_t extended = signExtend16(state->bOffset);
			state->PC_b = state->PC + (extended << 2);

		}
		else if(state->Bdelayed){	//ran delayed instr
			state->Bdelayed = false;
			state->PC = state->PC_b;	//take branch now
		}
		else{		// normal PC inrement
			state->PC +=4;
		}
		
// PC location checked at top of simulate, so advance_PC does not need to check if valid location
}


uint32_t GET_C(){
// get char form stdin
//return -1 if EOF
	int x;

	// catch IO error?
	try{
		x = std::getchar();
	}
	catch (std::ios_base::failure){
		std::cerr << "getchar fail" << std::endl;
		std::exit(-21); // IO error -21
	}

	if(feof(stdin)){
		return 0xFFFFFFFF; // return -1
	}
	else{
		uint32_t X = x & 0x000000FF;	// remove sign in C
		return X;
	}
}

void PUT_C(uint8_t ADDR_PUTC){
//writes LSB of ADDR_PUTC to stdout

	int c = ADDR_PUTC;

	// catch IO error?
	try{
		std::putchar(c);
	}
	catch (std::ios_base::failure){
		std::cerr << "putchar fail" << std::endl;
		std::exit(-21); // IO error -21
	}

}



uint32_t get_word(uint8_t B0, uint8_t B1, uint8_t B2, uint8_t B3){
	// B0 is MSB of word
	uint32_t word = 0;
	return word = (B0 << 24) | (B1 << 16) | (B2 << 8) | B3;
}


uint32_t get_effA(uint32_t rs, uint16_t offset){
// returns effective address
	uint32_t eff = rs;							// base address
	uint32_t extended = signExtend16(offset);

	return eff + extended;
}

uint32_t signExtend8(uint8_t x){
	uint8_t sign = x >> 7;	// lsb is sign bit
    uint32_t value = 0x000000FF & x;

    if (sign == 1){		// neg
	    return value += 0xFFFFFF00;
    }
	else{
		return value;
	}
}









