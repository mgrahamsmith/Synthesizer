:: If old build folder is found, delete and rebuild
@echo off
if exist build (
  rmdir /s build
)

md build
cd build

@echo on
cl /EHsc ..\main.cpp

cd ..
