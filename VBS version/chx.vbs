Dim objFSO
Set objFSO = CreateObject("Scripting.FileSystemObject")

Dim newext
newext = InputBox(objFSO.GetBaseName(Wscript.Arguments(0)) & "." & objFSO.GetExtensionName(Wscript.Arguments(0)) & vbCrLf & vbCrLf & "New extension:","Change file extension", objFSO.GetExtensionName(Wscript.Arguments(0)))

If newext = "" Then
	If MsgBox("Remove file extension?", vbYesNo Or vbDefaultButton2, "Change file extension")=vbNo Then Wscript.Quit
Else
	newext = "." & newext
End If

'WScript.Echo "newext=<" & newext & ">"

For Each strArg in Wscript.Arguments
	Dim newpath
	newpath=objFSO.GetParentFolderName(strArg) & "\" & objFSO.GetBaseName(strArg) & newext
	'WScript.Echo newpath
	If objFSO.GetExtensionName(strArg)<>objFSO.GetExtensionName(newpath) Then objFSO.MoveFile strArg, newpath
Next