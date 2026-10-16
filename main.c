#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ncurses.h>

int acc = 0;
int db = 0;
int ab = 0;
int pc = 0;
int mc = 0;
int running = 1;
int ins = 0;
int ram[1000] = {0};
void (*mcaction[200])() = {NULL};

FILE *fptr;

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
    mc = (ins / 1000) * 10;
}

void acc_zero() {
    acc = 0;
    mc += 1;
}

void ins_ab() {
    ab = ins % 1000;
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
    running = 0;
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
    mcaction[83] = minus;
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

/* ---------- CLI helpers ---------- */

void step_cli() {
    if (mc < 0 || mc >= 200 || mcaction[mc] == NULL) {
        running = 0;
        return;
    }
    mcaction[mc]();
}

void run_cli() {
    int count = 0;
    while (running && mc >= 0 && mc < 200 && mcaction[mc] != NULL && count < 100000) {
        mcaction[mc]();
        count++;
    }
}

void load_program(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char line[256];
    int idx = 0;
    memset(ram, 0, sizeof(ram));
    while (fgets(line, sizeof(line), f) && idx < 1000) {
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '#' || *p == '\n' || *p == '\0') continue;
        char *end;
        long val = strtol(p, &end, 10);
        if (end == p) continue;
        ram[idx] = (int)val;
        idx++;
    }
    fclose(f);
}

void mem_set(int addr, int val) {
    if (addr < 0 || addr >= 1000) return;
    ram[addr] = val;
}

/* ---------- ncurses GUI ---------- */

WINDOW *win_top;
WINDOW *win_regs;
WINDOW *win_mem;
WINDOW *win_calc;
WINDOW *win_help;

static const char *opcode_names[200];

static void init_opcode_names(void) {
    int i;
    for (i = 0; i < 200; i++) opcode_names[i] = NULL;
    opcode_names[0]  = "pc_ab  (Fetch)";
    opcode_names[1]  = "ram_db (Fetch)";
    opcode_names[2]  = "db_ins (Fetch)";
    opcode_names[3]  = "ins_mc (Fetch)";
    opcode_names[10] = "acc_zero (Take)";
    opcode_names[11] = "ins_ab (Take)";
    opcode_names[12] = "ram_db (Take)";
    opcode_names[13] = "plus (Take)";
    opcode_names[14] = "pcpp (Take)";
    opcode_names[15] = "mc_zero (Take)";
    opcode_names[20] = "ins_ab (Add)";
    opcode_names[21] = "ram_db (Add)";
    opcode_names[22] = "plus (Add)";
    opcode_names[23] = "pcpp (Add)";
    opcode_names[24] = "mc_zero (Add)";
    opcode_names[30] = "ins_ab (Sub)";
    opcode_names[31] = "ram_db (Sub)";
    opcode_names[32] = "minus (Sub)";
    opcode_names[33] = "pcpp (Sub)";
    opcode_names[34] = "mc_zero (Sub)";
    opcode_names[40] = "ins_ab (Save)";
    opcode_names[41] = "acc_db (Save)";
    opcode_names[42] = "db_ram (Save)";
    opcode_names[43] = "pcpp (Save)";
    opcode_names[44] = "mc_zero (Save)";
    opcode_names[50] = "ins_pc (Jmp)";
    opcode_names[51] = "mc_zero (Jmp)";
    opcode_names[60] = "ins_ab (Tst)";
    opcode_names[61] = "ram_db (Tst)";
    opcode_names[62] = "db_acc (Tst)";
    opcode_names[63] = "zeropcpp (Tst)";
    opcode_names[64] = "pcpp (Tst)";
    opcode_names[65] = "mc_zero (Tst)";
    opcode_names[70] = "acc_zero (Inc)";
    opcode_names[71] = "ins_ab (Inc)";
    opcode_names[72] = "ram_db (Inc)";
    opcode_names[73] = "plus (Inc)";
    opcode_names[74] = "accpp (Inc)";
    opcode_names[75] = "acc_db (Inc)";
    opcode_names[76] = "db_ram (Inc)";
    opcode_names[77] = "pcpp (Inc)";
    opcode_names[78] = "mc_zero (Inc)";
    opcode_names[80] = "acc_zero (Dec)";
    opcode_names[81] = "ins_ab (Dec)";
    opcode_names[82] = "ram_db (Dec)";
    opcode_names[83] = "minus (Dec)";
    opcode_names[84] = "accmm (Dec)";
    opcode_names[85] = "acc_db (Dec)";
    opcode_names[86] = "db_ram (Dec)";
    opcode_names[87] = "pcpp (Dec)";
    opcode_names[88] = "mc_zero (Dec)";
    opcode_names[90] = "ins_ab (Null)";
    opcode_names[91] = "acc_zero (Null)";
    opcode_names[92] = "acc_db (Null)";
    opcode_names[93] = "db_ram (Null)";
    opcode_names[94] = "pcpp (Null)";
    opcode_names[95] = "mc_zero (Null)";
    opcode_names[100]= "stop (Hlt)";
}

