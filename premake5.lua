workspace "lua_capi"
    configurations { "Debug", "Release" }

    targetdir "."
    includedirs { 
        ".", 
        "/usr/include/luajit-2.1" 
    }
    links { "luajit-5.1" }

    project "ex_01"
        files { "*.h", "ex_01.c" }
        kind "ConsoleApp"
        language "C"

    project "ex_02"
        files { "*.h", "ex_02.c" }
        kind "ConsoleApp"
        language "C"

        --targetdir "bin/%{cfg.buildcfg}"
        --buildoptions { "-fPIC" }
        --libdirs { "/usr/lib" }
        --links { "lua5.1" }

    --filter "configurations:Debug"
    --defines { "DEBUG" }
    --symbols "On"

    --filter "configurations:Release"
    --defines { "NDEBUG" }
    --optimize "On"
