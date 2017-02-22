1. to compile. Use

	`make`

2. to delete all binary/executable files:

	`make clean`

3. send data use:

	`./bin/rfcomm-send`
	
4. receive data use:

	`./bin/rfcomm-receive`
	
Notice:  
1. in rfcomm-client.c line 70, change the string of char array dest to the bluetooth address of the device which will receive the data
2. I believe we should run rfcomm-receive before rfcomm-send, but this is not tested