static void draw_top(WINDOW *w) {
    werase(w);
    box(w, 0, 0);
    mvwprintw(w, 0, 2, " Jonny Rechner Emulator  (GUI) ");
    wrefresh(w);
}

static void draw_regs(WINDOW *w) {
    werase(w);
    box(w, 0, 0);
    mvwprintw(w, 0, 2, " Registers ");
    mvwprintw(w, 2, 2, "ACC = %d", acc);
    mvwprintw(w, 3, 2, "DB  = %d", db);
    mvwprintw(w, 4, 2, "AB  = %d", ab);
    mvwprintw(w, 5, 2, "PC  = %d", pc);
    mvwprintw(w, 6, 2, "INS = %d", ins);
    mvwprintw(w, 7, 2, "MC  = %d", mc);
    mvwprintw(w, 8, 2, "RUN = %s", running ? "1" : "0");
    if (mc >= 0 && mc < 200 && opcode_names[mc]) {
        mvwprintw(w, 10, 2, "Next micro-op: %s", opcode_names[mc]);
    }
    wrefresh(w);
}

static void draw_calc(WINDOW *w) {
    werase(w);
    box(w, 0, 0);
    mvwprintw(w, 0, 2, " Jonny Calculator Display ");
    mvwprintw(w, 2, 2, "   = %d   ", acc);
    wrefresh(w);
}

static void draw_mem(WINDOW *w) {
    int start, i, rows, cols;
    getmaxyx(stdscr, rows, cols);
    start = pc < 0 ? 0 : (pc > 980 ? 990 - rows + 3 : pc - 1);
    if (start < 0) start = 0;

    werase(w);
    box(w, 0, 0);
    mvwprintw(w, 0, 2, " Memory around PC ");
    mvwprintw(w, 1, 2, "Addr   Value");
    int r = 2;
    for (i = start; i < start + (rows - 4) && i < 1000; i++, r++) {
        if (i == pc) wattron(w, A_REVERSE | COLOR_PAIR(1));
        mvwprintw(w, r, 2, "[%03d]   %d", i, ram[i]);
        if (i == pc) wattroff(w, A_REVERSE | COLOR_PAIR(1));
    }
    wrefresh(w);
}

static void draw_help(WINDOW *w) {
    werase(w);
    box(w, 0, 0);
    mvwprintw(w, 0, 2, " Controls ");
    mvwprintw(w, 2, 2, "SPACE = Step   R = Run   H = Halt/Stop");
    mvwprintw(w, 3, 2, "L = Load   S = Set Mem   Q = Quit");
    mvwprintw(w, 4, 2, "Up/Down = scroll memory");
    wrefresh(w);
}

static void gui_mem_set_prompt(int *sel_addr) {
    echo();
    char buf[32] = {0};
    char buf2[32] = {0};
    int ch, i = 0;

    mvprintw(LINES - 2, 2, "Set address: ");
    while ((ch = getch()) != '\n' && ch != 27 && i < 31) {
        if (ch == KEY_BACKSPACE || ch == 127) { if (i) buf[--i] = 0; }
        else if (ch >= '0' && ch <= '9') buf[i++] = ch;
        mvprintw(LINES - 2, 14, "%-30s", buf);
        refresh();
    }
    buf[i] = 0;
    if (ch == 27 || buf[0] == '\0') { noecho(); return; }
    *sel_addr = atoi(buf);

    i = 0;
    mvprintw(LINES - 1, 2, "Set value: ");
    while ((ch = getch()) != '\n' && ch != 27 && i < 31) {
        if (ch == KEY_BACKSPACE || ch == 127) { if (i) buf2[--i] = 0; }
        else if (ch >= '0' && ch <= '9' || ch == '-') buf2[i++] = ch;
        mvprintw(LINES - 1, 14, "%-30s", buf2);
        refresh();
    }
    buf2[i] = 0;
    if (ch != 27 && buf2[0] != '\0') {
        mem_set(*sel_addr, atoi(buf2));
    }
    noecho();
}

static void gui_load_prompt(void) {
    echo();
    char buf[256] = {0};
    int ch, i = 0;
    mvprintw(LINES - 2, 2, "Load file: ");
    while ((ch = getch()) != '\n' && ch != 27 && i < 255) {
        if (ch == KEY_BACKSPACE || ch == 127) { if (i) buf[--i] = 0; }
        else if (ch >= 32 && ch <= 126) buf[i++] = ch;
        mvprintw(LINES - 2, 13, "%-40s", buf);
        refresh();
    }
    buf[i] = 0;
    noecho();
    if (ch == 27 || buf[0] == '\0') return;
    load_program(buf);
    mc = 0;
    pc = 0;
    ins = 0;
    db = 0;
    ab = 0;
    acc = 0;
    running = 1;
}

