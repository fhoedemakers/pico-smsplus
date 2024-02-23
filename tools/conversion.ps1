# Description: This script converts all .sms files in the current directory to a custom format.
# The custom format consists of a 32-bit header containing the size of the file in bytes, followed by the contents of the file.
# Create 'converted' directory if it doesn't exist
if (-not (Test-Path -Path "./converted" -PathType Container)) {
    New-Item -ItemType Directory -Path "./converted"
}

# Get all *.sms files in the current directory
$smsFiles = Get-ChildItem -Path . -Filter *.sms

foreach ($file in $smsFiles) {
    # Get the size of the file
    $fileSize = (Get-Item $file.FullName).Length

    # Create destination filename in the 'converted' directory
    $destinationFile = "./converted/" + $file.Name

    # Write the file size as a 32-bit header to the destination file
    $fileSizeBytes = [BitConverter]::GetBytes($fileSize)
    Set-Content -Path $destinationFile -Value $fileSizeBytes -Encoding Byte -NoNewline

    # Append the contents of the source file to the destination file
    Add-Content -Path $destinationFile -Value (Get-Content $file.FullName -Raw) -Encoding Byte

    Write-Host "File copied successfully: $($file.Name)"
}

Write-Host "All files copied successfully."

