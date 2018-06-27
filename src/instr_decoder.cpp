#include "instr_def.hpp"
#include <cstdint>


instrC decode_instr(const uint32_t& fourByte){
    instrC load_data;

    //string type;
    uint8_t op_code = ((fourByte>>26) & 0x3f);

    if (op_code == 0) {
//  type = r_type;
        load_data.instr_code = r_instr_finder(fourByte);
        r_extract(fourByte, load_data);
    }
// type = j_type;
    else if (op_code == 0x02) {             //J
        load_data.instr_code = 28;
        j_extract(fourByte, load_data);
    }

    else if (op_code == 0x03) {             //JAL
        load_data.instr_code = 48;
        j_extract(fourByte, load_data);
    }

    else {

//  type = i_type;
        uint8_t special_i = ((fourByte >> 16) & 0x1f);

        load_data.instr_code = i_instr_finder(fourByte, op_code, special_i);
        i_extract(fourByte, load_data);
    }

    return load_data;
}


uint8_t r_instr_finder(uint32_t fourByte){
    uint8_t func = ((fourByte) & 0x3f);

    switch(func){

        case 0x21:		//ADDU
			{
				uint8_t tmp = fourByte & 0x7C0;	    	// mask shift SHIFT REG
				if(tmp == 0){
					return 0;
				} else return 69;
			}

        case 0x24:      //AND
            {
				uint8_t tmp = fourByte & 0x7C0; 		// mask shift SHIFT REG
				if(tmp == 0){
					return 1;
				} else return 69;
            }

        case 0x8:		//JR
			{
				uint16_t tmp = fourByte & 0x1FFFC0;     // mask shift RT,RD & SHIFT REG
				if(tmp == 0){
					return 2;
				} else return 69;
			}

        case 0x25:      //OR
			{
		        uint8_t tmp = fourByte & 0x7C0;		    // mask shift SHIFT REG
					if(tmp == 0){
						return 3;
					} else return 69;
			}

        case 0x2B:      //SLTU
			{
		        uint8_t tmp = fourByte & 0x7C0;		    // mask shift SHIFT REG
					if(tmp == 0){
						return 4;
					} else return 69;
			}

        case 0x23:		//SUBU
			{
		        uint8_t tmp = fourByte & 0x7C0;		    // mask shift SHIFT REG
					if(tmp == 0){
						return 5;
					} else return 69;
			}

        case 0x26:      //XOR
			{
		        uint8_t tmp = fourByte & 0x7C0;		    // mask shift SHIFT REG
					if(tmp == 0){
						return 6;
					} else return 69;
			}

        case 0x20:      //ADD
			{
		        uint8_t tmp = fourByte & 0x7C0;		    // mask shift SHIFT REG
					if(tmp == 0){
						return 7;
					} else return 69;
			}

        case 0x0:       //SLL
            {
                uint8_t tmp = (fourByte & 0x03E00000);      // mask shift RS REG
                if(tmp == 0){
                    return 14;
                } else return 69;
            }

        case 0x2A:      //SLT
            {
		        uint8_t tmp = fourByte & 0x7C0;         // mask shift SHIFT REG
					if(tmp == 0){
						return 15;
					} else return 69;
			}

        case 0x3:       //SRA
            {
		        uint8_t tmp = (fourByte & 0x3E00000);   // mask shift RS REG
					if(tmp == 0){
						return 16;
					} else return 69;
			}

        case 0x7:       //SRAV
            {
		        uint8_t tmp = (fourByte & 0x7C0);	    // mask shift SHIFT REG
					if(tmp == 0){
						return 17;
					} else return 69;
			}

        case 0x2:       //SRL
            {
		        uint8_t tmp = (fourByte & 0x3E00000);   // mask shift RS REG
					if(tmp == 0){
						return 18;
					} else return 69;
			}

        case 0x22:      //SUB
            {
		        uint8_t tmp = (fourByte & 0x7C0);	    // mask shift SHIFT REG
					if(tmp == 0){
						return 19;
					} else return 69;
			}

        case 0x10:      //MFHI
            {
		        uint8_t tmp = (fourByte & 0x3FF07C0);   // mask shift RS, RT, SHIFT REG
					if(tmp == 0){
						return 33;
					} else return 69;
			}

        case 0x12:      //MFLO
            {
		        uint8_t tmp = (fourByte & 0x3FF07C0);   // mask shift RS, RT, SHIFT REG
					if(tmp == 0){
						return 34;
					} else return 69;
			}
            return 34;
        case 0x11:      //MTHI
            {
		        uint8_t tmp = (fourByte & 0x1FFFC0);    // mask shift RT, RD, SHIFT REG
					if(tmp == 0){
						return 35;
					} else return 69;
			}

        case 0x13:      //MTLO
            {
		        uint8_t tmp = (fourByte & 0x1FFFC0);    // mask shift RT, RD, SHIFT REG
					if(tmp == 0){
						return 36;
					} else return 69;
			}

        case 0x4:       //SLLV            
            {
		        uint8_t tmp = (fourByte & 0x7C0);	    // mask shift SHIFT REG
					if(tmp == 0){
						return 39;
					} else return 69;
			}

        case 0x6:       //SRLV
            {
		        uint8_t tmp = (fourByte & 0x7C0);	    // mask shift SHIFT REG
					if(tmp == 0){
						return 42;
					} else return 69;
			}

        case 0x1A:      //DIV
            {
		        uint8_t tmp = (fourByte & 0xFFC0);	    // mask shift RD, SHIFT REG
					if(tmp == 0){
						return 45;
					} else return 69;
			}       

        case 0x1B:      //DIVU
            {
		        uint8_t tmp = (fourByte & 0xFFC0);	    // mask shift RD, SHIFT REG
					if(tmp == 0){
						return 46;
					} else return 69;
			}       

        case 0x9:       //JALR
            {
		        uint8_t tmp = (fourByte & 0x1F07C0);    // mask shift RT, SHIFT REG
					if(tmp == 0){
						return 47;
					} else return 69;
			}        

        case 0x18:      //MULT
            {
		        uint8_t tmp = (fourByte & 0xFFC0);	    // mask shift RD, SHIFT REG
					if(tmp == 0){
						return 49;
					} else return 69;
			}       

        case 0x19:      //MULTU
            {
		        uint8_t tmp = (fourByte & 0xFFC0);	    // mask shift RD, SHIFT REG
					if(tmp == 0){
						return 50;
					} else return 69;
			}       


        default:        //INVALID
            return 69;
    }

}

