#ifndef PROCWATCH_HPP
#define PROCWATCH_HPP

#include <iostream> //cout,cerr
#include <fstream> // read from /proc
#include <string>
#include <sstream>
#include <vector>
#include <dirent.h>
#include <cctype>
#include <unistd.h> //sleep()
#include <iomanip> // output formatting (setw, left)
#include <csignal>   //SIGTERM handling
#include <syslog.h>  //system logging
#include <iomanip>

using namespace std;

struct ProcessInfo {
    string pid;
    string name;
    long long cpuTime; 
    long long cpuDiff; 
    long rss;          
};

struct Config {
    int interval = 2;
    int n = 5;
    bool crashTest = false;
};


void printStatic();
void getSysCpuTimes(long long &totalTime, long long &idleTime);
ProcessInfo getProcessInfo(string pid);
bool isNumeric(string str);
vector<ProcessInfo> getAllProcesses();
void sortTopN(vector<ProcessInfo>& arr);
Config readConfig(string path);
void signalHandler(int signum);

#endif