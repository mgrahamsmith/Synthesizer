:: If old build folder is found, delete and rebuild
@echo off
if exist build (
  @echo on
  rmdir /s build
)

:: Leave project root, create and enter build folder 
md build
cd build

:: Compiler the project
@echo on
cl /EHsc ..\main.cpp

:: Exit build folder back to project root.
cd ..