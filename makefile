
CXXFLAGS = -std=c++23 -g -I"../../../SFML-3.0.0/include" -Iinclude -I"../../class/PointEngine/include"

final : main.o UIutils.o Ship.o Map.o Planet.o Player.o PointEngine.o Element.o Point.o Rectangle.o PhysicConstraint.o PointEngine.o
	g++ $(CXXFLAGS) Point.o Rectangle.o PhysicConstraint.o PointEngine.o Element.o UIutils.o Ship.o Map.o Planet.o Player.o main.o -o Spaceiscool -L"../../../SFML-3.0.0/lib" -lsfml-graphics-d -lsfml-window-d -lsfml-system-d

main.o : main.cpp
	g++ $(CXXFLAGS) -c main.cpp 

Element.o : src/Element.cpp
	g++ $(CXXFLAGS) -c src/Element.cpp

UIutils.o : ../../class/UIutils.cpp
	g++ $(CXXFLAGS) -c "../../class/UIutils.cpp"

Ship.o : src/Ship.cpp
	g++ $(CXXFLAGS) -c "src/Ship.cpp"

Map.o : src/Map.cpp
	g++ $(CXXFLAGS) -c "src/Map.cpp"

Planet.o : src/Planet.cpp
	g++ $(CXXFLAGS) -c "src/Planet.cpp"

Player.o : src/Player.cpp
	g++ $(CXXFLAGS) -c "src/Player.cpp"

PointEngine.o : ../../class/PointEngine/src/PointEngine.cpp
	g++ $(CXXFLAGS) -c "../../class/PointEngine/src/PointEngine.cpp"

Point.o : ../../class/PointEngine/src/Point.cpp
	g++ $(CXXFLAGS) -c "../../class/PointEngine/src/Point.cpp"

Rectangle.o : ../../class/PointEngine/src/Rectangle.cpp
	g++ $(CXXFLAGS) -c "../../class/PointEngine/src/Rectangle.cpp"

PhysicConstraint.o : ../../class/PointEngine/src/PhysicConstraint.cpp
	g++ $(CXXFLAGS) -c "../../class/PointEngine/src/PhysicConstraint.cpp"

clean : main.o
	del *.o

