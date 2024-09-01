const char *msg = "Hello, World!";
char *vmem = (char*)0xb8000;

int bmain(void) {
    for (char i=0; msg[i] != '\0'; i++) {
        *(vmem + i*2) = msg[i];
    }
    return 0;
}