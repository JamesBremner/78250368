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
    int myWorkDaysMax;
    int myWorkDaysActual;

    cAgent()
        : myWorkDaysActual(0)
    {
    }

    bool cando(const std::string &task);

    static cAgent *find(const std::string &id);
};

std::set<std::string> theTasks;
std::vector<cAgent> theAgents;

cAgent *cAgent::find(const std::string &id)
{
    int index = 0;
    for (auto &a : theAgents)
    {
        if (a.myID == id)
        {
            return &theAgents[index];
        }
        index++;
    }
    return 0;
}

bool cAgent::cando(const std::string &task)
{
    if (std::find(
            myTasks.begin(), myTasks.end(),
            task) == myTasks.end())
        return false;
    if (myWorkDaysActual >= myWorkDaysMax)
        return false;
    return true;
}

void readfile()
{
    std::ifstream ifs("../dat/data1.txt");
    if (!ifs.is_open())
        throw std::runtime_error(
            "Cannot read input");
    std::string line;
    enum class eSection
    {
        teachers,
        subjects,
        classes
    } section;
    while (getline(ifs, line))
    {
        // std::cout << line << "\n";

        if (line.find("const teachers") != -1)
            section = eSection::teachers;
        else if (line.find("const subjects") != -1)
            section = eSection::subjects;
        else if (line.find("const classes") != -1)
            section = eSection::classes;

        else if (line.find("id:") != -1)
        {
            int p = line.find("\"");
            int q = line.find("\"", p + 1);
            line = line.substr(p + 1, q - p - 1);
            switch (section)
            {
            case eSection::teachers:
                cAgent A;
                A.myID = line;
                theAgents.push_back(A);
                break;
            }
        }
        else if (line.find("workDays:") != -1)
        {
            int p = line.find(":");
            theAgents.back().myWorkDaysMax = atoi(line.substr(p + 2).c_str());
        }
        else if (line.find("subjects:") != -1)
        {
            if (section == eSection::teachers)
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
        }
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

    // loop over weekdays
    for (int day = 0; day < 6; day++)
    {
        raven::graph::cGraph g;
        g.directed();
        
        // loop over the tasks
        for (auto &task : theTasks)
        {
            // loop over agents
            for (cAgent &a : theAgents)
            {
                // check teacher can teach this subject
                if (!a.cando(task))
                    continue;

                // add link from agent to task agent is able to do
                g.add(
                    a.myID,
                    task);
            }
        }

        // apply pathfinder maximum flow allocation algorithm
        raven::graph::sGraphData gd;
        gd.g = g;
        auto sg = alloc(gd);

        const std::vector<std::string> daynames{"Sat", "Sun", "Mon", "Tue", "Wed", "Thr"};
        std::cout << "\nAssignments for " << daynames[day] << ":\n";
        for (std::pair<int, int> p : sg.edgeList())
        {
            auto teacherID = sg.userName(p.first);
            auto *pt = cAgent::find(teacherID);
            pt->myWorkDaysActual++;
            std::cout << "teacher " << teacherID
                      << " assigned to " << sg.userName(p.second)
                      << "\n";
        }
    }
}
main()
{
    readfile();
    allocateMaxFlow();
    return 0;
}
