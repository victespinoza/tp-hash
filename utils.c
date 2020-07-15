#include "utils.h"

size_t siguiente_primo(size_t numero){
    if(numero%2 == 0 && numero != 2){
        numero+=1;
    }
    while(!es_primo(numero)){
        numero+=2;
    }
    return numero;
}

bool es_primo(size_t primo){
    int i;
    if(primo == 2){
        return true;
    }
    if(primo % 2 == 0 || primo <= 1){
        return false;
    } else {
        for(i=3; i<=sqrt((double)primo); i+=2){
            if(primo % i == 0){
                return false;
            }
        }
    }
    return true;
}

double sqrt(double numero){
    double temp, sqrt;
    sqrt = numero / 2;
    temp = 0;
    while(sqrt != temp){
        temp = sqrt;
        sqrt = (numero / temp + temp) / 2;
    }
    return sqrt;
}