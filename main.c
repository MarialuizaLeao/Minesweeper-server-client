#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int main(){
    int teste = 0;
    int x = 0;
    while (x <= 0)
    {
        switch(teste)
        {
        case 0:
            printf("teste 0\n");
            printf("mesmo\n");
            x++;
            continue;
        case 1:
            printf("teste 1\n");
            break;
        default:
            break;
        }
        printf("Saiu\n");
    }
    
    
}