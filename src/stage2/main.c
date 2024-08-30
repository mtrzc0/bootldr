int bmain(void) {
    *(char*)0xb8000 = 'X';
    while(1);
}