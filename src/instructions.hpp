#ifndef INSTRUCTIONS_HPP
#define INSTRUCTIONS_HPP

#include <cstdint>

struct double_uint32 { //Stores two uint32_t's used for Multiplications and Divisions

        uint32_t hi; 
        uint32_t lo;
};


/////////////////// UNDER THE HOOD STUFF ///////////////////////////
int32_t signExtend16(int16_t instr);
uint32_t zeroExtend16(int16_t instr);
////////////////////////////////////////////////////////////////////


///////////////////////// LEVEL 1 //////////////////////////////////
uint32_t addU(uint32_t rsU, uint32_t rtU);
uint32_t mAnd(uint32_t rsU, uint32_t rtU);
void jr(uint32_t rsU, bool& flag);
uint32_t mOr(uint32_t rsU, uint32_t rtU);
uint32_t sltU(uint32_t rsU, uint32_t rtU);
uint32_t subU(uint32_t rsU, uint32_t rtU);
uint32_t mXor(uint32_t rsU, uint32_t rtU);
////////////////////////////////////////////////////////////////////


///////////////////////// LEVEL 2 //////////////////////////////////
uint32_t add(uint32_t rsU, uint32_t rtU, bool& overflowFlag);
uint32_t addI(uint32_t rsU, uint16_t immd, bool& overflowFlag);
uint32_t addIU(uint32_t rsU, uint16_t immd);
uint32_t mAndI(uint32_t rsU, uint16_t immd);
uint32_t lui(uint16_t immd);
uint32_t mOrI(uint32_t rsU, uint16_t immd);
uint32_t mXorI(uint32_t rsU, uint16_t immd);
uint32_t sll(uint32_t rtU, uint8_t sh);
uint32_t slt(uint32_t rsU, uint32_t rtU);
uint32_t sra(uint32_t rtU, uint8_t sh);
uint32_t srav(uint32_t rsU, uint32_t rtU);
uint32_t srl(uint32_t rtU, uint8_t sh);
uint32_t sub(uint32_t rsU, uint32_t rtU, bool& overflowFlag);
//________________IMPLEMENTED DIRECTLY IN SIMULATOR_______________//
                        //LW            //SW
////////////////////////////////////////////////////////////////////


////////////////////////// LEVEL 3 /////////////////////////////////
bool beq(uint32_t rsU, uint32_t rtU);
bool bgez(uint32_t rsU);
bool bltz(uint32_t rsU);
bool bgtz(uint32_t rsU);
bool blez(uint32_t rsU);
bool bne(uint32_t rsU, uint32_t rtU);
uint32_t sltI(uint32_t rsU, uint16_t immd);
uint32_t sltIU(uint32_t rsU, uint16_t immd);
uint32_t srlv(uint32_t rsU, uint32_t rtU);
uint32_t sllv(uint32_t rsU, uint32_t rtU);
//________________IMPLEMENTED DIRECTLY IN SIMULATOR_______________//
//J     //LB    //LBU   //LH    //LHU   //MFHI  //MFLO  //MTHI
             //MTLO          //SB            //SH
////////////////////////////////////////////////////////////////////


/////////////////////////// LEVEL 4 ////////////////////////////////
bool bgezal(uint32_t rsU);
bool bltzal(uint32_t rsU); 
double_uint32 div(uint32_t rsU, uint32_t rtU);
double_uint32 divU(uint32_t rsU, uint32_t rtU);
double_uint32 mult(uint32_t rsU, uint32_t rtU);
double_uint32 multU(uint32_t rsU, uint32_t rtU);
//________________IMPLEMENTED DIRECTLY IN SIMULATOR_______________//
                        //JAL           //JALR
////////////////////////////////////////////////////////////////////


////////////////////////// BOSS LEVEL //////////////////////////////
//________________IMPLEMENTED DIRECTLY IN SIMULATOR_______________//
                        //LWL           //LWR
////////////////////////////////////////////////////////////////////

#endif
