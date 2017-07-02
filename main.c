#define _DEFAULT_SOURCE
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h> 

#define DPMASTER_PORT 27950

int main(int argc, char **argv) {
	struct sockaddr_in si_other;
	int sock = -1;
	socklen_t slen;
	unsigned char data[2048];
	unsigned char *data_ptr;
	int data_len;
	if (argc != 2) {
	}
    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		perror("socket");
		goto finish;
	}
    memset(&si_other, 0, sizeof(si_other));
	if (!inet_aton(argv[1], &si_other.sin_addr)) {
		perror("sendto");
		goto finish;
	}
    si_other.sin_family = AF_INET;
	si_other.sin_port = htons(DPMASTER_PORT);
	if (sendto(sock, "\377\377\377\377getservers Nexuiz 3 empty full", 34, 0, (struct sockaddr *)&si_other, sizeof(si_other)) < 0) {
		perror("sendto");
		goto finish;
	}
	for (slen = sizeof(si_other); (data_len = recvfrom(sock, data, 2048, 0, (struct sockaddr *)&si_other, &slen)) >= 0; slen = sizeof(si_other)) {
		slen = sizeof(si_other);
		data_ptr = data;
		if (data_len < 22 || strncmp((char *)data_ptr, "\377\377\377\377getserversResponse", 22))
			continue;

		data_len -= 22;
		data_ptr += 22;
		while (data_len > 7 && data_ptr[0] == '\\') {
			printf("%i.%i.%i.%i:%i\n", data_ptr[1], data_ptr[2], data_ptr[3], data_ptr[4], data_ptr[5] * 256 + data_ptr[6]);
			data_len -= 7;
			data_ptr += 7;
		}
	}
	perror("recvfrom");
finish:
	if (sock != -1)
		close(sock);

	return 0;
}
