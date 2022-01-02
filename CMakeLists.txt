cmake_minimum_required(VERSION 3.15)

project(eleeye)
set(CMAKE_CXX_STANDARD 20)

add_executable(eleeye eleeye/eleeye.cpp
                        eleeye/ucci.cpp eleeye/ucci.h
                        eleeye/pregen.cpp eleeye/pregen.h
                        eleeye/position.cpp eleeye/position.h
                        eleeye/genmoves.cpp
                        eleeye/hash.cpp eleeye/hash.h
                        eleeye/book.cpp eleeye/book.h
                        eleeye/movesort.cpp eleeye/movesort.h
                        eleeye/search.cpp eleeye/search.h
                        eleeye/preeval.cpp eleeye/pregen.h
                        eleeye/evaluate.cpp
                        base/base.h base/base2.h
                        base/parse.h base/pipe.h base/pipe.cpp
                        base/rc4prng.h
                        base/x86asm.h)