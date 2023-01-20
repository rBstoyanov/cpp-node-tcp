#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "json.hpp"
using json = nlohmann::json;

// get working dir
const char *get_wd() {
  static char buf[1024];
  ssize_t len = sizeof(buf);
  memset((void*)buf, 0, 1024);
  readlink("/proc/self/exe", buf, len); // check err?
  char *cp = strrchr((char*)&buf, (int)'/') + 1; // +1 to skip last '/'
  *cp=0; // terminate string. strip everything from last '/' (filename) so we get working dir
  return buf;
}

int main(int argc, char **argv) {
  int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (3 != argc) {
    printf("use: %s <address> <port>\n", argv[0]);
    return 1;
  }

  char *host = argv[1];
  uint16_t port = atoi(argv[2]);

  struct sockaddr_in serv_addr;

  if (inet_aton(host, &serv_addr.sin_addr) == 0) {
    fprintf(stderr, "Invalid address\n");
    printf("cannot parse address!\n");
    return -1;
  }
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);

  printf("connect to %s:%d\n", host, port);

  const char* wd = get_wd(); // working dir
  const char *username = getlogin(); // username
  uid_t uid=getuid(), euid=geteuid();
  bool admin = 0==uid || 0==euid; // true = root
  // to test admin - 
  // su postgres -c "./client 127.0.0.1 10000"

  // this json.hpp is slow to compile, but its easy
  json js;
  js["filepath"] = wd;
  js["is_admin"] = admin;
  js["username"] = username;

  std::string serialized_str = js.dump();
  const char *serialized = serialized_str.c_str();

  while (true) {
    int result = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (result == -1) {
      printf("connec() failed.. sleeping \n");
      sleep(30);
    } else {
      int wr = write(sock, serialized, strlen(serialized));
      printf("nodehs server online, wrote %d bytes.. bye\n", wr);
      exit(0);
    }
  }
}
