#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include "GraphTheory.h"

class cAgent
{
public:
    std::string myID;
    std::vector<std::string> myTasks;

    bool cando( const std::string& task )
    {
        return std::find(
            myTasks.begin(),myTasks.end(),
            task )
             != myTasks.end();
    }
};

std::set<std::string> theTasks;
std::vector<cAgent> theAgents;

void readfile()
{
    std::ifstream ifs("../dat/data1.txt");
    if (!ifs.is_open())
        throw std::runtime_error(
            "Cannot read input");
    std::string line;
    while (getline(ifs, line))
    {
        // std::cout << line << "\n";

        if (line.find("id:") != -1)
        {
            int p = line.find("\"");
            int q = line.find("\"", p + 1);
            line = line.substr(p + 1, q - p - 1);
            if (line[0] == 'T')
            {
                cAgent A;
                A.myID = line;
                theAgents.push_back(A);
            }
        }
        else if (line.find("subjects:") != -1)
        {
            int p = line.find("\"", p + 1);
            while (p != -1)
            {
                int q = line.find("\"", p + 1);
                auto task = line.substr(p + 1, q - p - 1);
                theTasks.insert(task);
                theAgents.back().myTasks.push_back(task);

                p = line.find("\"", q + 1);
            }
        }
        else if (line.find("const subjects") != -1)
            break;
    }
    std::cout << "Teachers:\n ";
    for (auto &a : theAgents)
    {
        std::cout << a.myID << " can teach ";
        for (auto &t : a.myTasks)
            std::cout << t << " ";
        std::cout << "\n";
    }
}

void allocateMaxFlow()
{
    raven::graph::cGraph g;

    g.directed();

    // loop over the tasks in timeslot
    for (auto & task : theTasks)
    {
        // loop over agents that can do task
        for (cAgent &a : theAgents)
        {
            if( ! a.cando( task ))
                continue;

            // add link from agent to task agent is able to do
            g.add(
                a.myID,
                task );
        }
    }

    // apply pathfinder maximum flow allocation algorithm 
    raven::graph::sGraphData gd;
    gd.g = g;
    auto sg = alloc(gd);

    std::cout << "\nAssignments:\n";
    for (std::pair<int, int> p : sg.edgeList())
    {
        std::cout << "teacher " << sg.userName(p.first )
            << " assigned to " << sg.userName (p.second)
            << "\n";
    }
}
main()
{
    readfile();
    allocateMaxFlow();
    return 0;
}
