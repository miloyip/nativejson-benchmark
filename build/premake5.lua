function setTargetObjDir(outDir)
	targetdir(outDir)
	objdir(string.lower("../intermediate/%{cfg.shortname}/" .. _ACTION))
	targetsuffix(string.lower("_%{cfg.shortname}_" .. _ACTION))
end

function copyfiles(dstDir, srcWildcard)
	os.mkdir(dstDir)
	local matches = os.matchfiles(srcWildcard)
	for _, f in ipairs(matches) do
		local filename = string.match(f, ".-([^\\/]-%.?[^%.\\/]*)$")
		os.copyfile(f, dstDir .. "/" .. filename)
	end
end

function gmake_common()
    buildoptions "-march=native -Wall -Wextra"
    if (os.findlib("boost_system")) then
        defines { "HAS_BOOST=1" }
        links { "boost_system" }
    end

    -- On some boost distributions, the naming contains -mt suffixes
    if (os.findlib("boost_thread")) then
        links  { "boost_thread" }
    elseif (os.findlib("boost_thread-mt")) then
        links  { "boost_thread-mt" }
    end

    if (os.findlib("boost_locale")) then
        links  { "boost_locale" }
    elseif (os.findlib("boost_locale-mt")) then
        links  { "boost_locale-mt" }
    end

    -- For clock_gettime in jvar
    if (os.findlib("rt")) then
        links  { "rt" }
    end

    if (os.findlib("PocoJSON")) then
        defines { "HAS_POCO=1" }
        links { "PocoFoundation", "PocoJSON" }
    end

    if (os.findlib("folly")) then
        defines { "HAS_FOLLY=1" }
        links { "folly" }
    end

    if (os.findlib("v8")) then
        defines { "HAS_V8=1" }
        links { "v8_base", "v8_libbase", "v8_libplatform", "v8_nosnapshot" }
    end

    if (os.findlib("libcpprest")) then
        defines { "HAS_CPPREST=1" }
        links { "cpprest"}
    end

    configuration "macosx"
        if (os.isdir("/usr/local/opt/qt5/include")) then
            defines { "HAS_QT=1" }
            links { "QtCore.framework" }
            includedirs { "/usr/local/opt/qt5/include" }
            linkoptions { "-F /usr/local/opt/qt5/lib" }
        end

        -- Temp fix for OSX brew + V8 include path issue
        if (os.isdir("/usr/local/opt/v8/")) then
            includedirs { "/usr/local/opt/v8/" }
        end
end

solution "benchmark"
	configurations { "release" }
	platforms { "x32", "x64" }

	location ("./" .. (_ACTION or ""))
	language "C++"
	flags { "ExtraWarnings" }
	defines { "__STDC_FORMAT_MACROS=1" }

	configuration "release"
		defines { "NDEBUG" }
		optimize "Full"

	configuration "vs*"
		defines { "_CRT_SECURE_NO_WARNINGS" }

	configuration "gmake"
		gmake_common()

	project "jsonclibs"
		kind "StaticLib"

        includedirs {
            "../thirdparty/",
            "../thirdparty/include/",
            "../thirdparty/ujson4c/3rdparty/",
            "../thirdparty/pjson/inc/",
  			"../thirdparty/udp-json-parser/",
            "../thirdparty/facil.io/lib/facil/core/types",
            "../thirdparty/facil.io/lib/facil/core/types/fiobj",
        }

		files {
			"../src/**.c",
		}

		setTargetObjDir("../bin")

		copyfiles("../thirdparty/include/yajl", "../thirdparty/yajl/src/api/*.h" )

	project "nativejson"
		kind "ConsoleApp"

        includedirs {
            "../thirdparty/",
            "../thirdparty/fastjson/include/",
            "../thirdparty/jsonbox/include/",
            "../thirdparty/jsoncpp/include/",
            "../thirdparty/rapidjson/include/",
            "../thirdparty/udp-json-parser/",
            "../thirdparty/include/",
            "../thirdparty/json-voorhees/include",
            "../thirdparty/json-voorhees/src",
            "../thirdparty/jsoncons/src",
            "../thirdparty/ArduinoJson/include",
            "../thirdparty/include/jeayeson/include/dummy",
  			"../thirdparty/jvar/include",
            "../thirdparty/pjson/inc",
            "../thirdparty/ULib/include",
            "../thirdparty/facil.io/lib/facil/core/types",
            "../thirdparty/facil.io/lib/facil/core/types/fiobj",
            "../thirdparty/simdjson/singleheader",
            "../thirdparty/boost/libs/json/include",
            "../thirdparty/boost/libs/config/include",
            "../thirdparty/boost/libs/assert/include",
            "../thirdparty/boost/libs/exception/include",
            "../thirdparty/boost/libs/throw_exception/include",
            "../thirdparty/boost/libs/core/include",
            "../thirdparty/boost/libs/container/include",
            "../thirdparty/boost/libs/move/include",
            "../thirdparty/boost/libs/static_assert/include",
            "../thirdparty/boost/libs/intrusive/include",
            "../thirdparty/boost/libs/system/include",
            "../thirdparty/boost/libs/mp11/include",
            "../thirdparty/boost/libs/align/include",
        }

      linkoptions { "../../thirdparty/ULib/src/ulib/.libs/libulib.a" }

		files {
			"../src/*.h",
			"../src/*.cpp",
			"../src/tests/*.cpp"
		}

		libdirs { "../bin",
                "../thirdparty/simdjson"  
                }

		setTargetObjDir("../bin")

		-- linkLib("jsonclibs")
		links "jsonclibs"

		configuration "gmake"
			buildoptions "-std=c++14"

