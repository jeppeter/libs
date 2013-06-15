
Option Explicit
const HKEY_LOCAL_MACHINE = &H80000002

Function GetAllUsers()
	dim strComputer,strKeyPath,oReg,arrSubKeys
	const HKEY_USERS =  &H80000003
	strComputer = "."
	 
	Set oReg=GetObject("winmgmts:{impersonationLevel=impersonate}!\\" &_ 
	strComputer & "\root\default:StdRegProv")
	 
	'strKeyPath = "SYSTEM\CurrentControlSet\Control\Lsa"
	strKeyPath = ""
	oReg.EnumKey HKEY_USERS, strKeyPath, arrSubKeys
	GetAllUsers = arrSubKeys
End Function

dim users,StdOut,subkey
Set StdOut = WScript.StdOut
users = GetAllUsers()
For Each subkey In users
    StdOut.WriteLine subkey
Next

