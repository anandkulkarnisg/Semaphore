#!/bin/bash
for item in $(seq 100)
do
	./Example8.out
	echo -e "The iteration = $item is complete.\n"
done
