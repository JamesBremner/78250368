#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include "GraphTheory.h"

#include "schoolify.h"

sDataStore theDataStore;

main()
{
    readfile();
    cAgent::display();
    allocateMaxFlow();
    return 0;
}
