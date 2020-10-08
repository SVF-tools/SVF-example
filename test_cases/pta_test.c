void MAYALIAS(void*,void*);
void NOALIAS(void*,void*);


void foo(int* a, int* b, int* c){
MAYALIAS(a, b);
NOALIAS(a, c);
}

int* y;

// void src(){
// }

// void sink(){
// }

int main(){

    int* x = malloc(10);
    y = x;
    int* z = malloc(20);
    foo(x,y,z);
    // int a = 1;
    // while (a <= 1){
    //     src();
    //     a++;
    // }
    // sink();
}