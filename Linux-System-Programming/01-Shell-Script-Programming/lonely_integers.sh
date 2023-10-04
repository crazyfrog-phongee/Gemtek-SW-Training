#!/bin/bash

# There are N integers in an array A. All but one integer occur in pairs. Your task is to find the number that occurs only once

# Solution: 
# "arr=($(cat))" in bash will read data from input and assign it to an array named "arr".
# The "cat" command is used to read data from stdin (standard input).
# 1. `echo "${arr[@]}"`: Prints all elements in array `arr`.
# 2. `tr ' ' '\n'`: Replaces spaces with newlines, converting from a list to a vertical list.
# 3. `sort`: Sort elements in lexicographic order.
# 4. `uniq -u`: Filter out only non-duplicate elements.
# 5. `tr '\n' ' ``: Replace newlines with spaces, converting from a vertical list to a horizontal list.

read
arr=($(cat)) 
echo "${arr[@]}" | tr ' ' '\n' |sort | uniq -u | tr '\n' ' '