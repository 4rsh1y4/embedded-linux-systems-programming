#include "procwatch.hpp"

using namespace std;

void printStatic(){
    string out;

    // kernel version:
    ifstream versionFile("/proc/version");
    if(versionFile.is_open()){
        getline(versionFile, out);
        // outputting only the first part of info.
        // as the full is like:
        //Linux version 7.0.0-15-generic (buildd@lcy02-amd64-048) (x86_64-linux-gnu-gcc (Ubuntu 15.2.0-16ubuntu1) 15.2.0, GNU ld (GNU Binutils for Ubuntu) 2.46) #15-Ubuntu SMP PREEMPT_DYNAMIC Wed Apr 22 16:06:43 UTC 2026
//rsh414@rsh414
        cout << "Kernel Version: " << out.substr(0, out.find(" (")) << "\n";
        versionFile.close();  
    }

    //Load Average
    // 1, 5, 15 past minutes
    ifstream loadAvgFile("/proc/loadavg");
    if(loadAvgFile.is_open()){
        double load1, load5, load15;
        loadAvgFile >> load1 >> load5 >> load15;
        cout << "Load Average (1,5,15)minutes: " << load1 << " | " << load5 << " | " << load15 << "\n";
        loadAvgFile.close();
    }

    //processor count
    ifstream cpuinfoFile("/proc/cpuinfo");
    int cores = 0;
    if (cpuinfoFile.is_open()) {
        while (getline(cpuinfoFile, out))
            if (out.find("processor") == 0)
                cores++;
        cout << "CPU Cores: " << cores << "\n";
        cpuinfoFile.close();
    }

    //memory info
    ifstream meminfoFile("/proc/meminfo");
    long memTotal = 0, memFree = 0, memAvailable = 0;
    if (meminfoFile.is_open()) {
        string key;
        long value;
        string unit;
        while (meminfoFile >> key >> value >> unit) {
            if (key == "MemTotal:") memTotal = value;
            else if (key == "MemFree:") memFree = value;
            else if (key == "MemAvailable:") memAvailable = value;
        }
    meminfoFile.close();

    long memUsed = memTotal - memAvailable;
    
    cout << "Memory (kB) -> Total: " << memTotal 
         << " | Free: " << memFree 
         << " | Available: " << memAvailable 
         << " | Used: " << memUsed << "\n";
    }
}

// total and idle Cpu times/ overall CPU usage
void getSysCpuTimes(long long &totalTime, long long &idleTime) {
    ifstream statFile("/proc/stat");
    if (statFile.is_open()) {
        string line;
        getline(statFile, line);
        statFile.close();

        istringstream iss(line);
        string cpuLabel;
        long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;

        iss >> cpuLabel >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;

        idleTime = idle + iowait;
        totalTime = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
    }
}

ProcessInfo getProcessInfo(string pid) {
    ProcessInfo p;
    p.pid = pid;
    p.cpuTime = 0;
    p.cpuDiff = 0;
    p.rss = 0;

    //process name
    string commPath = "/proc/" + pid + "/comm";
    ifstream commFile(commPath);
    if (commFile.is_open()) {
        getline(commFile, p.name);
        commFile.close();
    }

    // cpu time
    string statPath = "/proc/" + pid + "/stat";
    ifstream statFile(statPath);
    if (statFile.is_open()) {
        string line;
        getline(statFile, line);
        statFile.close();

        //finding the last ) skipping program name
        size_t rparen = line.find_last_of(')');
        if (rparen != string::npos) {
            string rest = line.substr(rparen + 2); 
            istringstream iss(rest);
            
            string dummy;
            long long utime = 0, stime = 0;
            
            //skip 11 words to reac utime and stime
            int i = 1;
            while (i <= 11) {
                iss >> dummy;
                i++;
            }
            iss >> utime >> stime;
            p.cpuTime = utime + stime;
        }
    }

    //rss memory from statys
    string statusPath = "/proc/" + pid + "/status";
    ifstream statusFile(statusPath);
    if (statusFile.is_open()) {
        string key;
        long value;
        string unit;
        string line;
        while (getline(statusFile, line)) {
            istringstream iss(line);
            iss >> key >> value >> unit;
            if (key == "VmRSS:") {
                p.rss = value;
                // break loop as soon as Vmrss is read
                break; 
            }
        }
        statusFile.close();
    }

    return p;
}

// check if string is numeric (for pids)
bool isNumeric(string str) {
    int i = 0;
    // casting length to int to avoid unsigned comparison warning
    while (i < (int)str.length()) {
        if (!isdigit(str[i])) return false;
        i++;
    }
    return true;
}

// read all running processes from /proc
vector<ProcessInfo> getAllProcesses() {
    vector<ProcessInfo> procs;
    DIR* dir = opendir("/proc");
    if (dir == nullptr) return procs;

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        string dirName = entry->d_name;
        if (entry->d_type == DT_DIR && isNumeric(dirName)) {
            ProcessInfo p = getProcessInfo(dirName);
            if (p.name != "") { 
                procs.push_back(p);
            }
        }
    }
    closedir(dir);
    return procs;
}

// sort based on cpuDiff
void sortTopN(vector<ProcessInfo>& arr) {
    // casting size to int to avoid unsigned comparison warning
    int n = (int)arr.size();
    int i = 0;
    while (i < n - 1) {
        int j = 0;
        while (j < n - i - 1) {
            if (arr[j].cpuDiff < arr[j + 1].cpuDiff) {
                // swap
                ProcessInfo temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
            j++;
        }
        i++;
    }
}

Config readConfig(string path) {
    Config cfg;
    ifstream file(path);
    if (!file.is_open()) {
        cerr << "Error: Could not open config file: " << path << endl;
        return cfg;
    }
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            size_t delim = line.find('=');
            if (delim != string::npos) {
                string key = line.substr(0, delim);
                string val = line.substr(delim + 1);
                if (key == "INTERVAL") cfg.interval = stoi(val);
                else if (key == "N") cfg.n = stoi(val);
                else if (key == "CRASH_TEST") cfg.crashTest = (val == "1");
            }
        }
        file.close();
    }
    return cfg;
}



