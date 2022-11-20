






rem compile all cpp files in engine directory
cl -I deps\include -c -EHsc deps/include/obs/*.cpp

rem move all compiled files to one directory
cmd /k move *.obj objs