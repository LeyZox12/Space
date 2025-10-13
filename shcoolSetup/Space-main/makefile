
CXXFLAGS = -std=c++23 -g -I"../SFML-3.0.0/include" -Iinclude -I"../verlet/include"

MINGW = "\\8408-SRV-FILES\perso\F_ELE\TG6\LAPLACE-TOULOUSE\MinGW\bin\g++"

final : main.o Ship.o Map.o Planet.o Player.o PointEngine.o Element.o Point.o Rectangle.o PhysicConstraint.o PointEngine.o
	$(MINGW) $(CXXFLAGS) Point.o Rectangle.o PhysicConstraint.o PointEngine.o Element.o Ship.o Map.o Planet.o Player.o main.o -o Spaceiscool -L"../SFML-3.0.0/lib" -lsfml-graphics-d -lsfml-window-d -lsfml-system-d

main.o : main.cpp
	$(MINGW) $(CXXFLAGS) -c main.cpp 

Element.o : src/Element.cpp
	$(MINGW) $(CXXFLAGS) -c src/Element.cpp


Ship.o : src/Ship.cpp
	$(MINGW) $(CXXFLAGS) -c "src/Ship.cpp"

Map.o : src/Map.cpp
	$(MINGW) $(CXXFLAGS) -c "src/Map.cpp"

Planet.o : src/Planet.cpp
	$(MINGW) $(CXXFLAGS) -c "src/Planet.cpp"

Player.o : src/Player.cpp
	$(MINGW) $(CXXFLAGS) -c "src/Player.cpp"

PointEngine.o : ../verlet/src/PointEngine.cpp
	$(MINGW) $(CXXFLAGS) -c "../verlet/src/PointEngine.cpp"

Point.o : ../verlet/src/Point.cpp
	$(MINGW) $(CXXFLAGS) -c "../verlet/src/Point.cpp"

Rectangle.o : ../verlet/src/Rectangle.cpp
	$(MINGW) $(CXXFLAGS) -c "../verlet/src/Rectangle.cpp"

PhysicConstraint.o : ../verlet/src/PhysicConstraint.cpp
	$(MINGW) $(CXXFLAGS) -c "../verlet/src/PhysicConstraint.cpp"

clean : main.o
	del *.o

