#ifndef StatusUpdate__H
#define StatusUpdate__H

#include <vector>
#include <map>
#include <cstddef>

class StatusUpdate {
    private:
        std::vector<int> runId_start;
        std::vector<int> runId_end;
        std::map<int, std::vector<bool> >  towStat; // tower statuses 
        int* runId;
        ptrdiff_t array_size;
    public:
        StatusUpdate(const char*, int*);
        bool is_good(int towId);
};

#endif
