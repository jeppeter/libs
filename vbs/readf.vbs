'  read the file
'
'
Option Explicit

Const ForReading = 1, ForWriting = 2, ForAppending = 8 
Function ReadFile(ifname)
	dim ifh
	dim ofh
	dim s
	dim fsobj
	Set fsobj =  CreateObject("Scripting.FileSystemObject") 
	wscript.echo(ifname)
	if fsobj is Nothing then
		ReadFile = Nothing
	End If
	ifh = fsobj.OpenTextFile("readf.vbs",ForReading, True)

	while True
		s = ifh.readline()
		wscript.echo (s)
	Wend
	
End Function

dim v
dim argv
Set argv = WScript.Arguments
for each v in argv
ReadFile(v)
next