bool keepRunning = true;
void signalHandler(int signum) {
    if (signum == SIGTERM) keepRunning = false;
}

int main (int argc, char* argv[]){
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <interactive | background [config_file]>\n";
        return 1;
    }
    string mode = argv[1];

    if (mode == "interactive") {
        long long previousTotal =0;
        long long previousIdle = 0;
        
        vector<ProcessInfo> prevProcs; 
        int N = 5;

        while (true) {
            system("clear");
            
            printStatic();

            long long currentTotal =0;
            long long currentIdle = 0;

            getSysCpuTimes(currentTotal, currentIdle);

            if (previousTotal != 0) {
                long long totalDiff = currentTotal - previousTotal;
                long long idleDiff = currentIdle - previousIdle;

                double cpuUsage = 100.0 * (totalDiff - idleDiff) / totalDiff;
                
                cout << "Total CPU Usage: " << cpuUsage << " %\n";
            }
            previousTotal = currentTotal;
            previousIdle = currentIdle;

            
            // read current processes
            vector<ProcessInfo> currentProcs = getAllProcesses();
            
            // calculate diff if we have previous data
            if (prevProcs.size() > 0) {
                vector<ProcessInfo> diffProcs;
                int i = 0;
                // casting size to int
                while (i < (int)currentProcs.size()) {
                    long long prevCpuTime = 0;
                    int j = 0;
                    
                    // find matching pid in prevProcs (casting size to int)
                    while (j < (int)prevProcs.size()) {
                        if (prevProcs[j].pid == currentProcs[i].pid) {
                            prevCpuTime = prevProcs[j].cpuTime;
                            break;
                        }
                        j++;
                    }

                    if (prevCpuTime != 0) {
                        currentProcs[i].cpuDiff = currentProcs[i].cpuTime - prevCpuTime;
                        diffProcs.push_back(currentProcs[i]);
                    }
                    i++;
                }

                sortTopN(diffProcs);
                // output top n
                cout << "\n--- Top " << N << " Processes ---\n";
                // set fixed width for each column and align left
                cout << left << setw(10) << "PID" 
                     << setw(20) << "Name" 
                     << setw(15) << "CPU Activity" 
                     << "RSS(kB)\n";
                
                int k = 0;
                // casting size to int
                while (k < N && k < (int)diffProcs.size()) {
                    cout << left << setw(10) << diffProcs[k].pid 
                         << setw(30) << diffProcs[k].name 
                         << setw(15) << diffProcs[k].cpuDiff 
                         << diffProcs[k].rss << "\n";
                    k++;
                }
            }
            
            prevProcs = currentProcs;
            
            sleep(2);
        }
    } 
    else if (mode == "background") {
        // no config path added
        if (argc < 3) {
            cerr << "Error: Background mode requires a config file.\n";
            return 1;
        }
        // read config file
        Config cfg = readConfig(argv[2]);
        
        // handle sigterm
        signal(SIGTERM, signalHandler);
        
        // start system log
        openlog("procwatch", LOG_PID | LOG_CONS, LOG_DAEMON);
        syslog(LOG_INFO, "Service started. Interval: %d, N: %d", cfg.interval, cfg.n);

        // holder for previous processes
        vector<ProcessInfo> prevProcs; 

        while (keepRunning) {
            // intentional error for systemd testing
            if (cfg.crashTest) { 
                syslog(LOG_ERR, "Intentional crash!");
                closelog();
                return 1;
            }

            // read current processes
            vector<ProcessInfo> currentProcs = getAllProcesses();
            
            // calculate diff if we have previous data
            if (prevProcs.size() > 0) {
                vector<ProcessInfo> diffProcs;
                int i = 0;
                // casting size to int
                while (i < (int)currentProcs.size()) {
                    long long prevCpuTime = 0;
                    int j = 0;
                    
                    // find matching pid in prevProcs (casting size to int)
                    while (j < (int)prevProcs.size()) {
                        if (prevProcs[j].pid == currentProcs[i].pid) {
                            prevCpuTime = prevProcs[j].cpuTime;
                            break;
                        }
                        j++;
                    }

                    if (prevCpuTime != 0) {
                        currentProcs[i].cpuDiff = currentProcs[i].cpuTime - prevCpuTime;
                        diffProcs.push_back(currentProcs[i]);
                    }
                    i++;
                }

                // sort based on cpuDiff
                sortTopN(diffProcs);

                // log top n processes
                syslog(LOG_INFO, "\n\n\n--- Top %d Processes ---", cfg.n);
                for(int k = 0; k < cfg.n && k < (int)diffProcs.size(); k++) {
                    syslog(LOG_INFO, "PID: %s | Name: %s | Activity: %lld | RSS: %ld kB", 
                           diffProcs[k].pid.c_str(), diffProcs[k].name.c_str(), 
                           diffProcs[k].cpuDiff, diffProcs[k].rss);
                }

            }
            
            // save for next round
            prevProcs = currentProcs;
            sleep(cfg.interval);
        }
        
        syslog(LOG_INFO, "Service stopped gracefully.");
        closelog();
    }
    
    else {
        cerr << "Invalid argument! Use 'interactive' or 'background'.\n";
        return 1;
    }

    return 0;

}