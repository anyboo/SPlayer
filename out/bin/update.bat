@echo off 
set curdir=%~dp0
 cd /d %curdir%
"7-Zip\7zG.exe" x "update.7z"  -y 
del update.7z
del update_new.xml
exit