static int sel_addr = 0;

static void gui_loop(void) {
    int rows, cols;
    int y_calc = 2;
    int y_regs = 7, h_regs = 12;
    int y_mem = 20, h_mem = 12;
    int y_help = 34;

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    start_color();
    use_default_colors();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);

    getmaxyx(stdscr, rows, cols);

    win_top  = newwin(1, cols, 0, 0);
    win_calc = newwin(4, cols, y_calc, 0);
    win_regs = newwin(h_regs, cols / 3, y_regs, 0);
    win_mem  = newwin(h_mem, cols * 2 / 3, y_mem, cols / 3);
    win_help = newwin(5, cols, y_help, 0);

    init_opcode_names();

    int ch;
    nodelay(stdscr, FALSE);
    timeout(100);

    while (running) {
        draw_top(win_top);
        draw_calc(win_calc);
        draw_regs(win_regs);
        draw_mem(win_mem);
        draw_help(win_help);

        ch = getch();
        if (ch == ERR) continue;

        switch (ch) {
            case ' ':       // Step
                step_cli();
                break;
            case 'r': case 'R':  // Run
                run_cli();
                break;
            case 'h': case 'H':  // Halt
                running = 0;
                break;
            case 'l': case 'L':  // Load
                gui_load_prompt();
                break;
            case 's': case 'S':  // Set memory
                gui_mem_set_prompt(&sel_addr);
                break;
            case KEY_UP:
                if (sel_addr > 0) sel_addr--;
                break;
            case KEY_DOWN:
                if (sel_addr < 999) sel_addr++;
                break;
            case KEY_LEFT:
                if (pc > 0) pc--;
                break;
            case KEY_RIGHT:
                if (pc < 999) pc++;
                break;
            case 'q': case 'Q':
                running = 0;
                break;
        }
    }

    delwin(win_top);
    delwin(win_calc);
    delwin(win_regs);
    delwin(win_mem);
    delwin(win_help);
    endwin();
}

int main(int argc, char **argv) {
    mc_defining();

    if (argc > 1 && strcmp(argv[1], "cli") == 0) {
        /* ---------- CLI mode ---------- */
        printf("=== MicroCPU Simulator (CLI) ===\n");
        printf("Commands: run, step, regs, mem <addr>, set <addr> <val>, load <file>, quit\n");
        char line[256];
        while (running) {
            printf("> ");
            if (!fgets(line, sizeof(line), stdin)) break;
            char cmd[64] = {0};
            char a1[128] = {0};
            char a2[128] = {0};
            sscanf(line, "%63s %127s %127s", cmd, a1, a2);
            if (strcmp(cmd, "run") == 0 || strcmp(cmd, "r") == 0) {
                run_cli();
                printf("Finished. Running=%d PC=%d ACC=%d DB=%d INS=%d AB=%d MC=%d\n", running, pc, acc, db, ins, ab, mc);
            } else if (strcmp(cmd, "step") == 0 || strcmp(cmd, "s") == 0) {
                step_cli();
                printf("MC=%d PC=%d ACC=%d DB=%d INS=%d AB=%d\n", mc, pc, acc, db, ins, ab);
            } else if (strcmp(cmd, "regs") == 0) {
                printf("REGISTERS -> ACC=%d DB=%d AB=%d PC=%d INS=%d MC=%d RUNNING=%d\n", acc, db, ab, pc, ins, mc, running);
                printf("RAM[pc] -> %d\n", ram[pc]);
            } else if (strcmp(cmd, "mem") == 0 && a1[0] != '\0') {
                int start = atoi(a1);
                int i;
                for (i = start; i < start + 10 && i < 1000; i++) {
                    printf("ram[%03d] = %d\n", i, ram[i]);
                }
            } else if (strcmp(cmd, "set") == 0 && a1[0] != '\0' && a2[0] != '\0') {
                mem_set(atoi(a1), atoi(a2));
                printf("ram[%d] set to %d\n", atoi(a1), atoi(a2));
            } else if (strcmp(cmd, "load") == 0 && a1[0] != '\0') {
                load_program(a1);
                printf("Loaded %s\n", a1);
            } else if (strcmp(cmd, "quit") == 0 || strcmp(cmd, "q") == 0) {
                break;
            } else {
                printf("Unknown command: %s\n", cmd);
            }
        }
    } else {
        /* ---------- GUI mode (default) ---------- */
        gui_loop();
    }

    return 0;
}