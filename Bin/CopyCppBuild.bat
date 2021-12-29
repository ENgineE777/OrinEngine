RMDIR /s /q ENgine\CppBuild\include
xcopy ..\Libs\vjson\*.h ENgine\CppBuild\include\Libs\vjson\ /sy
xcopy ..\Libs\eastl\include\*.* ENgine\CppBuild\include\Libs\eastl\include\ /sy
xcopy ..\Libs\fmod\include\*.* ENgine\CppBuild\include\Libs\fmod\include\ /sy
xcopy ..\Libs\physx\include\*.* ENgine\CppBuild\include\Libs\physx\include\ /sy
xcopy ..\ENgine\*.h ENgine\CppBuild\include\ENgine\ /sy
copy Oak.dll ENgine\ExportBinaries\Oak.dll