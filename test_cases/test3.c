#include<stdbool.h>

void sink(){
}

int src(){
    int b = 2;
    return b;
}

int main(){
    int a = 1;
    if(a >= src())
        a++;
    else 
        sink();
    return 0;
}
