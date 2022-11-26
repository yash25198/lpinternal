#!/bin/sh
regEx=".*\.txt"
echo "" > $1

for i in *;
do
  a=$( echo $i | grep -E $regEx );
  if [ "$a" != "" -a "$a" != $1 ];
  then
    echo $a >> $1
    fi;
done;
