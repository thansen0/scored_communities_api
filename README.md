# Scored Communities C++ API

This is a C++ implementation of the [scored.co](https://scored.co/) API. This is the same site as patriots.win and communities.win, and you can use their API interchangeably.

## Building

This project (unfortunately) has two dependencies. json.hpp for parsing json (nlohmann/json), and libcurl for processing GET and POST requests. Be sure to include `-lcurl` when building with g++, or to put it in your CMake file.
