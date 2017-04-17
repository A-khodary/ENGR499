#!/bin/bash
# Returns the bluetooth MAC address of this device

hcitool dev | grep -o "[[:xdigit:]:]\{11,17\}"
