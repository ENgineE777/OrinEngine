call CopyCppBuild.bat
cd ../ENgine/Doxygen
call gen_doc.bat
cd ../../
F:\VS\VS2019\Common7\IDE\devenv.exe /Build Release ENgine\Projects\Windows\Orin.sln /Project Runner
RMDIR /s /q EditorRelease\
xcopy Editor\*.* EditorRelease\Editor\ /sy

del EditorRelease\Editor\CopyCppBuild.bat
del EditorRelease\Editor\eastl.iobj
del EditorRelease\Editor\eastl.ipdb
del EditorRelease\Editor\eastl_Debug.dll
del EditorRelease\Editor\eastl_Debug.ilk
del EditorRelease\Editor\imgui.ini
del EditorRelease\Editor\Orin.iobj
del EditorRelease\Editor\Orin.ipdb
del EditorRelease\Editor\Orin_Debug.dll
del EditorRelease\Editor\Orin_Debug.ilk
del EditorRelease\Editor\OrinEditor.iobj
del EditorRelease\Editor\OrinEditor.ipdb
del EditorRelease\Editor\OrinEditor_Debug.exe
del EditorRelease\Editor\OrinEditor_Debug.ilk
del EditorRelease\Editor\PhysX_64_debug.dll
del EditorRelease\Editor\PhysXCommon_64_debug.dll
del EditorRelease\Editor\PhysXCooking_64_debug.dll
del EditorRelease\Editor\PhysXFoundation_64_debug.dll
del EditorRelease\Editor\PrepareBuild.bat

RMDIR /s /q EditorRelease\Editor\Logs

del EditorRelease\Editor\DebugInfo\eastl_Debug.pdb
del EditorRelease\Editor\DebugInfo\Orin_Debug.pdb
del EditorRelease\Editor\DebugInfo\OrinEditor_Debug.pdb

del EditorRelease\Editor\ENgine\ExportBinaries\Orin_Debug.exe
del EditorRelease\Editor\ENgine\ExportBinaries\Orin_Debug.ilk

del EditorRelease\Editor\projects

xcopy Samples\*.* EditorRelease\Samples\ /sy

RMDIR /s /q EditorRelease\Samples\Beatemup\VsProj

RMDIR /s /q EditorRelease\Samples\Tanks\VsProj

RMDIR /s /q EditorRelease\Samples\SimplePlatformer\VcProj

copy EditorRelease\Samples\projects EditorRelease\Editor\projects
del EditorRelease\Samples\projects

pause