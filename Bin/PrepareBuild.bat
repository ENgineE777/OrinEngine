call CopyCppBuild.bat
cd ../Doxygen
call gen_doc.bat
cd ../
RMDIR /s /q ENgineRelease\
xcopy Bin\*.* ENgineRelease\ /sy

del ENgineRelease\CopyCppBuild.bat
del ENgineRelease\eastl.iobj
del ENgineRelease\eastl.ipdb
del ENgineRelease\eastl_Debug.dll
del ENgineRelease\eastl_Debug.ilk
del ENgineRelease\imgui.ini
del ENgineRelease\Oak.iobj
del ENgineRelease\Oak.ipdb
del ENgineRelease\Oak_Debug.dll
del ENgineRelease\Oak_Debug.ilk
del ENgineRelease\OakEditor.iobj
del ENgineRelease\OakEditor.ipdb
del ENgineRelease\OakEditor_Debug.exe
del ENgineRelease\OakEditor_Debug.ilk
del ENgineRelease\PhysX_64_debug.dll
del ENgineRelease\PhysXCommon_64_debug.dll
del ENgineRelease\PhysXCooking_64_debug.dll
del ENgineRelease\PhysXFoundation_64_debug.dll
del ENgineRelease\PrepareBuild.bat
del ENgineRelease\editor

RMDIR /s /q ENgineRelease\Logs

del ENgineRelease\DebugInfo\eastl_Debug.pdb
del ENgineRelease\DebugInfo\Oak_Debug.pdb
del ENgineRelease\DebugInfo\OakEditor_Debug.pdb

del ENgineRelease\ENgine\ExportBinaries\Oak_Debug.exe
del ENgineRelease\ENgine\ExportBinaries\Oak_Debug.ilk

RMDIR /s /q ENgineRelease\DemoProjects\Beatemup\_Code
del ENgineRelease\DemoProjects\Beatemup\gameplay_Debug.dll

RMDIR /s /q ENgineRelease\DemoProjects\Tanks\_Code
del ENgineRelease\DemoProjects\Tanks\gameplay_Debug.dll

del ENgineRelease\projects
copy ENgineRelease\ENgine\defaultProjects ENgineRelease\projects
del ENgineRelease\ENgine\defaultProjects

pause