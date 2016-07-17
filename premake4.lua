#!lua

includeDirList = { 
    "shared",
    "shared/gl3w",
    "shared/imgui",
    "shared/include",
    "include"
}

libDirectories = { 
    "lib"
}

if os.get() == "macosx" then
    linkLibs = {
        "cs488-framework",
        "imgui",
        "glfw3",
        "lua",
        "lodepng"
    }
end

if os.get() == "linux" then
    linkLibs = {
        "cs488-framework",
        "imgui",
        "glfw3",
        "lua",
        "lodepng",
        "GL",
        "Xinerama",
        "Xcursor",
        "Xxf86vm",
        "Xi",
        "Xrandr",
        "X11",
        "stdc++",
        "dl",
        "pthread"
    }
end

if os.get() == "macosx" then
    linkOptionList = { "-framework IOKit", "-framework Cocoa", "-framework CoreVideo", "-framework OpenGL" }
end

buildOptions = {"-std=c++0x"}


-- Build glfw3 static library and copy it into <cs488_root>/lib if it is not
-- already present.
if not os.isfile("lib/libglfw3.a") then
    os.chdir("shared/glfw-3.1.1")
    os.mkdir("build")
    os.chdir("build")
    os.execute("cmake ../")
    os.execute("make")
    os.chdir("../../../")
    os.mkdir("lib")
    os.execute("cp shared/glfw-3.1.1/build/src/libglfw3.a lib/")
end

if not os.isfile("lib/liblua.a") then
    os.chdir("shared/lua-5.3.1")

    if PLATFORM == "macosx" then
        os.execute("make macosx")
    elseif PLATFORM == "linux" then
        os.execute("make linux")
    elseif PLATFORM == "windows" then
        os.execute("make mingw")
    end

    os.chdir("../../")
    os.execute("cp shared/lua-5.3.1/src/liblua.a lib/")
end


solution "BuildStaticLibs"
    configurations { "Debug", "Release" }

    -- Builds cs488-framework static library
    project "cs488-framework"
        kind "StaticLib"
        language "C++"
        location "build"
        objdir "build"
        targetdir "lib"
        buildoptions (buildOptions)
        includedirs (includeDirList)
        files { "shared/cs488-framework/*.cpp" }

    -- Build imgui-framework static library
    project "imgui"
        kind "StaticLib"
        language "C++"
        location "build"
        objdir "build"
        targetdir "lib"
        includedirs (includeDirList)
        includedirs {
            "shared/imgui/examples/opengl3_example",
            "shared/imgui/examples/libs/gl3w/",
        }
        files { 
            "shared/imgui/*.cpp",
            "shared/gl3w/GL/gl3w.c"
        }

    configuration "Debug"
        defines { "DEBUG" }
        flags { "Symbols" }

    configuration "Release"
        defines { "NDEBUG" }
        flags { "Optimize" }
