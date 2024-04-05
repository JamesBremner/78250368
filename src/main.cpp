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
    cAgent::displaySpec();
    AllocateSubjectsDays();
    cAssign::TeachersToSubjects();
    cAgent::displayWork();
    return 0;
}
