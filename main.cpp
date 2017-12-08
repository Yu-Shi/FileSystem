#include <iostream>
#include "operations.h"

int main(){
    initialize();
    while(1){
        string cmd;
        cout << "<<";
        cin >> cmd;
        operation(cmd);
    }
    return 0;
}
