#include <stdio.h>
#include <stdlib.h>

int acc = 0;
int db = 0;
int ab = 0;
int pc = 0;
int mc = 0;
int inc = 0;
int ram[999];


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

int main(){
    
}