# Native JSON Benchmark

Copyright(c) 2014 Milo Yip (miloyip@gmail.com)

## Introduction

This benchmark evaluates the performance of 21 open-source C/C++ JSON parser/generator libraries. Performance means speed, memory, and code size.

Performance of JSON parsing/generation may be critical for server-side applications, mobile/embedded systems, or any application that requires processing of large size or number of JSONs. Native (C/C++) libraries are important because they should provide the best possible performance, while other languages may create bindings of native libraries.

The results show that several performance measurements varies in a large scales among libraries. For example, the parsing time can be over 100 times. These differences came from many factors, including design and implementation details. For example, memory allocation strategies, design of variant type for JSON, number-string conversions, etc.

This benchmark may be useful for optimizing existing libraries and developing new, high-performance libraries.

## Disclaimer

The original author (Milo Yip) of this benchmark is also the primary author of [RapidJSON](https://github.com/miloyip/rapidjson). Although he has tried to make the benchmark as objective and fair as possible, every benchmarks have their drawbacks, and are limited to particular testing procedures, datasets and platforms. And also, this benchmark does not compare additional features that a library may support, or the user-friendliness of APIs, securities, cross-platform, etc. The author encourage users to benchmarks with their own data sets and platforms.

## Benchmarks and Measurements

Benchmark      | Description
---------------|----------------------------------------------------
Parse          | Parse in-memory JSON into DOM (tree structure).
Stringify      | Serialize DOM into condensed JSON in memory.
Prettify       | Serialize DOM into prettified (with indentation and new lines) JSON in memory.
Statistics     | Traverse DOM and count the number of JSON types, total length of string, and total numbers of elements/members in array/objects.
Sax Round-trip | Parse in-memory JSON into events and use events to generate JSON in memory.
Sax Statistics | Parse in-memory JSON into events and use events to conduct the statistics.
Code size      | Executable size in byte. (Currently only support `jsonstat` program, which calls "Parse" and "Staistics" to print out statistics of a JSON file. )

All benchmarks contain the following measurements:

Measurement | Description
------------|----------------------------------------------------
Time        | duration in millisecond
Memory      | Memory consumption in bytes for the result data structure.
MemoryPeak  | Peak memory consumption in bytes throughout the parsing process.
AllocCount  | Number of memory allocation (including `malloc`, `realloc()`, `new` et al.)

## Libraries

Currently 21 libraries are successfully benchmarked. They are listed in alphabetic order:

Library | Language | Version | Notes
--------|----------|---------|-------------------
[CAJUN](https://github.com/cajun-jsonapi/cajun-jsonapi) | C++ |
[Casablanca](https://casablanca.codeplex.com/) (C++ REST SDK) | C++11 | | Need Boost on non-Windows platform. DOM strings must be UTF16 on Windows and UTF8 on non-Windows platform.
[cJSON](http://sourceforge.net/projects/cjson/) | C |
[dropbox/json11](https://github.com/dropbox/json11) | C++11 |
[FastJson](https://github.com/mikeando/fastjson) | C++ |
[gason](https://github.com/vivkin/gason) | C++11 |
[jansson](https://github.com/akheron/jansson) | C |
[json-c](https://github.com/json-c/json-c) | C |
[json spirit](http://www.codeproject.com/Articles/20027/JSON-Spirit-A-C-JSON-Parser-Generator-Implemented) | C++ | 4.08 | Need Boost
[Json Box](https://github.com/anhero/JsonBox) | C++ |
[JsonCpp](https://github.com/open-source-parsers/jsoncpp) | C++
[JSON++](https://github.com/hjiang/jsonxx) | C++
[parson](https://github.com/kgabis/parson) | C
[picojson](https://github.com/kazuho/picojson) | C++
[RapidJSON](https://github.com/miloyip/rapidjson) | C++
[simplejson](https://github.com/MJPA/SimpleJSON) | C++
udp/json ([udp/json-builder](https://github.com/udp/json-builder) & 
[udp/json-parser](https://github.com/udp/json-parser)) | C
[ujson4c](https://github.com/esnme/ujson4c) | C
[vincenthz/libjson](https://github.com/vincenthz/libjson) | C
[YAJL](https://github.com/lloyd/yajl) | C | 2.1.0

Libraries with Git repository are included as submodule in `thirdparty` path. Other libraries are add as files in `thirdparty` path.

Some libraries was tried but unable to benchmark:

Library   | Issue
----------|------------------------------
[libjson](http://sourceforge.net/projects/libjson/) | Unable to parse UTF-8 string
[lastjson](https://github.com/lastfm/last.json) | 
[StiX Json](https://bitbucket.org/StiX/stix-json) |

## JSON data

JSON file   | Size | Description
------------|------------------------------
canada.json [source](https://github.com/mloskot/json_benchmark/blob/master/data/canada.json) | 2199KB | Contour of Canada border in [GeoJSON](http://geojson.org/) format. Contains a lot of real numbers.
citm_catalog.json [source](https://github.com/RichardHightower/json-parsers-benchmark/blob/master/data/citm_catalog.json) | 1737KB | 
* twitter.json | 632KB | Search "一" (character of "one" in Japanese and Chinese) in Twitter public time line for gathering some tweets with CJK characters.

The benchmark program reads `data/data.txt` which contains file names of JSON to be tested.

## Build and Run

1. Obtain [premake4](http://industriousone.com/premake/download).
2. Copy premake4 executable to `build/` path (or system path).
3. Run `premake.bat` or `premake.sh` in `build`
4. On Windows, build the solution at `build/vs2008/` or `/vs2010/`.
5. On other platforms, run GNU `make config=release32` (or `release64`) at `build/gmake/`
6. Optional: run `buuild/machine.sh` for UNIX or CYGWIN to use CPU info to generate prefix of result filename.
7. Run the `nativejson_release_...` executable is generated at `bin/`
8. The results in CSV format will be written to `result/`.
9. Run GNU `make` in `result/` to generate results in HTML.

## Results

## FAQ

1. How to add a library?

   Use `submodule add https://...xxx.git thirdparty/xxx` to add the libary's repository as a submobule. If that is not possible, just copy the files into 'thirdparty/xxx'.

   For C libary, add a `xxx_all.c` in `src/cjsonlibs`, which `#include` all the necessary `.c` files of the library. And then create a `tests/xxxtest.cpp`.

   For C++ library, just need to create a `tests/xxxtest.cpp`, which `#include` all the necessary `.cpp` files of the library.

   You may find a existing library which similar to your case as a start of implementing `tests/xxxtest.cpp`.

## Other native JSON benchmarks

* [Basic benchmarks for miscellaneous C++ JSON parsers and generators](https://github.com/mloskot/json_benchmark) by Mateusz Loskot (Jun 2013)
 * [Casablanca](https://casablanca.codeplex.com/)
 * [json spirit](https://github.com/cierelabs/json_spirit)
 * [JsonCpp](http://jsoncpp.sourceforge.net/)
 * [libjson](http://sourceforge.net/projects/libjson/)
 * [rapidjson](https://github.com/miloyip/rapidjson/)
 * [QJsonDocument](http://qt-project.org/doc/qt-5.0/qtcore/qjsondocument.html)
 
* [JSON Parser Benchmarking](http://chadaustin.me/2013/01/json-parser-benchmarking/) by Chad Austin (Jan 2013)
 * [sajson](https://github.com/chadaustin/sajson)
 * [RapidJSON](https://github.com/miloyip/rapidjson/)
 * [vjson](https://code.google.com/p/vjson/) (Replaced by gason)
 * [YAJL](http://lloyd.github.com/yajl/)
 * [Jansson](http://www.digip.org/jansson/)
