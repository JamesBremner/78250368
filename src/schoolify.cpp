#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include "GraphTheory.h"

#include "schoolify.h"

cAgent::cAgent() {}
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
    std::pair<int, int> &workload,
    const std::string &task,
    int day)
{
    workload.first = ActualDaysWorked();
    workload.second = myWorkDaysActual[day];

    if (workload.first >= myWorkDaysMax &&
        workload.second == 0)
        return false;

    if (!myAvailable[day])
        return false;

    if (std::find(
            myTasks.begin(), myTasks.end(),
            task) == myTasks.end())
        return false;

    // std::cout << "cando " << myID
    //     << " " << day
    //     << " ";
    // for( int i : myWorkDaysActual )
    //     std::cout << i << " ";
    // std::cout << "\n";

    return true;
}

int cAgent::ActualDaysWorked() const
{
    int ret = 0;
    for (int day = 0; day < 6; day++)
        if (myWorkDaysActual[day] > 0)
            ret++;
    return ret;
}
int cAgent::totalLectures() const
{
    int ret = 0;
    for (int day = 0; day < 6; day++)
        ret += myWorkDaysActual[day];
    return ret;
}

void cAgent::displaySpec()
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
void cAgent::sort()
{
    std::sort(
        theDataStore.Agents.begin(), theDataStore.Agents.end(),
        [](const cAgent &a, const cAgent &b)
        {
            return a.myID < b.myID;
        });
}
void cAgent::displayWork()
{
    for (auto &a : theDataStore.Agents)
    {
        std::cout << a.myID
                  << " works " << a.ActualDaysWorked()
                  << " days, " << a.totalLectures()
                  << " total lectures\n";
    }
}
bool cSubject::parseWeekly(const std::string &line)
{
    if (line.find("weeklyLectures:") == -1)
        return false;
    int p = line.find(":");
    myWeeklyLectures = atoi(line.substr(p + 2).c_str());
    myWeeklyUnassigned = myWeeklyLectures;
    return true;
}

void cSubject::sort()
{
    std::sort(
        theDataStore.Subjects.begin(), theDataStore.Subjects.end(),
        [](const cSubject &a, const cSubject &b)
        {
            return a.myID < b.myID;
        });
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

void AllocateSubjectsDays()
{
    const int maxSubjectPerDay = 4;

    theDataStore.SubjectsPerDay.clear();
    theDataStore.SubjectsPerDay.resize(6, std::vector<std::string>(0));

    // loop over the subjects
    for (auto iterS = theDataStore.Subjects.begin();
         iterS != theDataStore.Subjects.end();
         iterS++)
    {
        // loop until all lectures in this subject are assigned to a day
        while (iterS->WeeklyUnassigned())
        {
            // round robin: assign lectures to day with fewest lectures so far
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

            // assign up to maximum number of lectures
            int lmax = iterS->WeeklyUnassigned();
            if (lmax > maxSubjectPerDay)
                lmax = maxSubjectPerDay;
            for (int l = 0; l < lmax; l++)
            {
                theDataStore.SubjectsPerDay[minday].push_back(iterS->ID());
                iterS->assign();
            }
        }
    }

    // std::cout << "\nSubjects to days\n";
    // for (int day = 0; day < 6; day++)
    // {
    //     std::cout
    //         << theDataStore.daynames[day] << " ";
    //     for (auto id : theDataStore.SubjectsPerDay[day])
    //     {
    //         std::cout << id << " ";
    //     }
    //     std::cout << "\n";
    // }
}

void cAssign::TeachersToSubjects()
{
    theDataStore.AssignsPerDay.clear();
    theDataStore.AssignsPerDay.resize(6);

    // loop over the days
    for (int day = 0; day < 6; day++)
    {
        // loop over the subjects assigned to this day
        std::vector<std::string> vUnassigned;
        for (auto &s : theDataStore.SubjectsPerDay[day])
        {
            // find teacher with least workload that can be assigned
            cAgent *pbest;
            std::pair<int, int> bestWorkload;
            bestWorkload.first = INT_MAX;
            bestWorkload.second = INT_MAX;
            for (auto &t : theDataStore.Agents)
            {
                std::pair<int, int> workload;
                if (!t.cando(
                        workload,
                        s, day))
                    continue;
                if (workload.first >= bestWorkload.first)
                    continue;
                if (workload.second >= bestWorkload.second)
                    continue;
                bestWorkload = workload;
                pbest = &t;
            }
            if (bestWorkload.first == INT_MAX)
            {
                // no teacher could be assigned
                vUnassigned.push_back(s);
                continue;
            }

            // assign the teacher with the least workload
            theDataStore.AssignsPerDay[day].emplace_back(
                pbest->ID(), s);

            pbest->incDaysWorked(day);
        }

        // if possible, postpone to the next day
        // subjects that were not assigned
        if (vUnassigned.size())
        {
            // std::cout << "\nUnassigned: ";
            // for (auto &s : vUnassigned)
            //     std::cout << s << " ";
            // std::cout << "\n";

            if (day < 5)
            {
                theDataStore.SubjectsPerDay[day + 1].insert(
                    theDataStore.SubjectsPerDay[day + 1].begin(),
                    vUnassigned.begin(), vUnassigned.end());
            } else {
                std::cout << "Unassigned: ";
                for( auto& s : vUnassigned )
                    std::cout << s << ", ";
                std::cout << "\n";
            }
        }
    }

    cAssign::display();
}
void cAssign::display()
{
    for (int day = 0; day < 6; day++)
    {
        std::cout << "\n"
                  << theDataStore.daynames[day] << "\n";
        int c = 0;
        for (auto &va : theDataStore.AssignsPerDay[day])
        {
            if (c == 4)
            {
                std::cout << "\n";
                c = 0;
            }
            std::cout << va.Teacher
                      << " assigned " << va.Subject
                      << ", ";
            c++;
        }
    }
}
