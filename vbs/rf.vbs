'
'   this file %IP_PORT%
'


Function FileReplace(ifname,ofname,orig,repl)
	dim objFSO,objInputFile,objOutputFile
	dim strin,strout,objstdout
	Set objFSO = CreateObject("Scripting.FileSystemObject")

	Set objInputFile = objFSO.OpenTextFile(ifname,1)
	Set objOutputFile = objFSO.OpenTextFile(ofname,2,True)
	Set objstdout = wscript.stdout
	Do until objInputFile.AtEndofStream
		strin = objInputFile.ReadLine()
		strout = Replace(strin,orig,repl)
		objOutputFile.write(strout & vbCRLF)
		objstdout.write (strout & vbCRLF)
	Loop

	objInputFile.Close
	objOutputFile.Close
End Function

FileReplace "rf.vbs","rf_output.vbs","vbs","c"