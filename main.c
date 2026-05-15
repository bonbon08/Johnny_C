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
}

void ram_db() {
    db = ram[ab];
}

void db_ins() {
    ins = db;
}

void ins_mc() {
    int mc = ins / 1000;
}

void acc_zero() {
    acc = 0;
}

void ins_ab() {
    int ins = ab % 1000;
}

void plus() {
    acc = acc + db;
}

void minus() {
    acc = acc - db;
}

void pcpp() {
    pc = pc + 1;
}

void mc_zero() {
    mc = 0;
}

void db_ram() {
    ram[ab] = db;
}

void ins_pc() {
    pc = ins % 1000;
}

void db_acc() {
    acc = db;
}

void accpp() {
    acc += 1;
}

void accmm() {
    acc -= 1;
}

void zeropcpp() {
    if (acc == 0) {
        pcpp();
    }
}

void stop() {
    exit(0)
}

// Defining MC
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


int main(){
    
}