build:
	g++ -std=c++11 -o project2.out src/main.cpp

tasks:
	./project2.out part11.tga circles.tga multiply layer1.tga

	./project2.out part12.tga layer1.tga flip

	./project2.out part13.tga layer1.tga subtract layer2.tga

clean:
	rm -f project2.out