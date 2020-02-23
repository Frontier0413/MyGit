#!/bin/bash

function f(){
    echo "your chocie is $1"
}

read -p "Please input your choice: " yn
until [ "$yn" == "Y" ] || [ "$yn" == "y" ] || [ "$yn" == "N" ] || [ "$yn" == "n" ]
do
    read -p "error, except input, Please input again:" yn
done
if [ test -ef "test" ];then
f $yn >> test
else
touch test
f $yn >>test
fi
exit