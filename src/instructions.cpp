#include "instructions.hpp"
#include <cstdint>
#include <cmath>

//mehedi: any comments preceding with 'mehedi:' have been added by mehedi to suggest changes

int32_t signExtend16(int16_t instr) {
    int32_t sign = (0x00008000 & instr) >> 15;
    int32_t value = (0x0000FFFF & instr);

    if (sign == 1){
       return value += 0xFFFF0000;
    } else return value;
}

uint32_t zeroExtend16(int16_t instr){
    uint32_t value = (0x0000FFFF & instr);
	return value;
}


uint32_t addU(uint32_t rsU, uint32_t rtU){

    uint32_t tmp = rsU + rtU;

    return tmp;

}

uint32_t mAnd(uint32_t rsU, uint32_t rtU){

    uint32_t tmp = rsU & rtU;

    return tmp;

}

void jr(uint32_t rsU, bool& flag){

    uint8_t tmp = rsU & 0x3;

    if(tmp == 0){
        flag = true;
    } else flag = false;

}



uint32_t mOr(uint32_t rsU, uint32_t rtU){

    uint32_t tmp = rsU | rtU;

    return tmp;

}

uint32_t sltU(uint32_t rsU, uint32_t rtU){

    if(rsU < rtU){
        return 1;
    } else return 0;

}


uint32_t subU(uint32_t rsU, uint32_t rtU){

    uint32_t tmp = rsU - rtU;

    return tmp;

}


uint32_t mXor(uint32_t rsU, uint32_t rtU){

    uint32_t tmp = rsU ^ rtU;

    return tmp;

}

uint32_t add(uint32_t rsU, uint32_t rtU, bool& overflowFlag){

    overflowFlag = false;

    int32_t rs = rsU;
    int32_t rt = rtU;

    if( ( (rs>=0)&&(rt>=0)&&((rs+rt)<0) ) || ( (rs<0)&&(rt<0)&&((rs+rt)>=0) ) ){
        overflowFlag = true;
        return 10;
    }
    else{
        uint32_t tmp = rs + rt;
        return tmp;
    }
}

uint32_t addI(uint32_t rsU, uint16_t immd, bool& overflowFlag){

    overflowFlag = false;

    int32_t rs = rsU;
    int32_t iConst = signExtend16(immd);

    if( ( (rs>=0)&&(iConst>=0)&&((rs+iConst)<0) ) || ( (rs<0)&&(iConst<0)&&((rs+iConst)>=0) ) ){
            overflowFlag = true;
            return 10;
    }
    else{
        uint32_t tmp = rs + iConst;
        return tmp;
    }

}

uint32_t addIU(uint32_t rsU, uint16_t immd){

    uint32_t iConst = zeroExtend16(immd);
    uint32_t tmp = rsU + iConst;

    return tmp;

}

uint32_t mAndI(uint32_t rsU, uint16_t immd){

    uint32_t iConst = zeroExtend16(immd);

    uint32_t tmp = rsU & iConst;

    return tmp;
}

uint32_t lui(uint16_t immd){

    uint32_t tmp = immd << 16; // low 16b filled with 0's

    return tmp;
}

//Load word pending
//Store word pending

uint32_t mOrI(uint32_t rsU, uint16_t immd){

    uint32_t iConst = zeroExtend16(immd);

    uint32_t tmp = rsU | iConst;

    return tmp;

}

uint32_t mXorI(uint32_t rsU, uint16_t immd){

    uint32_t iConst = zeroExtend16(immd);

    uint32_t tmp = rsU ^ iConst;

    return tmp;

}

uint32_t sll(uint32_t rtU, uint8_t sh){

    uint32_t tmp = rtU << sh;

    return tmp;

}

uint32_t slt(uint32_t rsU, uint32_t rtU){

    int32_t rs = rsU;
    int32_t rt = rtU;
    if(rs < rt){
        return 1;
    } else return 0;

}
uint32_t sra(uint32_t rtU, uint8_t sh){

    uint8_t sign = rtU >> 31;
    uint32_t mask = 0;
    
    if(sign == 1){
        for(uint8_t i=0; i<sh; i++){
            mask += pow(2,31-i);
        }
    }

    uint32_t tmp = (mask & (rtU >> sh));

    return tmp;

}

uint32_t srav(uint32_t rsU, uint32_t rtU){
    uint8_t sh = 0x1F & rsU;
    uint32_t tmp = sra(rtU, sh);

    return tmp;

}


uint32_t srl(uint32_t rtU, uint8_t sh){

    uint32_t tmp = rtU >> sh;

    return tmp;

}

