## Before using the program:
### If bluetooth doesn't work  
1. Block Bluetooth:  
```Shell
rfkill block bluetooth #to stop bluetooth
```

2. Unblock Bluetooth  
```Shell
rfkill unblock bluetooth #to start bluetooth  
```

### For permission error  
run `$ chmod 755 getMyBtAddr.sh`  

## Compile:  
### 1. To compile
use `make`

### 2. To delete all binary/executable files
use `make clean`

### 3. To run
use `./bin/main $(./getMyBtAddr.sh)`

## Functionality: Now support
### 1. scan devices
### 2. send data
### 3. receive data
### 4. threaded


## Notes
### 1. Devices Bluetooth MAC:
1) 00:04:4B:66:9F:3A
// the one Cathy has
2) 00:04:4B:65:BB:42
// the one Emma has