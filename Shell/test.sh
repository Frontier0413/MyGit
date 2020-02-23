#!/bin/bash

function f(){
    echo "your chocie is $1"
}

read -p "Please input your choice: " yn
until [ "$yn" == "Y" ] || [ "$yn" == "y" ] || [ "$yn" == "N" ] || [ "$yn" == "n" ]
do
    read -p "error, except input, Please input again:" yn
done
f $yn
exit