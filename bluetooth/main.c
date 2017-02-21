#include "lib/config.h"
#include "lib/scan.h"

void print_menu();

int main(int argc, char **argv)
{
	inquiry_info *ii = NULL;
	int max_rsp = 255, num_rsp = -1;
	int dev_id, sock;
	char addr[19] = { 0 };
	char name[248] = { 0 };

	char* input = (char*)malloc(sizeof(char) * 10);
	int choice = 1;

	while (choice != 0) {
		if (choice != 1) {
			print_menu();
			scanf("%s", input);

			choice = atoi(input);
		}

		switch (choice)
		{
		case 1:
			if (bt_scan(&ii, max_rsp, &num_rsp, &dev_id, &sock)) {
				exit(1);
			}

			int i;
			for (i = 0; i < num_rsp; i++) {
				ba2str(&(ii + i)->bdaddr, addr);
				memset(name, 0, sizeof(name));

				if (hci_read_remote_name(sock, &(ii + i)->bdaddr, sizeof(name),
					name, 0) < 0) {
					printf("%s  [unknown]\n", addr);
				}
				else {
					printf("%s  %s\n", addr, name);
				}
			}
			break;
		case 2:
			if (ii == NULL) {
				printf("Should search before sending data\n");
				choice = 1;
				continue;
			}
			break;
		case 3:
			break;
		default:
			break;
		}
	}

	free(ii);
	free(input);
	close(sock); // a system call that is used to close an open file descriptor
	return 0;
}

void print_menu() {
	printf("What to do?\n");
	printf("1. Search for Bluetooth devices\n");
	printf("2. Send data to other devices\n");
	printf("3. Wait for data from other devices\n");
	printf("4. Exit\n");
	printf("Enter your choice (any invalid input will exit the program: ");
}