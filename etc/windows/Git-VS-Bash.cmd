@echo off
%HOMEDRIVE%
cd %HOMEPATH%
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
echo Use full exe names when running under bash, e.g. "msbuild.exe"
echo Loading bash, you may now use git and msbuild in the same console \o/.
"C:\Program Files\Git\bin\sh.exe" --login -i
