/// @brief A teacher == an agent that can be assigned to tasks
class cAgent
{
    std::string myID;
    std::vector<std::string> myTasks; // tasks ID that this agent can be assigned to
    int myWorkDaysMax;                // Specified in input - no explanation fo what it does
    int myWorkDaysActual[6];          // number of scheduled days worked in a week
    bool myAvailable[6];              // days of the week ( Sat = 0 ) when agent can be assigned

public:
    cAgent();
    cAgent(const std::string &id);

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
        int &daysWorked,
        int &lecturesToday,
        const std::string &task,
        int day);

    void incDaysWorked(int day)
    {
        myWorkDaysActual[day]++;
    }

    int ActualDaysWorked() const;

    int totalLectures() const;

    std::string ID() const
    {
        return myID;
    }

    static void displaySpec();
    static void displayWork();
    static void sort();

    static cAgent *find(const std::string &id);
};

class cSubject
{
    std::string myID;
    std::string myClass;
    int myWeeklyLectures;
    int myWeeklyUnassigned;

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
    bool parseWeekly(const std::string &line);

    void assign()
    {
        myWeeklyUnassigned--;
    }

    int WeeklyUnassigned() const
    {
        return myWeeklyUnassigned;
    }

    std::string ID() const
    {
        return myID;
    }
    int weeklyLectures() const
    {
        return myWeeklyLectures;
    }

    static void sort();
};

struct cAssign
{
    std::string Teacher;
    std::string Subject;

    cAssign(
        const std::string &t,
        const std::string &s)
        : Teacher(t),
          Subject(s)
    {
    }
};

struct sDataStore
{
    std::vector<cAgent> Agents;
    std::vector<cSubject> Subjects;
    std::vector<std::vector<std::string>> SubjectsPerDay;
    std::vector<std::vector<cAssign>> AssignsPerDay;
    std::vector<std::string> daynames{"Sat", "Sun", "Mon", "Tue", "Wed", "Thr"};
};

extern sDataStore theDataStore;

void readfile();

void AllocateSubjectsDays();

void AllocateTeachersToSubjects();

void displayAssigns();
