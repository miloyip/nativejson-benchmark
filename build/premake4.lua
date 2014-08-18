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
        }

		files { 
			"../src/**.c",
		}

		setTargetObjDir("../bin")

		os.mkdir("../thirdparty/include/yajl")
		local matches = os.matchfiles("../thirdparty/yajl/src/api/*.h")
		for _, f in ipairs(matches) do 
			local filename = string.match(f, ".-([^\\/]-%.?[^%.\\/]*)$")
			os.copyfile(f, "../thirdparty/include/yajl/" .. filename)
		end

	project "nativejson"
		kind "ConsoleApp"

        includedirs {
            "../thirdparty/",
            "../thirdparty/casablanca/Release/include/",
            "../thirdparty/casablanca/Release/src/pch",
            "../thirdparty/jsoncpp/include/",
            "../thirdparty/rapidjson/include/",
            "../thirdparty/include/",
        }

		files { 
			"../src/**.h",
			"../src/**.cpp",
		}

		libdirs { "../bin" }

		setTargetObjDir("../bin")

		linkLib("jsonclibs")

		configuration "gmake"
			buildoptions "-std=c++11"

