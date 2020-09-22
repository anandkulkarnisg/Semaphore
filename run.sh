#!/bin/bash
for item in $(seq $1)
do
	./Example8.out
	echo -e "The iteration = $item is complete.\n"
  echo -e "Current timestamp is : " $(date)
done
