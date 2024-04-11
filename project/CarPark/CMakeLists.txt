project(CarPark)

set(HEADER_FILES
        Car.h
        Map.h
        Point.h
        Solver.h
)

set(SOURCE_FILES
        Car.cpp
        Map.cpp
        Point.cpp
        Solver.cpp
)

add_library(CarPark STATIC ${SOURCE_FILES} ${HEADER_FILES})