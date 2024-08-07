#!/bin/bash

g++ main.cpp -o ./main 

if ($LASTEXITCODE -eq 0){
	./main
	echo "Converting ppm to png."
	py script.py out.ppm 
	if ($LASTEXITCODE -eq 0){
		echo "Done."
		./out.png
	}
}
