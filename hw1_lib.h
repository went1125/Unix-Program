#include <dirent.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <string>
#include <cstdlib>
#include <vector>
#include <unistd.h>
#include <cstring>
#include <getopt.h>
#include <iomanip>
#include <libgen.h>
#include <regex.h>

using namespace std;

// Global variable.
const string TCP_INFO_PATH = "/proc/net/tcp";
const string UDP_INFO_PATH = "/proc/net/udp";
const string PROC_INFO_PATH = "/proc";
extern unordered_map<string, string> SOCKET_MAP;

//All need function.
string convert_IPv4(const string& hexStr);

string convert_IPv6(const string& hexStr);

void print_index(const string& protocol);

void print_content(fstream& file, const string& protocol ,int flag, const string& filter);

bool is_number(const string& str);

bool is_socket(const string& str);

string get_socket_inode(const string& str);

string get_program_info(const string& str);

void create_proc_socket_hash();
