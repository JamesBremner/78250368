#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>


#include "schoolify.h"

sDataStore theDataStore;

main()
{
    readfile();
    cAgent::display();
    AllocateSubjectsDays();
    AllocateTeachersToSubjects();
    displayAssigns();
    return 0;
}
