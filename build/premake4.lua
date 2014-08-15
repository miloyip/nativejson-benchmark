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

	project "nativejson"
		kind "ConsoleApp"
		
        includedirs {
            "../thirdparty/rapidjson/include/",
        }

		files { 
			"../src/**.h",
			"../src/**.cpp",
		}

		setTargetObjDir("../bin")
