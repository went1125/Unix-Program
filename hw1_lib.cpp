#include "hw1_lib.h"

unordered_map<string, string> SOCKET_MAP;

using namespace std;
// function implement.

string convert_IPv4(const string& hexStr) {
	string ip_hex = hexStr.substr(0, 8);
	string port_hex = hexStr.substr(9);

	struct in_addr in;
	int ip_int = stol(ip_hex, NULL, 16);
	int port_int = stol(port_hex, NULL, 16);
	in.s_addr = (uint32_t)ip_int;

	char buffer[256];
	inet_ntop(AF_INET, &in, buffer, sizeof(buffer));
	string port_str = (port_int == 0) ? "*" : to_string(port_int);
	return string(buffer) + ':' + port_str;
}

string convert_IPv6(const string& hexStr) {
	string ip_hex = hexStr.substr(0, 32);
	string port_hex = hexStr.substr(33);

	struct in6_addr in;
	for(int i = 0, j = 0; i < 32; i+=8, ++j) {
		uint32_t ipv6_chunk = stol(ip_hex.substr(i, 8), NULL, 16);
		in.__in6_u.__u6_addr32[j] = ipv6_chunk;
	}
	int port_int = stol(port_hex, NULL, 16);

	char buffer[256];
	inet_ntop(AF_INET6, &in, buffer, sizeof(buffer));
	string port_str = (port_int == 0) ? "*" : to_string(port_int);
	return string(buffer) + ':' + port_str;
}

void print_index(const string& protocol) {
	cout << "List of " << protocol << " connections:" << endl;

	cout << "  Protocol" << '|' \
         << "        Local address"  << '|' \
         << "      Foreign address"  << '|' \
         << "      PID/Program name and arguments" << endl;

	cout << "----------" << '-' \
		 << "---------------------" << '-' \
		 << "---------------------" << '-' \
		 << "------------------------------------" << endl;
}

void print_content(fstream& file, const string& protocol ,int flag, const string& filter) {
	char buffer[256];
	file.getline(buffer, sizeof(buffer), '\n'); // dummy line.

	int idx = 0; // use idx to choose colums.
	bool match = false;
	regex_t reg;
	regmatch_t pmatch[1];
	regcomp(&reg, filter.c_str(), REG_EXTENDED);
	vector<string> content;

	while(file >> buffer) {
		if(idx == 0) {
			content.push_back(protocol);
			match = !regexec(&reg, protocol.c_str(), 1, pmatch, 0) ? true : match;
		}
		else if(idx == 1 || idx == 2) {
			string str_buf = string(buffer);
			string ip_content = (flag == 1) ? convert_IPv6(str_buf) : convert_IPv4(str_buf);
			content.push_back(ip_content);
		  	match = !regexec(&reg, ip_content.c_str(), 1, pmatch, 0) ? true : match;
		}
		else if(idx == 9) {
			content.push_back(SOCKET_MAP[string(buffer)]);
			match = !regexec(&reg, SOCKET_MAP[string(buffer)].c_str(), 1, pmatch, 0) ? true : match;
			if(filter == "\0" || match) {
				cout << setw(10) << content[0] << '|';
				cout << setw(21) << content[1] << '|';
				cout << setw(21) << content[2] << '|';
				cout << setw(36) << content[3] << endl;
			}
			match = false;
			file.getline(buffer, sizeof(buffer), '\n');
			content.clear();
			idx = 0;
			continue;
		}
		idx++;
	}
	regfree(&reg);
	file.close();
}

bool is_number(const string& str) {
    for(auto c : str) if(!isdigit(c)) return false;
    return true;
}

bool is_socket(const string& str) {
	if(str.length() < 6) return false;
	return str.substr(0, 6) == "socket";
}

string get_socket_inode(const string& str) {
	string temp = str.substr(0, str.length()-1);
	return temp.substr(8);
}

string get_program_info(const string& str) {
	string ret;
	char buffer[256];
	memset(buffer, 0, sizeof(buffer));
	fstream p_info;
	p_info.open(str, ios::in);
	p_info.getline(buffer, sizeof(buffer), '\0');
	ret = string(basename(buffer));
	while(p_info.getline(buffer, sizeof(buffer), '\0')) {
		ret += ' ';
		ret += string(buffer);
	}
	p_info.close();
	return ret;
}

void create_proc_socket_hash() {
    DIR* PROC_DIR = opendir(PROC_INFO_PATH.c_str());

    if(!PROC_DIR) {
        cout << PROC_INFO_PATH << " is not exist or not a director." << endl;
        return ;
    }

    struct dirent* pid_path;
    while((pid_path = readdir(PROC_DIR)) != NULL) {
        if(!is_number(pid_path->d_name)) continue;

        string fd_path = PROC_INFO_PATH + '/' + pid_path->d_name + "/fd";

		    DIR* FD_DIR = opendir(fd_path.c_str());
        if(!FD_DIR) {
            cout << fd_path << " is not exist or not a directory." << endl;
            return ;
        }

        struct dirent* fd;
        while((fd = readdir(FD_DIR)) != NULL) {
            if(!is_number(fd->d_name)) continue;

            string fd_file = fd_path + '/' +  fd->d_name;

			      char soft_link[256];
			      memset(soft_link, 0, sizeof(soft_link));
            readlink(fd_file.c_str(), soft_link, sizeof(soft_link));

      			string soft_link_str = string(soft_link);
      			if(is_socket(soft_link_str)) {
      				string inode = get_socket_inode(soft_link_str);
      				string program_info = get_program_info(PROC_INFO_PATH + "/" + pid_path->d_name + "/cmdline");
      				SOCKET_MAP[inode] = string(pid_path->d_name) + '/' + program_info;
      			}
      }
        closedir(FD_DIR);
    }
    closedir(PROC_DIR);
}
