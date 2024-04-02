/// @brief A teacher == an agent that can be assigned to tasks
class cAgent
{
    std::string myID;
    std::vector<std::string> myTasks; // tasks ID that this agent can be assigned to
    int myWorkDaysMax;                // Specified in input - no explanation fo what it does
    int myWorkDaysActual;             // number of scheduled days worked in a week
    bool myAvailable[6];              // days of the week ( Sat = 0 ) when agent can be assigned

public:
    cAgent(const std::string &id)
        : myID(id),
          myWorkDaysActual(0)
    {
        for (int d = 0; d < 6; d++)
            myAvailable[d] = true;
    }

    /// @brief parse unavailable days
    /// @param line
    /// @return true if successful

    bool parseUnavailableDays(
        const std::string &line);

    bool parseSubjects(
        const std::string &line);

    bool parseWorkdays(
        const std::string &line);

    /// @brief true if agent can be assigned to a task
    /// @param task
    /// @param day of week ( Sat = 0 )
    /// @return true if agent can be assigned to a task

    bool cando(
        const std::string &task,
        int day);

    void incDaysWorked()
    {
        myWorkDaysActual++;
    }

    std::string ID() const
    {
        return myID;
    }

    static void display();

    static cAgent *find(const std::string &id);
};

class cSubject
{
    std::string myID;
    std::string myClass;
    int myWeeklyLectures;

public:
    void ID(const std::string &id)
    {
        myID = id;
    }
    bool parseClass(const std::string &line)
    {
        if (line.find("class:"))
            return false;
        int p = line.find(":");
        myClass = line.substr(p + 2);
        return true;
    }
    bool parseWeekly(const std::string &line)
    {
        if (line.find("weeklyLectures:"))
            return false;
        int p = line.find(":");
        myWeeklyLectures = atoi(line.substr(p + 2).c_str());
        return true;
    }
};

struct sDataStore
{
    std::set<std::string> Tasks;
    std::vector<cAgent> Agents;
    std::vector<cSubject> Subjects;
};

extern sDataStore theDataStore;

void readfile();

void allocateMaxFlow();
