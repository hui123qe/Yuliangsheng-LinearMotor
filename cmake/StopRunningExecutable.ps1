param(
    [Parameter(Mandatory = $true)]
    [string]$ExecutablePath
)

Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;
using System.Text;

public static class NativeProcessPath
{
    private const uint ProcessQueryLimitedInformation = 0x1000;

    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern IntPtr OpenProcess(
        uint desiredAccess,
        bool inheritHandle,
        int processId);

    [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    private static extern bool QueryFullProcessImageName(
        IntPtr processHandle,
        int flags,
        StringBuilder executablePath,
        ref int pathLength);

    [DllImport("kernel32.dll")]
    private static extern bool CloseHandle(IntPtr handle);

    public static string Get(int processId)
    {
        IntPtr processHandle = OpenProcess(
            ProcessQueryLimitedInformation,
            false,
            processId);

        if (processHandle == IntPtr.Zero)
        {
            return null;
        }

        try
        {
            int pathLength = 32768;
            StringBuilder executablePath = new StringBuilder(pathLength);
            return QueryFullProcessImageName(
                processHandle,
                0,
                executablePath,
                ref pathLength)
                ? executablePath.ToString()
                : null;
        }
        finally
        {
            CloseHandle(processHandle);
        }
    }
}
'@

$targetPath = [System.IO.Path]::GetFullPath($ExecutablePath)
$processName = [System.IO.Path]::GetFileNameWithoutExtension($targetPath)
$pathComparison = [System.StringComparison]::OrdinalIgnoreCase

$matchingProcesses = Get-Process -Name $processName -ErrorAction SilentlyContinue | Where-Object {
    try {
        $processPath = $_.Path
        if (-not $processPath) {
            $processPath = [NativeProcessPath]::Get($_.Id)
        }
        $processPath -and [System.IO.Path]::GetFullPath($processPath).Equals($targetPath, $pathComparison)
    }
    catch {
        $false
    }
}

foreach ($runningProcess in $matchingProcesses) {
    $processId = $runningProcess.Id
    $requestedClose = $runningProcess.CloseMainWindow()

    if ($requestedClose -and $runningProcess.WaitForExit(3000)) {
        Write-Host "Closed $targetPath (PID $processId)."
        continue
    }

    Stop-Process -Id $processId -Force -ErrorAction Stop
    Wait-Process -Id $processId -ErrorAction SilentlyContinue
    Write-Host "Stopped $targetPath (PID $processId)."
}
