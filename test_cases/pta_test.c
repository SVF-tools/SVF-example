void MAYALIAS(void*,void*);
void NOALIAS(void*,void*);


void foo(int* a, int* b, int* c){
MAYALIAS(a, b);
NOALIAS(a, c);
}

int* y;

int main(){

    int* x = malloc(10);
    y = x;
    int* z = malloc(20);
    foo(x,y,z);
}