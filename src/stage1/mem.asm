%ifndef MEM_ASM
%define MEM_ASM

%define START_STAGE1 0x7C00                     ; address of stage1
%define START_STAGE2 0x7E00                     ; address of next stage
                                                ; TODO: make it random address between  0x7E00 and (0xFFFFF - STAGE2_SIZE) (1MB)

%endif