uint32_t sub(uint32_t rsU, uint32_t rtU, bool& overflowFlag){

    overflowFlag = false;

    int32_t rs = rsU;
    int32_t rt = rtU;

    if( ( (rs>=0)&&(rt<0)&&((rs-rt)<0) ) || ( (rs<0)&&(rt>=0)&&((rs-rt)>=0) ) ){
        overflowFlag = true;
        return -10;
    }else
    {
        uint32_t tmp = rs - rt;
        return tmp;
    }
}

bool beq(uint32_t rsU, uint32_t rtU){
    if(rsU == rtU){
        return true;
    }
    else{
        return false;
    }
}

bool bgez(uint32_t rsU){
// mehedi: more effecient to just conert to int32_t and then cmp to 0?
	//int32_t rs = rsU
	//if(rs >= 0)..

    int8_t sign = rsU >> 31;
	
    if(sign == 0){
        return true;
    }
    else{
        return false;
    }
}

bool bltz(uint32_t rsU){

    int8_t sign = rsU >> 31;

    if(sign == 1){
        return true;
    }
    else{
        return false;
    }
}

bool bgtz(uint32_t rsU){
//mehedi: no need for signed int32_t
    int32_t rs = rsU;
    uint8_t sign = rsU >> 31;

    if(sign == 0 && rs != 0 ){
        return true;
    }
    else{
        return false;
    }
}

bool blez(uint32_t rsU){

    int32_t rs = rsU;
    int8_t sign = rsU >> 31;

    if(sign == 1 || rs == 0 ){
        return true;
    }
    else{
        return false;
    }
}

bool bne(uint32_t rsU, uint32_t rtU){
//mehedi: no need for type conv
    if(rsU != rtU){
        return true;
    }
    else{
        return false;
    }
}

uint32_t sltI(uint32_t rsU, uint16_t immd){

    int32_t rs = rsU;
    int32_t iConst = signExtend16(immd);
    if(rs < iConst){
        return 1;
    } else return 0;
}

uint32_t sltIU(uint32_t rsU, uint16_t immd){

    uint32_t iConst = zeroExtend16(immd);
    if(rsU < iConst){
        return 1;
    } else return 0;
}

uint32_t sltIU(uint32_t rsU, uint32_t rtU){

    uint8_t rs5 = 0x1F & rsU;
    uint32_t tmp = rtU << rs5;

    return tmp;
}

uint32_t sllv(uint32_t rsU, uint32_t rtU){

    uint8_t rs5 = 0x1F & rsU;
    uint32_t tmp = rtU << rs5;

    return tmp;
}

uint32_t srlv(uint32_t rsU, uint32_t rtU){

    uint8_t rs5 = 0x1F & rsU;
    uint32_t tmp = rtU >> rs5;

    return tmp;
}

bool bgezal(uint32_t rsU){
    
    int32_t rs = rsU;

    if((rs>>31) == 0){
        return true;
    } 
    else return false;
}

bool bltzal(uint32_t rsU){

    int32_t rs = rsU;

    if((rs>>31) == 1){
        return true;
    } 
    else return false;
}

double_uint32 div(uint32_t rsU, uint32_t rtU){
    
    double_uint32 result;
    result.hi = 0;
    result.lo = 0;

    uint32_t remainder = 0;
    int64_t quotient = 0;
    int32_t rs = rsU;
    int32_t rt = rtU;

	if(rt != 0){			//C++ div 0 protection
   		 quotient = rs / rt;
	}
    remainder = rs % rt;

    result.lo = (quotient & 0xFFFFFFFF);
    result.hi = remainder;
	
    return result;
}

double_uint32 divU(uint32_t rsU, uint32_t rtU){
    
    double_uint32 result;
    result.hi = 0;
    result.lo = 0;

    uint32_t remainder = 0;
    uint64_t quotient = 0;

	if(rtU != 0){	//div 0 protection
    	quotient = (uint64_t)rsU / (uint64_t)rtU;
	}
    remainder = rsU % rtU;

     
    result.lo = (quotient & 0xFFFFFFFF);
    result.hi = remainder;

    return result;
}

double_uint32 mult(uint32_t rsU, uint32_t rtU){

    double_uint32 result;
    result.hi = 0;
    result.lo = 0;

    int64_t product = 0;
    int32_t rs = rsU;
    int32_t rt = rtU;

    product = (int64_t)rs * (int64_t)rt;
    
    result.hi = ((product>>32) & 0xFFFFFFFF);
    result.lo = (product & 0xFFFFFFFF);
    
    return result;
}

double_uint32 multU(uint32_t rsU, uint32_t rtU){

    double_uint32 result;
    result.hi = 0;
    result.lo = 0;

    uint64_t product = 0;

    product = (uint64_t)rsU * (uint64_t)rtU;


    result.hi = ((product>>32) & 0xFFFFFFFF);
    result.lo = (product & 0xFFFFFFFF);


    return result;
}