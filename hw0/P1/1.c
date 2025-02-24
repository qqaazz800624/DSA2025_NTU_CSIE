#include "1.h"

int relation(Employee *employee1, Employee *employee2){
    
    Employee *emp1 = employee1;
    Employee *emp2 = employee2;

    while (emp1 != emp1->boss){
        if (emp1 == employee2){
            return 1;
        }
        emp1 = emp1->boss;
    }
    if (emp1 == employee2){
        return 1;
    }

    while (emp2 != emp2->boss){
        if (emp2 == employee1){
            return 2;
        }
        emp2 = emp2->boss;
    }
    if (emp2 == employee1){
        return 2;
    }

    if (emp1 == emp2){
        return 3;
    }

    return 4; 
}