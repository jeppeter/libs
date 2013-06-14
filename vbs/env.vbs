'  to get environment value
'
'
Set wshShell = CreateObject( "WScript.Shell" )
WScript.Echo wshShell.ExpandEnvironmentStrings( "%SystemDrive%" )

Set wshShell = Nothing

if wshShell is Nothing then
	wscript.echo "set nothing"
end if

