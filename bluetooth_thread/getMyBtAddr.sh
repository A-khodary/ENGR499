#!/bin/bash
# My first script

hcitool dev | grep -o "[[:xdigit:]:]\{11,17\}"
