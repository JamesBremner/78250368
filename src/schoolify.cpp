#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include "GraphTheory.h"

#include "schoolify.h"

cAgent::cAgent(const std::string &id)
    : myID(id)
{
    for (int d = 0; d < 6; d++)
    {
        myAvailable[d] = true;
        myWorkDaysActual[d] = 0;
    }
}

bool cAgent::parseSubjects(
    const std::string &line)
{
    if (line.find("subjects:") == -1)
        return false;

    int p = line.find("\"");
    while (p != -1)
    {
        int q = line.find("\"", p + 1);
        auto task = line.substr(p + 1, q - p - 1);
        theDataStore.Tasks.insert(task);
        myTasks.push_back(task);

        p = line.find("\"", q + 1);
    }
    return true;
}

bool cAgent::parseWorkdays(
    const std::string &line)
{
    if (line.find("workDays:") == -1)
        return false;

    int p = line.find(":");
    theDataStore.Agents.back().myWorkDaysMax = atoi(line.substr(p + 2).c_str());

    return true;
}

bool cAgent::parseUnavailableDays(
    const std::string &line)
{
    if (line.find("unavailableDays:") == -1)
        return false;

    int p = line.find("'");
    while (p != -1)
    {
        auto day = line.substr(p + 1, 3);
        int iday;
        if (day == "Sat")
            iday = 0;
        else if (day == "Sun")
            iday = 1;
        else if (day == "Mon")
            iday = 2;
        else if (day == "Tue")
            iday = 3;
        else if (day == "Wed")
            iday = 4;
        else if (day == "Thr")
            iday = 5;
        myAvailable[iday] = false;
        p = line.find("'", p + 5);
    }
    return true;
}

cAgent *cAgent::find(const std::string &id)
{
    int index = 0;
    for (auto &a : theDataStore.Agents)
    {
        if (a.myID == id)
        {
            return &theDataStore.Agents[index];
        }
        index++;
    }
    return 0;
}

bool cAgent::cando(
    const std::string &task,
    int day)
{
    if (ActualDaysWorked() >= myWorkDaysMax)
        return false;

    if (!myAvailable[day])
        return false;

    if (std::find(
            myTasks.begin(), myTasks.end(),
            task) == myTasks.end())
        return false;

    return true;
}

void cAgent::display()
{
    std::cout << "Teachers:\n";
    for (auto &a : theDataStore.Agents)
    {
        std::cout << a.myID << " can teach ";
        for (auto &t : a.myTasks)
            std::cout << t << " ";
        std::cout << "\n";
    }
}

void readfile()
{
    std::ifstream ifs("data.txt");
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

        if (line.find("//") != -1)
            continue;

        if (line.find("const teachers") != -1)
        {
            section = eSection::teachers;
            continue;
        }
        else if (line.find("const subjects") != -1)
        {
            section = eSection::subjects;
            continue;
        }
        else if (line.find("const classes") != -1)
        {
            section = eSection::classes;
            continue;
        }

        else if (line.find("id:") != -1)
        {
            int p = line.find("\"");
            int q = line.find("\"", p + 1);
            line = line.substr(p + 1, q - p - 1);
            switch (section)
            {
            case eSection::teachers:
                theDataStore.Agents.emplace_back(line);
                break;
            case eSection::subjects:
                theDataStore.Subjects.back().ID(line);
                break;
            }
            continue;
        }

        else if (line.find("class:") != -1)
        {
            switch (section)
            {
            case eSection::subjects:
            {
                cSubject S;
                theDataStore.Subjects.push_back(S);
                theDataStore.Subjects.back().parseClass(line);
                break;
            }
            }
            continue;
        }

        if (theDataStore.Agents.back().parseWorkdays(line))
            continue;

        if (theDataStore.Agents.back().parseUnavailableDays(line))
            continue;

        if (
            section == eSection::teachers &&
            theDataStore.Agents.back().parseSubjects(line))
            continue;

        if (
            section == eSection::subjects &&
            theDataStore.Subjects.back().parseWeekly(line))
            continue;
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
        for (auto &task : theDataStore.Tasks)
        {
            // loop over agents
            for (cAgent &a : theDataStore.Agents)
            {
                // check teacher can teach this subject
                if (!a.cando(task, day))
                    continue;

                // add link from agent to task agent is able to do
                g.add(
                    a.ID(),
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
            pt->incDaysWorked(day);
            std::cout << "teacher " << teacherID
                      << " assigned to " << sg.userName(p.second)
                      << "\n";
        }
    }
}

void allocateTeachers()
{

    // loop over weekdays
    for (int day = 0; day < 6; day++)
    {
        raven::graph::cGraph g;
        g.directed();

        // loop over the tasks
        for (auto &task : theDataStore.SubjectsPerDay[day])
        {
            // loop over agents
            for (cAgent &a : theDataStore.Agents)
            {
                // check teacher can teach this subject
                if (!a.cando(task, day))
                    continue;

                // add link from agent to task agent is able to do
                g.add(
                    a.ID(),
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
            pt->incDaysWorked(day);
            std::cout << "teacher " << teacherID
                      << " assigned to " << sg.userName(p.second)
                      << "\n";
        }
    }
}

void AllocateSubjectsDays()
{
    theDataStore.SubjectsPerDay.clear();
    theDataStore.SubjectsPerDay.resize(6, std::vector<std::string>(0));
    for (auto &s : theDataStore.Subjects)
    {
        for (int l = 0; l < s.weeklyLectures(); l++)
        {
            int minday = 0;
            int minl = theDataStore.SubjectsPerDay[0].size();
            for (int day = 0; day < 6; day++)
            {
                int sz = theDataStore.SubjectsPerDay[day].size();
                if (sz < minl)
                {
                    minl = sz;
                    minday = day;
                }
            }
            theDataStore.SubjectsPerDay[minday].push_back(s.ID());
        }
    }

    std::cout << "\nSubjects to days\n";
    for (int day = 0; day < 6; day++)
    {
        std::cout
            << theDataStore.daynames[day] << " ";
        for (auto id : theDataStore.SubjectsPerDay[day])
        {
            std::cout << id << " ";
        }
        std::cout << "\n";
    }
}

void AllocateTeachersToSubjects()
{
    theDataStore.AssignsPerDay.clear();
    theDataStore.AssignsPerDay.resize(6);
    for (int day = 0; day < 6; day++)
    {
        for (auto &s : theDataStore.SubjectsPerDay[day])
        {
            for (auto &t : theDataStore.Agents)
            {
                if (!t.cando(s, day))
                    continue;

                theDataStore.AssignsPerDay[day].push_back(
                    std::make_pair(t.ID(), s));
            }
        }
    }
}
void displayAssigns()
{
    for (int day = 0; day < 6; day++)
    {
        std::cout << "\n"
                  << theDataStore.daynames[day] << ":\n";

        for (auto &va : theDataStore.AssignsPerDay[day])
        {
            cAgent::find(va.first)->incDaysWorked(day);

            std::cout << va.first
                      << " assigned to " << va.second
                      << ", ";
        }
    }
    for(auto &t : theDataStore.Agents )
        std::cout << t.ID() 
            << " worked " << t.ActualDaysWorked()
            << "\n";
}
