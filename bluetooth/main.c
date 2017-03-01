#include "lib/config.h"
#include "lib/scan.h"
#include "lib/rfcomm.h"

void print_menu();
int prompt_device(char*, int);

int main(int argc, char **argv)
{
	inquiry_info *ii = NULL;
	int max_rsp = 255, num_rsp = -1;
	int dev_id, sock;
	char addr[BT_ADDR_SIZE] = { 0 };
	char name[248] = { 0 };

	char* input = (char*)malloc(sizeof(char) * 10);
	int choice = -1;

	int device_choice = 0;
	char* dest = malloc(sizeof(char) * BT_ADDR_SIZE);

	while (choice != 0) {
		if (choice == -1) {
			print_menu();
			scanf("%s", input);

			choice = atoi(input);
		}

		switch (choice)
		{
		case 1:
			if (bt_scan(&ii, max_rsp, &num_rsp, &dev_id, &sock) != 0) {
				exit(1);
			}

			int i;
			for (i = 0; i < num_rsp; i++) {
				// convert to address
				ba2str(&(ii + i)->bdaddr, addr);
				// initialize name
				memset(name, 0, sizeof(name));

				if (hci_read_remote_name(sock, &(ii + i)->bdaddr, sizeof(name),
					name, 0) < 0) {
					printf("\t%i. %s  [unknown]\n", i + 1, addr);
				}
				else {
					printf("\t%i. %s  %s\n", i + 1, addr, name);
				}
			}
			choice = -1;

			device_choice = prompt_device(input, num_rsp);
			if (device_choice <= 0) {
				printf("No available device\n");
			}
			else {
				ba2str(&(ii + device_choice - 1)->bdaddr, dest);
				printf("%s\n", dest);
			}
			break;
		case 2:
			if (ii == NULL || num_rsp == -1) {
				printf("Should search devices first\n");
				choice = 1;
				break;
			}
			if (device_choice <= 0) {
				device_choice = prompt_device(input, num_rsp);
			}
			if (rfcomm_send(dest) != 0) {
				perror("Error");
			}
			break;
		case 3:
			if (rfcomm_send(dest) != 0) {
				perror("Error");
			}
			break;
		case -1:
			break;
		default:
			choice = 0;
			break;
		}
	}

	free(ii);
	free(input);
	free(dest);
	close(sock); // a system call that is used to close an open file descriptor
	return 0;
}

void print_menu() {
	printf("What to do?\n");
	printf("1. Search for Bluetooth devices\n");
	printf("2. Send data to other devices\n");
	printf("3. Wait for data from other devices\n");
	printf("4. Exit\n");
	printf("Enter your choice (all other input is considered exit): ");
}


int prompt_device(char* input, int max_num) {
	if (max_num <= 0) {
		return -1;
	}

	int choice = 0;

	do {
		printf("Select a device: ");
		scanf("%s", input);

		choice = atoi(input);
	} while (choice == 0 || choice > max_num);

	return choice;
}