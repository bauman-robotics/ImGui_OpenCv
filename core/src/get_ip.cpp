#include <ifaddrs.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>


using namespace std;

//==============================
unsigned char* get_current_ip() ;

//===================================================================================

unsigned char* get_current_ip() {
    struct ifaddrs *ifaddr, *ifa;
    int family, s;
    char host[NI_MAXHOST];
    unsigned char* ip_bytes = new unsigned char[4]; // Динамически выделяем память для IP-адреса

    // Получаем список интерфейсов
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return nullptr;
    }

    // Проходим по списку интерфейсов
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        family = ifa->ifa_addr->sa_family;

        // Проверяем, что это IPv4 адрес
        if (family == AF_INET) {
            s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            
            if (s != 0) {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                continue;
            }

            // Проверяем, что это не локальный адрес
            if (strcmp(host, "127.0.0.1") != 0) {
                // Преобразуем строку IP в массив байтов
                inet_pton(AF_INET, host, ip_bytes);
                break; // Выходим после первого найденного адреса
            }
        }
    }

    freeifaddrs(ifaddr);
    return ip_bytes;
}
//=====================================

