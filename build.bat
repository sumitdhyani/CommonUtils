mkdir build
cd build
cmake ..
msbuild CommonUtils.sln /p:Configuration=Release
msbuild CommonUtils.sln /p:Configuration=Debug
pause
