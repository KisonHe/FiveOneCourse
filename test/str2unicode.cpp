#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

// put the string in hhh
// it will print the unicodes

int main()
{
    char hhh[] = "何扬帆老师好我的姓名和学号是";
//    int i = (sizeof(hhh)-1)/2;
    u_int8_t *ptr = reinterpret_cast<u_int8_t *>(hhh);
    for (int i = 0;i<((sizeof(hhh)-1));i++){
        printf("0x%x",*ptr++);
        printf("%x,",*ptr++);
    }
    return 0;
}

