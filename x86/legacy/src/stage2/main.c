#include "vga.h"

const char *test1 = "New\nline\ntest ";
const char *test2 = "\tTab test\tTab is ok";
const char *test3 = "This is a test string\nwith a new line\nand a tab\t";
const char *test4 = "Some success";
const char *test5 = "Some failure";

int bmain(void) {
    vga_init();
    cls();
    bprintf(test1);
    log_ok(test4);
    log_fail(test5);

    return 0;
}
