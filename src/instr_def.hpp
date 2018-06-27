#ifndef INSTR_DEF_HPP
#define INSTR_DEF_HPP
#include <cstdint>

struct instrC { //instrC = Instruction Component
       //COMMON
               //string type;
               //custom variable
               //uint8_t alu_code;
       uint8_t instr_code;
       //FOR R_TYPE
       uint8_t RS;
       uint8_t RT;
       uint8_t RD;
       uint8_t shift;
               //uint8_t func; ------ Redundant for CPU -------
       //For I_TYPE
       uint16_t imd_const;
       //For J_TYPE
       uint32_t j_memory;
};

instrC decode_instr(const uint32_t& fourByte);
uint8_t r_instr_finder(uint32_t fourByte);
void r_extract(const uint32_t& fourByte, instrC& load_data);
uint8_t i_instr_finder(uint32_t fourByte, uint8_t op_code, uint8_t RT);
void i_extract(const uint32_t& fourByte, instrC& load_data);
void j_extract(const uint32_t& fourByte, instrC& load_data);

#endif //INST_DEFINITIONS_HPP
