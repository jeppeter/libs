'
'     to makethe services
'
Option Explicit
'On Error Resume Next

Const OWN_PROCESS = 16
Const NOT_INTERACTIVE = False
Const NORMAL_ERROR_CONTROL = 2


Function RunCmd(cmd)
	Dim WshShell, oExec,x
	Set WshShell = WScript.CreateObject("WScript.Shell")
	Set oExec = WshShell.Exec(cmd)
	x = oExec.StdOut.ReadAll
	wscript.stderr.writeline("Run Cmd (" & cmd & ")")
	wscript.stdout.writeline(x)
End Function


Function StartService(sname)
	dim sccmd
'	Dim WshShell, oExec,x
	sccmd = "sc start " & sname
'	wscript.echo ("cmd (" & sccmd & ")")
'	Set WshShell = WScript.CreateObject("WScript.Shell")
'	Set oExec = WshShell.Exec(sccmd)
'	x = oExec.StdOut.ReadAll
'	wscript.echo(x)	
	RunCmd sccmd
End Function

Function StopService(sname)
	dim sccmd
'	Dim WshShell, oExec,x
	sccmd = "sc stop " & sname
	RunCmd sccmd
'	wscript.echo ("cmd (" & sccmd & ")")
'	Set WshShell = WScript.CreateObject("WScript.Shell")
'	Set oExec = WshShell.Exec(sccmd)
'	x = oExec.StdOut.ReadAll
'	wscript.echo(x)	
End Function

Function RemoveService(sname)
	dim sccmd
'	Dim WshShell, oExec,x
	sccmd = "sc delete " & sname
	RunCmd sccmd
'	wscript.echo ("cmd (" & sccmd & ")")
'	Set WshShell = WScript.CreateObject("WScript.Shell")
'	Set oExec = WshShell.Exec(sccmd)
'	x = oExec.StdOut.ReadAll
'	wscript.echo(x)	
End Function


Function InstallService(sname,dname,bin,cmd)
	dim sccmd
	sccmd = "sc create " & " " & sname & " "
	sccmd = sccmd & "DisplayName= "
	sccmd = sccmd & chr(34) & dname & chr(34)
	sccmd = sccmd & " type= own "
	sccmd = sccmd & " start= auto "
	sccmd = sccmd & " error= normal "
	sccmd = sccmd & " binPath= "& chr(34) & cmd & chr(34)
	RunCmd sccmd
End Function

Function GetSystemDrive()
	dim drv
	dim wshShell
	Set wshShell = CreateObject( "WScript.Shell" )
	drv = wshShell.ExpandEnvironmentStrings( "%SystemDrive%" )
	GetSystemDrive = drv
End Function

Function CopyDir(srcdir,dstdir)
	dim fso
	Set fso = CreateObject("Scripting.FileSystemObject")
	fso.CopyFolder srcdir   , dstdir,True
End Function

Function CurrentDir()
	dim sCurPath
	sCurPath = CreateObject("Scripting.FileSystemObject").GetAbsolutePathName(".")
	wscript.echo(sCurPath)
	CurrentDir = sCurPath
End Function

Function FileReplace(ifname,ofname,orig,repl)
	dim objFSO,objInputFile,objOutputFile
	dim strin,strout
	Set objFSO = CreateObject("Scripting.FileSystemObject")

	Set objInputFile = objFSO.OpenTextFile(ifname,1)
	Set objOutputFile = objFSO.OpenTextFile(ofname,2,True)
	Do until objInputFile.AtEndofStream
		strin = objInputFile.ReadLine()
		strout = Replace(strin,orig,repl)
		objOutputFile.write(strout & vbCRLF)
	Loop

	objInputFile.Close
	objOutputFile.Close
End Function

Function DeleteFolderFunc(d)
dim objFSO
Set	objFSO=CreateObject("Scripting.FileSystemObject")
if objFSO.FolderExists(d) then
	wscript.echo("Remove " & d)
	objFSO.DeleteFolder d ,FALSE
end if
End Function


Function Install3Proxy(port)
	dim cmd
	dim sCurDir,dstdir,bin,tmplf,conf
	sCurDir = CurrentDir
	dstdir = GetSystemDrive
	tmplf = sCurDir & "\3proxy.cfg.tmpl"
	conf = sCurDir & "\3proxy.cfg"
	FileReplace tmplf ,conf,"%LISTEN_PORT%",port
	dstdir = dstdir & "\" & "3proxy"
	CopyDir sCurDir,dstdir
	cmd = "\" &  chr(34) & dstdir &"\3proxy.exe" & "\" & chr(34)
	cmd = cmd & " "& "\" &  chr(34) &  dstdir &"\3proxy.cfg" & "\" & chr(34)
	cmd = cmd & " " & "\" & chr(34) &  "--service" & "\" & chr(34)
	bin = dstdir & "\3proxy.exe"
	InstallService "3proxy","3proxy tiny proxy","Z:\3proxy-bin\bin\3proxy.exe",cmd
	StartService "3proxy"
End Function

Function Remove3Proxy()
	dim dstdir
	Dim WshShell, oExec,x
	On Error Resume Next
	StopService "3proxy"
	RemoveService "3proxy"
	dstdir = GetSystemDrive
	dstdir = dstdir & "\" & "3proxy"
	DeleteFolderFunc dstdir
End Function

dim args,val,i
dim port 
Set args = Wscript.Arguments
i = 0
port = 8080

for each val in args
	i = i + 1
next

if i = 0 then
	wscript.stderr.writeline("service.vbs --install/--uninstall [port]")
	wscript.stderr.writeline("   install use port")
	wscript.stderr.writeline("   uninstall not use port")
	wscript.Quit 3
else
	if args(0) = "--install" then
		if i > 1 then
			port = args(1)
		end if
		Remove3Proxy
		Install3Proxy port
	end if
	if args(0) = "--uninstall" then
		Remove3Proxy 
	end if
end if

