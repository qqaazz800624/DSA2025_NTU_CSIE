#include "1.h"
#include <stddef.h>

int relation(Employee *employee1, Employee *employee2){
    // Check if employee1 is subordinate to employee2
    Employee *p = employee1;
    while (p != NULL) {
        if (p == employee2) {
            return 1; // employee1 is subordinate to employee2.
        }
        // If we reached a top-level employee (boss pointer equals itself), stop.
        if (p == p->boss) {
            break;
        }
        p = p->boss;
    }

    // Check if employee1 is supervisor of employee2.
    p = employee2;
    while (p != NULL) {
        if (p == employee1) {
            return 2; // employee1 is supervisor of employee2.
        }
        if (p == p->boss) {
            break;
        }
        p = p->boss;
    }

    // For colleague check, if an employee's boss pointer equals itself, treat it as NULL.
    if (employee1->boss != employee1 && employee2->boss != employee2 && employee1->boss == employee2->boss){
        return 3; // employee1 and employee2 are colleagues.
    }

    // Otherwise, employee1 and employee2 are unrelated
    return 4; 
}