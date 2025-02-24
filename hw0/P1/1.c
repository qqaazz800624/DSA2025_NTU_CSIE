#include "1.h"
#include <stddef.h>

int isSubordinate(Employee *employee1, Employee *employee2){
    
    while (employee1 != employee1->boss){
        if (employee1->boss == employee2){
            return 1;
        }
        employee1 = employee1->boss;
    }

    return 0;
}

int isColleague(Employee *employee1, Employee *employee2){
    if (employee1->boss == employee2->boss && employee1 != employee2){
        return 1;
    }
    return 0;
}

int relation(Employee *employee1, Employee *employee2){
    if (isSubordinate(employee1, employee2)){
        return 1;
    }
    if (isSubordinate(employee2, employee1)){
        return 2;
    }
    if (isColleague(employee1, employee2)){
        return 3;
    }
    return 4; 
}