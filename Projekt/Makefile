all: dynablaster
	# Done
dynablaster: plansza.cpp
	g++ -g --std=c++11 plansza.cpp -o warcaby -I/usr/local/include/ -L/usr/local/lib -lallegro -lallegro_main -lallegro_primitives -lallegro_image

clean:
	rm -rf *.o warcaby
