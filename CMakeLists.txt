cmake_minimum_required(VERSION 3.27)
project(LambdaNFA)

include_directories(./include)
set(CMAKE_CXX_STANDARD 20)

add_executable(LambdaNFA main.cpp
        include/lambda_nfa.h
        src/lambda_nfa.cpp)
