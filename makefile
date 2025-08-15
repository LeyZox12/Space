
CXXFLAGS = -std=c++23 -g -I"../../../SFML-3.0.0/include" -Iinclude -I"../../class"

final : main.o UIutils.o Ship.o Map.o
	g++ $(CXXFLAGS) UIutils.o Ship.o Map.o main.o -o Spaceiscool -L"../../../SFML-3.0.0/lib" -lsfml-graphics-d -lsfml-window-d -lsfml-system-d

main.o : main.cpp
	g++ $(CXXFLAGS) -c main.cpp 

UIutils.o : ../../class/UIutils.cpp
	g++ $(CXXFLAGS) -c "../../class/UIutils.cpp"

Ship.o : src/Ship.cpp
	g++ $(CXXFLAGS) -c "src/Ship.cpp"

Map.o : src/Map.cpp
	g++ $(CXXFLAGS) -c "src/Map.cpp"

clean : main.o
	del main.o

