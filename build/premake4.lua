function setTargetObjDir(outDir)
	for _, cfg in ipairs(configurations()) do
		for _, plat in ipairs(platforms()) do
			local action = _ACTION or ""
			
			local prj = project()
			
			--"_debug_win32_vs2008"
			local suffix = "_" .. cfg .. "_" .. plat .. "_" .. action
			
			targetPath = outDir
			
			suffix = string.lower(suffix)

			local obj_path = "../intermediate/" .. cfg .. "/" .. action .. "/" .. prj.name
			
			obj_path = string.lower(obj_path)
			
			configuration {cfg, plat}
				targetdir(targetPath)
				objdir(obj_path)
				targetsuffix(suffix)
		end
	end
end

function linkLib(libBaseName)
	local action = _ACTION or ""

	for _, cfg in ipairs(configurations()) do
		for _, plat in ipairs(platforms()) do			
			--"_debug_win32_vs2008"
			local suffix = "_" .. cfg .. "_" .. plat .. "_" .. action
			
			suffix = string.lower(suffix)
			
			local libFullName = libBaseName or ""
			libFullName = libFullName .. suffix
			
			-- print(libFullName)
			
			configuration {cfg, plat}
				links(libFullName)
		end
	end
end

function copyfiles(dstDir, srcWildcard)
	os.mkdir(dstDir)
	local matches = os.matchfiles(srcWildcard)
	for _, f in ipairs(matches) do 
		local filename = string.match(f, ".-([^\\/]-%.?[^%.\\/]*)$")
		os.copyfile(f, dstDir .. "/" .. filename)
	end
end

solution "benchmark"
	configurations { "debug", "release" }
	platforms { "x32", "x64" }

	location ("./" .. (_ACTION or ""))
	language "C++"
	flags { "ExtraWarnings" }
	defines { "__STDC_FORMAT_MACROS=1" }
	
	configuration "debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	configuration "release"
		defines { "NDEBUG" }
		flags { "Optimize" }

	configuration "vs*"
		defines { "_CRT_SECURE_NO_WARNINGS" }
		
	configuration "gmake"
		buildoptions "-msse4.2 -Wall -Wextra"

	project "jsonclibs"
		kind "StaticLib"

        includedirs {
            "../thirdparty/",
            "../thirdparty/include/",
            "../thirdparty/ujson4c/3rdparty/",
			"../thirdparty/udp-json-parser/"
        }

		files { 
			"../src/**.c",
		}

		setTargetObjDir("../bin")

		copyfiles("../thirdparty/include/yajl", "../thirdparty/yajl/src/api/*.h")

	project "nativejson"
		kind "ConsoleApp"

        includedirs {
            "../thirdparty/",
            "../thirdparty/casablanca/Release/include/",
            "../thirdparty/casablanca/Release/src/pch",
            "../thirdparty/fastjson/include/",
            "../thirdparty/jsonbox/include/",
            "../thirdparty/jsoncpp/include/",
            "../thirdparty/rapidjson/include/",
            "../thirdparty/udp-json-parser/",
            "../thirdparty/include/",
            "../thirdparty/json-voorhees/include",
            "../thirdparty/json-voorhees/src",
        }

		files { 
			"../src/*.h",
			"../src/*.cpp",
			"../src/tests/*.cpp",
		}

		libdirs { "../bin" }

		setTargetObjDir("../bin")

		linkLib("jsonclibs")
		links "jsonclibs"

		configuration "gmake"
			buildoptions "-std=c++11"

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
    	"TEST_INFO=0"
	}

    includedirs {
        "../thirdparty/",
        "../thirdparty/casablanca/Release/include/",
        "../thirdparty/casablanca/Release/src/pch",
        "../thirdparty/fastjson/include/",
        "../thirdparty/jsonbox/include/",
        "../thirdparty/jsoncpp/include/",
        "../thirdparty/rapidjson/include/",
        "../thirdparty/udp-json-parser/",
        "../thirdparty/include/",
    }

    configuration "release"
        defines { "NDEBUG" }
        flags { "Optimize", "EnableSSE2" }

    configuration "vs*"
        defines { "_CRT_SECURE_NO_WARNINGS" }

	project "jsonclibs2"
		kind "StaticLib"

        includedirs {
            "../thirdparty/",
            "../thirdparty/include/",
            "../thirdparty/ujson4c/3rdparty/",
			"../thirdparty/udp-json-parser/"
        }

		files { 
			"../src/**.c",
		}

        setTargetObjDir("../bin/jsonstat")

		copyfiles("../thirdparty/include/yajl", "../thirdparty/yajl/src/api/*.h")

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
			linkLib("jsonclibs2")
			links "jsonclibs2"
            setTargetObjDir("../bin/jsonstat")

			configuration "gmake"
				buildoptions "-std=c++11"
    end
