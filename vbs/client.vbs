'
'   use this to make install of proxy.pac ,and use 
'   cscript reg.vbs ipport
'       ipport is like 192.168.1.1:8080   format
Option Explicit

const HKEY_LOCAL_MACHINE = &H80000002
Const HKEY_CURRENT_USER = &H80000001 
Const HKEY_USERS =  &H80000003
const REG_SZ = 1
const REG_EXPAND_SZ = 2
const REG_BINARY = 3
const REG_DWORD = 4
const REG_MULTI_SZ = 7

dim strComputer,oReg
dim strKeyPath,arrValueNames
dim arrValueTypes,StdOut
dim i

Function SetUserValue(user,key,valuename,value)
	dim oReg,strComputer
	dim strKey,stdout
	set stdout = wscript.stdout
	strComputer = "."
	Set oReg=GetObject("winmgmts:{impersonationLevel=impersonate}!\\" &_ 
	strComputer & "\root\default:StdRegProv")
	strKey = user & "\\" & key
	stdout.writeline(strKey & " set " & valuename & "=" & value)
	oReg.SetStringValue HKEY_USERS,strKey,valuename,value
End Function

Function GetAllUsers()
	dim strComputer,strKeyPath,oReg,arrSubKeys
	strComputer = "."	 
	Set oReg=GetObject("winmgmts:{impersonationLevel=impersonate}!\\" &_ 
	strComputer & "\root\default:StdRegProv")	 
	strKeyPath = ""
	oReg.EnumKey HKEY_USERS, strKeyPath, arrSubKeys
	GetAllUsers = arrSubKeys
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


Function GetSystemDrive()
	dim drv
	dim wshShell
	Set wshShell = CreateObject( "WScript.Shell" )
	drv = wshShell.ExpandEnvironmentStrings( "%SystemDrive%" )
	GetSystemDrive = drv
End Function

Function RegistryAll(installpac)
	dim users,key,valuename,value,u
	users = GetAllUsers()
	key="Software\Microsoft\Windows\CurrentVersion\Internet Settings"
	valuename="AutoConfigURL"
	value="file://" & installpac
	for each u in users
		SetUserValue u,key,valuename,value
	next

End Function


Function InstallPacFile(ipport)
	dim installdir,installpac,tmplpac
	installdir = GetSystemDrive
	installpac = installdir & "\" & "proxy.pac"
	tmplpac = "proxy.pac.tmpl"
	FileReplace tmplpac , installpac,"%PROXY_IP_PORT%",ipport	
	RegistryAll installpac
End Function


dim ipport,args,numi,v
Set args = wscript.Arguments

numi = 0
for each v in args
	numi = numi + 1
next

if numi < 1 then
	wscript.stderr.writeline("client.vbs ippport")
	wscript.Quit 3
end if

ipport = args(0)

InstallPacFile ipport