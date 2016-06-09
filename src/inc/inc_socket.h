#ifdef _WIN32 //include libraries if the platform uses windows begin
#pragma comment(lib,"Ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>

#else

#include <sys/socket.h>
#include <netinet/in.h>
#endif //include libraries if the platform uses windows end
