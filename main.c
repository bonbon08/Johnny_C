#include <stdio.h>
#include <stdlib.h>

int acc = 0;
int db = 0;
int ab = 0;
int pc = 0;
int mc = 0;
int ins = 0;
int ram[1000] = {0};
void (*mcaction[200])() = {NULL};

//MicroBefehle
void pc_ab() {
    ab = pc;
    mc += 1;
}

void ram_db() {
    db = ram[ab];
    mc += 1;
}

void db_ins() {
    ins = db;
    mc += 1;
}

void acc_db() {
    db = acc;
    mc += 1;
}

void ins_mc() {
    int mc = ins / 1000;
    mc *= 10;
}

void acc_zero() {
    acc = 0;
    mc += 1;
}

void ins_ab() {
    int ins = ab % 1000;
    mc += 1;
}

void plus() {
    acc = acc + db;
    mc += 1;
}

void minus() {
    acc = acc - db;
    mc += 1;
}

void pcpp() {
    pc = pc + 1;
    mc += 1;
}

void mc_zero() {
    mc = 0;
}

void db_ram() {
    ram[ab] = db;
    mc += 1;
}

void ins_pc() {
    pc = ins % 1000;
    mc += 1;
}

void db_acc() {
    acc = db;
    mc += 1;
}

void accpp() {
    acc += 1;
    mc += 1;
}

void accmm() {
    acc -= 1;
    mc += 1;
}

void zeropcpp() {
    if (acc == 0) {
        pcpp();
    }
    mc += 1;
}

void stop() {
    exit(0);
}

// Defining MC
void mc_defining(){
    //Fetch
    mcaction[0] = pc_ab;
    mcaction[1] = ram_db;
    mcaction[2] = db_ins;
    mcaction[3] = ins_mc;

    //Take
    mcaction[10] = acc_zero;
    mcaction[11] = ins_ab;
    mcaction[12] = ram_db;
    mcaction[13] = plus;
    mcaction[14] = pcpp;
    mcaction[15] = mc_zero;

    //Add
    mcaction[20] = ins_ab;
    mcaction[21] = ram_db;
    mcaction[22] = plus;
    mcaction[23] = pcpp;
    mcaction[24] = mc_zero;

    //Sub
    mcaction[30] = ins_ab;
    mcaction[31] = ram_db;
    mcaction[32] = minus;
    mcaction[33] = pcpp;
    mcaction[34] = mc_zero;

    //Save
    mcaction[40] = ins_ab;
    mcaction[41] = acc_db;
    mcaction[42] = db_ram;
    mcaction[43] = pcpp;
    mcaction[44] = mc_zero;

    //Jmp
    mcaction[50] = ins_pc;
    mcaction[51] = mc_zero;

    //Tst
    mcaction[60] = ins_ab;
    mcaction[61] = ram_db;
    mcaction[62] = db_acc;
    mcaction[63] = zeropcpp;
    mcaction[64] = pcpp;
    mcaction[65] = mc_zero;

    //Inc
    mcaction[70] = acc_zero;
    mcaction[71] = ins_ab;
    mcaction[72] = ram_db;
    mcaction[73] = plus;
    mcaction[74] = accpp;
    mcaction[75] = acc_db;
    mcaction[76] = db_ram;
    mcaction[77] = pcpp;
    mcaction[78] = mc_zero;

    //Dec
    mcaction[80] = acc_zero;
    mcaction[81] = ins_ab;
    mcaction[82] = ram_db;
    mcaction[83] = plus;
    mcaction[84] = accmm;
    mcaction[85] = acc_db;
    mcaction[86] = db_ram;
    mcaction[87] = pcpp;
    mcaction[88] = mc_zero;

    //Null
    mcaction[90] = ins_ab;
    mcaction[91] = acc_zero;
    mcaction[92] = acc_db;
    mcaction[93] = db_ram;
    mcaction[94] = pcpp;
    mcaction[95] = mc_zero;

    //Hlt
    mcaction[100] = stop;
}

int main(){
    
}