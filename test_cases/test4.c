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
        sink();
    else 
        a++;
    return 0;
}
