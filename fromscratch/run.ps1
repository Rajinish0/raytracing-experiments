g++ main.cpp -Wall -o ./main 

# behold my mess of "if" statements

if ($LASTEXITCODE -eq 0){
	./main
}
if ($LASTEXITCODE -eq 0){
	echo "Converting ppm to png."
	py script.py out.ppm 
}
if ($LASTEXITCODE -eq 0){
	echo "Done."
	./out.png
}
