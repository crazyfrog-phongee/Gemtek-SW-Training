#!/bin/bash

if [ $# -eq 0 ] # $# is number of arguments passed to script
then
  echo "Error: Please pass the arguments through command line."
  echo "Usage: ./calculator.sh 1 + 2"
elif [ $# -eq 3 ]
then
  case $2 in
    +) sum=`expr "scale=2; $1 + $3" | bc`
      echo "$1 + $3 = $sum" ;;
    -) sub=`expr "scale=2; $1 - $3" | bc`
      echo "$1 - $3 = $sub" ;;
    x) mul=`expr "scale=2; $1 "*" $3" | bc`
      echo "$1 x $3 = $mul" ;;
    /) div=`expr "scale=2; $1 / $3" | bc`
      echo "$1 / $3 = $div" ;;
  esac
else
  echo "Error: Please pass 3 arguments."
  echo "Usage: ./calculator.sh 1 + 2"
fi