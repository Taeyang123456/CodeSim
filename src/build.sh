#!/bin/bash

#cur_dir=$(cd $(dirname $0) && pwd)

#echo "add current path "$cur_dir "to PATH"

#export PATH=$PATH:$cur_dir

#echo $PATH

exe="codesim"

target_path="/usr/bin"

mv ${exe} ${target_path}
