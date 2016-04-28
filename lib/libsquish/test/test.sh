#!/bin/sh
for file in ../images/*.png
do
	./squishpng $* "$file"
done

