# MetaRef

[![CI](https://github.com/DaniAffCH/metaref/actions/workflows/ci.yml/badge.svg)](https://github.com/DaniAffCH/metaref/actions/workflows/ci.yml)

This library is meant to make the use of C++26 static reflection less painful. 

[P2996](https://isocpp.org/files/papers/P2996R13.html) static reflection is still at an early stage and its use can be quite error-prone given the plethora of possible edge cases.

`metaref` provides a collection of STL-like utilities mostly built on top of `std::meta` with the goal of handling these cases internally and providing a simple, centralized API.

> Status: experimental. metaref is at a very early stage and is developed against GCC 16 with '-freflection'. The API will change.
