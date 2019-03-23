#include "hw1_lib.h"

using namespace std;

struct option long_options[] = {
	{"tcp", 0, NULL, 't'},
	{"udp", 0, NULL, 'u'}
};

int main(int argc, char* argv[]) {
	create_proc_socket_hash();

	char c;
	fstream tcp_file, tcp6_file, udp_file, udp6_file;
	bool tcp_flag = false, udp_flag = false;

	while((c = getopt_long(argc, argv, "tu", long_options, NULL)) != -1) {
		switch(c) {
			case 't':
				tcp_flag = true;
				break;

			case 'u':
                udp_flag = true;
				break;

			case '?':
				cout << "Unknown argument." << endl;
				break;
		}
	}

	const string filter = (optind < argc) ? argv[optind] : "\0";
	bool have_argument = tcp_flag | udp_flag;

	if(tcp_flag || !have_argument) {
		tcp_file.open(TCP_INFO_PATH, ios::in);
		tcp6_file.open(TCP_INFO_PATH + '6', ios::in);
		print_index("TCP");
		print_content(tcp_file, "tcp", 0, filter);
		print_content(tcp6_file, "tcp6", 1, filter);
	}
	if(udp_flag || !have_argument) {
		udp_file.open(UDP_INFO_PATH, ios::in);
    	udp6_file.open(UDP_INFO_PATH + '6', ios::in);
    	print_index("UDP");
    	print_content(udp_file, "udp", 0, filter);
    	print_content(udp6_file, "udp6", 1, filter);
	}
	return 0;
}