solution "jsonstat"
    configurations { "release" }
    platforms { "x32", "x64" }
    location ("./" .. (_ACTION or ""))
    language "C++"
    flags { "ExtraWarnings" }

    defines {
    	"USE_MEMORYSTAT=0",
    	"TEST_PARSE=1",
    	"TEST_STRINGIFY=0",
    	"TEST_PRETTIFY=0",
    	"TEST_TEST_STATISTICS=1",
    	"TEST_SAXROUNDTRIP=0",
    	"TEST_SAXSTATISTICS=0",
    	"TEST_SAXSTATISTICSUTF16=0",
    	"TEST_CONFORMANCE=0",
    	"TEST_INFO=0"
	}

    includedirs {
        "../thirdparty/",
        "../thirdparty/fastjson/include/",
        "../thirdparty/jsonbox/include/",
        "../thirdparty/jsoncpp/include/",
        "../thirdparty/rapidjson/include/",
        "../thirdparty/udp-json-parser/",
        "../thirdparty/include/",
        "../thirdparty/json-voorhees/include",
        "../thirdparty/json-voorhees/src",
        "../thirdparty/jsoncons/src",
        "../thirdparty/ArduinoJson/include",
        "../thirdparty/include/jeayeson/include/dummy",
        "../thirdparty/jvar/include",
        "../thirdparty/pjson/inc",
        "../thirdparty/ULib/include",
        "../thirdparty/facil.io/lib/facil/core/types",
        "../thirdparty/facil.io/lib/facil/core/types/fiobj",
        "../thirdparty/simdjson/singleheader",
        "../thirdparty/boost/libs/json/include",
        "../thirdparty/boost/libs/config/include",
        "../thirdparty/boost/libs/assert/include",
        "../thirdparty/boost/libs/exception/include",
        "../thirdparty/boost/libs/throw_exception/include",
        "../thirdparty/boost/libs/core/include",
        "../thirdparty/boost/libs/container/include",
        "../thirdparty/boost/libs/move/include",
        "../thirdparty/boost/libs/static_assert/include",
        "../thirdparty/boost/libs/intrusive/include",
        "../thirdparty/boost/libs/system/include",
        "../thirdparty/boost/libs/mp11/include",
        "../thirdparty/boost/libs/align/include",
    }

    configuration "release"
        defines { "NDEBUG" }
        optimize "Full"

    configuration "vs*"
        defines { "_CRT_SECURE_NO_WARNINGS" }

	configuration "gmake"
        gmake_common()

	project "jsonclibs2"
		kind "StaticLib"

        includedirs {
            "../thirdparty/",
            "../thirdparty/include/",
            "../thirdparty/ujson4c/3rdparty/",
  			"../thirdparty/udp-json-parser/",
            "../thirdparty/facil.io/lib/facil/core/types",
            "../thirdparty/facil.io/lib/facil/core/types/fiobj",
        }

		files {
			"../src/**.c",
		}

        setTargetObjDir("../bin/jsonstat")

		copyfiles("../thirdparty/include/yajl", "../thirdparty/yajl/src/api/*.h", "../thirdparty/simdjson/src/simdjson.cpp")

    local testfiles = os.matchfiles("../src/tests/*.cpp")
    for _, testfile in ipairs(testfiles) do
        project("jsonstat_" .. path.getbasename(testfile))
            kind "ConsoleApp"
            files {
            	"../src/jsonstat/jsonstatmain.cpp",
            	"../src/memorystat.cpp",
            	testfile
			}
			libdirs { "../bin/jsonstat" }
			links "jsonclibs2"
            setTargetObjDir("../bin/jsonstat")

      linkoptions { "../../thirdparty/ULib/src/ulib/.libs/libulib.a" }

			configuration "gmake"
				buildoptions "-std=c++14"
    end
