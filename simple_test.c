// Simple test: compute 5 + 3 = 8
// Step-by-step program:
// 0: ins_ab (ab = 0)       -> ab = 0
// 1: ram_db (db = ram[0])  -> db = 5 (we'll set ram[0]=5 first)
// 2: plus (acc = acc + db) -> acc = 0 + 5 = 5
// 3: ins_ab (ab = 1)      -> ab = 1
// 4: ram_db (db = ram[1]) -> db = 3 (we'll set ram[1]=3)
// 5: plus (acc = acc + db) -> acc = 5 + 3 = 8
// 6: stop (mcaction[100] = stop)

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int running = 1;
int ins = 0;
int acc = 0, db = 0, ab = 0, pc = 0, mc = 0;

// Microcode definition
void mc_defining() {
    //Fetch
    mcaction[0] = pc_ab;
    mcaction[1] = ram_db;
    mcaction[2] = plus;
    mcaction[3] = ins_ab;
    mcaction[4] = ram_db;
    mcaction[5] = plus;
    mcaction[7] = stop;

    //Take
    mcaction[10] = acc_zero;
    mcaction[11] = ins_ab;

    //Add
    mcaction[20] = ins_ab;
    mcaction[21] = ram_db;
    mcaction[22] = plus;
    mcaction[23] = pcpp;
    mcaction[24] = mc_zero;
}

void pc_ab() {
    ab = pc;
    mc += 1;
}

void ram_db() {
    db = ram[ab];
    mc += 1;
}

void plus() {
    acc = acc + db;
    mc += 1;
}

void ins_ab() {
    ab = ins % 1000;
    mc += 1;
}

void stop() {
    running = 0;
}

void acc_zero() {
    acc = 0;
    mc += 1;
}

void ins_pc() {
    pc = ins % 1000;
    mc += 1;
}

void mc_zero() {
    mc = 0;
}

void pcpp() {
    pc = pc + 1;
    mc += 1;
}

// Global microcode pointers (simplified for test)
// In real code, these would be filled by mc_defining()
void (*mcaction[200])() = {NULL};

int main() {
    // Define microcode
    mcaction[0] = pc_ab;
    mcaction[1] = ram_db;
    mcaction[2] = plus;
    mcaction[3] = ins_ab;
    mcaction[4] = ram_db;
    mcaction[5] = plus;
    mcaction[7] = stop;

    // Set up RAM with test values
    ram[0] = 5;  // First number
    ram[1] = 3;  // Second number

    // Start execution
    mc = 0;
    while (running) {
        mcaction[mc]();
    }

    return 0;
}