void r_extract(const uint32_t& fourByte, instrC& load_data) {
    load_data.RS = ((fourByte >> 21) & 0x1f);
    load_data.RT = ((fourByte >> 16 ) & 0x1f );
    load_data.RD = ((fourByte >> 11) & 0x1f);
    load_data.shift = ((fourByte >> 6) & 0x1f);
    //load_data.func = ((fourByte) & 0x3f);
    //load_data.alu_code = load_data.alu_code_finder(fourByte, load_data);

}


uint8_t i_instr_finder(uint32_t fourByte, uint8_t op_code, uint8_t RT){

     switch(op_code){
        case 0x8:       //ADDI
            return 8;
        case 0x9:       //ADDIU
            return 9;
        case 0xC:       //ANDI
            return 10;
        case 0xF:       //LUI
            {
		        uint8_t tmp = (fourByte & 0x3E00000);   // mask shift RS REG
					if(tmp == 0){
						return 11;
					} else return 69;
			}

        case 0x23:      //LW
            return 12;
        case 0xD:       //ORI
            return 13;
        case 0x2B:      //SW
            return 20;
        case 0xE:       //XORI
            return 21;
        case 0x4:       //BEQ
            return 22;
        case 0x1:
			{
		        switch (RT){
		            case 0x0:       //BLTZ
		                return 24;
		            case 0x1:       //BGEZ
		                return 23;
		            case 0x10:      //BLTZAL
		                return 44;
		            case 0x11:      //BGEZAL
		                return 43;

		            default:        //INVALID
		                return 69;
		        }
			}
        case 0x7:       //BGTZ
            return 25;
        case 0x6:       //BLEZ
            return 26;
        case 0x5:       //BNE
            return 27;
        case 0x20:      //LB
            return 29;
        case 0x24:      //LBU
            return 30;
        case 0x21:      //LH
            return 31;
        case 0x25:      //LHU
            return 32;
        case 0x28:      //SB
            return 37;
        case 0x29:      //SH
            return 38;
        case 0xA:       //SLTI
            return 40;
        case 0xB:       //SLTIU
            return 41;
        case 0x22:      //LWL
            return 51;
        case 0x26:      //LWR
            return 52;

        default:        //INVALID
            return 69;

    }

}


void i_extract(const uint32_t& fourByte, instrC& load_data) {
    load_data.RS = ((fourByte >> 21) & 0x1f);
    load_data.RT = ((fourByte >> 16) & 0x1f);
    load_data.imd_const = ((fourByte) & 0xffff);
    //load_data.alu_code = load_data.alu_code_finder(fourByte, load_data);
}

void j_extract(const uint32_t& fourByte, instrC& load_data) {
    load_data.j_memory = (fourByte & 0x3FFFFFF);
    //load_data.alu_code = load_data.alu_code_finder(fourByte, load_data);
}


