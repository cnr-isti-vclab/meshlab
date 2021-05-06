param($pssw, $path, $cert_path="")

if ([string]::IsNullOrEmpty($cert_path)) {
    $cert_path = Join-Path $PSScriptRoot ..\..\..\certificate\certificate.pfx
}

$files = Get-ChildItem $path -include ('*.exe', '*.dll', '*.lib') -Recurse

for ($i=0; $i -lt $files.Count; $i++) {
    $file = $files[$i].FullName
    signtool.exe sign /f $cert_path /p $pssw /t http://timestamp.comodoca.com/authenticode $file
}
