'
' to test for argument
'

set argv = WScript.Arguments

wscript.echo ("hello world")
for each val in argv
	wscript.echo (val)
next
