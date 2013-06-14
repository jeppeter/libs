Dim WshShell, oExec

Function FileReplace(ifname,ofname,orig,repl)
	dim objFSO,objInputFile,objOutputFile
	dim strcomputer
	Set objFSO = CreateObject("Scripting.FileSystemObject")

	Set objInputFile = objFSO.OpenTextFile(ifname,1)
	Set objOutputFile = objFSO.OpenTextFile(ofname,8,True)

	Do until objInputFile.AtEndofStream
		strcomputer = objInputFile.ReadLine
		objOutputFile.write(strcomputer)
	Loop

	objInputFile.Close
	objOutputFile.Close
End Function

FileReplace "rf.vbs","rf_output.vbs","vbs","c"