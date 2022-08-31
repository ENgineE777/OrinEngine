RMDIR /s /q ENgine\CppBuild\include
xcopy ..\ENgine\Thirdparty\vjson\*.h ENgine\CppBuild\include\Libs\vjson\ /sy
xcopy ..\ENgine\Thirdparty\eastl\include\*.* ENgine\CppBuild\include\Libs\eastl\include\ /sy
xcopy ..\ENgine\Thirdparty\fmod\include\*.* ENgine\CppBuild\include\Libs\fmod\include\ /sy
xcopy ..\ENgine\Thirdparty\physx\include\*.* ENgine\CppBuild\include\Libs\physx\include\ /sy
xcopy ..\ENgine\Source\*.h ENgine\CppBuild\include\ENgine\ /sy
copy Oak.dll ENgine\ExportBinaries\Oak.dll