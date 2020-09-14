#include<stdbool.h>

void sink(){
}

void src(){
    sink();
}

int main(){
    int a = 1;
    while (a <= 1){
        src();
        a++;
    }
    return 0;
}
