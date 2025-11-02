<################################################################ 
    Usage:
        powershell -NoProfile -ExecutionPolicy Bypass -File .\setup_python_win.ps1 `
            -ProjectDir "C:\path\to\your\project" `
            -RequirementsPath "C:\path\to\your\project\requirements.txt" `
            -EnsurePackages @("numpy==1.26.4","matplotlib>=3.8") `
            -TryInstallNIFPGA:$true

    Notes:
        - Idempotent: re-running will not reinstall if already correct.
        - Prefers winget; falls back to python.org silent installer.
        - Creates/uses venv at <ProjectDir>\.venv and installs packages if provided.
        - Prints [OK ] confirmations for verified steps and a final SUCCESS/FAILURE line.
#
################################################################
################################################################>



################################################################
#
#       0.      INITIALIZATIONS / CONSTANTS / HELPERS...
################################################################
################################################################

#       SCRIPT ARGUMENTS / PARAMETERS.
param(
    [Parameter(Mandatory = $true)]
    [string]$ProjectDir,


    [Parameter(Mandatory = $false)]
    [string]$RequirementsPath = "",


    # Fallback python.org version if winget unavailable
    [Parameter(Mandatory = $false)]
    [string]$PyFallbackVersion = "3.12.0"
)


#       CONFIG / GLOBAL CONSTANTS   [ EDIT HERE ].
$ErrorActionPreference = "Stop"
$CFG = [ordered]@{
#
#               1.      USER-SPECIFIED VALUES [ Settings for the script.  The default values are OK unless you have a specific use-case ].
    verify_stdlib           = $true                         #   [ OPTIONAL ] Set to "$true" for an additional check that the Python standard library is present.
    required_imports        = @("sys","time","json","threading","queue","signal","datetime","argparse","random","typing")
    required_packages       = @("nifpga")                   #   packages that MUST be importable; will attempt pip install then fail if missing
    optional_imports        = @("nifpga")                   #   Keeps future NI-FPGA checks consistent
#
    requirements_filename   = "requirements.txt"            #   Default filename for the Python "requirements.txt" file (inside the root "~/ProjectDir/" directory)
    venv_dir_name           = ".venv"                       #   Virtual-Environment (venv) folder name
#
#
#      
#               2.      HARD-CODED VALUES [ you likely do NOT need to touch these ].
    winget_id               = "Python.Python.3.12"          #   Winget package ID
    py_fallback_version     = "3.12.0"                      #   python.org fallback version
#
    installer_sleep_seconds = 5                             #   Pause after silent installer
    pipe_test_payload       = "hello"                       #   Payload for the unbuffered pipe test
#
    pip_common_args         = @("--disable-pip-version-check","--no-input")
}




#       VALUES FOR TRACKING FINAL STATUS.
$script:__final_ok = $true
$script:__final_err = $null

trap {
    $script:__final_ok = $false
    $script:__final_err = $_.Exception.Message
    Write-Err ("Uncaught error: {0}" -f $script:__final_err)
    continue
}



#
#
################################################################
################################################################    #   [[ END "0. CONSTANTS" ]].






################################################################
#
#
#       1.      HELPER FUNCTION DEFINITIONS...
################################################################
################################################################

#               LOGGING FUNCTIONS.
function Write-Info   ($msg) { Write-Host "[   INFO ] $msg" }
function Write-Warn   ($msg) { Write-Host "[   WARN ] $msg" -ForegroundColor Yellow }
function Write-Ok     ($msg) { Write-Host "[     OK ] $msg" -ForegroundColor Green }
function Write-Err    ($msg) { Write-Host "[  ERROR ] $msg" -ForegroundColor Red }
function Write-Notify { param($a,$b); if ($null -ne $b -and ($a -is [int])) { $msg=$b; if ($a -eq 0) { $color='Green' } elseif ($a -eq 1) { $color='Red' } else { $color='White' } } else { $msg=$a; $color='White' }; Write-Host "[ NOTIFY ]" -ForegroundColor Cyan -NoNewline; Write-Host " " -NoNewline; Write-Host $msg -ForegroundColor $color }




function Test-Command($name) { $null -ne (Get-Command $name -ErrorAction SilentlyContinue) }
function Ensure-Directory($path) {
    if (-not (Test-Path -LiteralPath $path)) {
        New-Item -ItemType Directory -Path $path | Out-Null
    }
}


#   "Detect-Python"     Return hashtable: @{ Found=bool; PyExe=string|null; PyLauncher='py'|$null }.
#
function Detect-Python {
    $result = @{ Found = $false; PyExe = $null; PyLauncher = $null }

    if (Test-Command "py") {
        try {
            $null = (& py -V) 2>$null
            if ($LASTEXITCODE -eq 0) {
                $result.PyLauncher = "py"
                $result.Found = $true
                return $result
            }
        } catch {}
    }

    if (Test-Command "python") {
    try {
        $null = (& python --version) 2>$null
        if ($LASTEXITCODE -eq 0) {
            $result.PyExe = (Get-Command python).Source
            $result.Found = $true
            return $result
        }
    } catch {}
    }

    $candidate = Join-Path $env:LocalAppData "Programs\Python\Python312\python.exe"
    if (Test-Path -LiteralPath $candidate) {
        $result.PyExe = $candidate
        $result.Found = $true
    }

    return $result
}


#   "Ensure-Python312"      Ensure Python 3.12 exists; use winget else python.org.
#
function Ensure-Python312 {
    $det = Detect-Python
    if ($det.Found) {
        Write-Ok "Python already present (py or python found)."
        return $det
    }

    if (Test-Command "winget") {
        Write-Info "Installing Python 3.12 via winget..."
        try {
            $null = winget install --id $($CFG.winget_id) -e --silent --accept-source-agreements --accept-package-agreements
            $det = Detect-Python
            if ($det.Found) {
                Write-Ok "Python installed via winget."
                return $det
            }
            Write-Warn "winget completed but Python not detected; proceeding to fallback."
        } catch {
            Write-Warn "winget install failed; proceeding to fallback."
        }
    } else {
        Write-Warn "winget not available; using python.org fallback."
    }

    # Fallback to python.org silent installer (per-user)
    $fallbackVer = if ($PyFallbackVersion) { $PyFallbackVersion } else { $CFG.py_fallback_version }
    $uri = "https://www.python.org/ftp/python/$fallbackVer/python-$fallbackVer-amd64.exe"
    $tmp = Join-Path $env:TEMP "python-$PyFallbackVersion-amd64.exe"
    Write-Info "Downloading $uri ..."
    $null = Invoke-WebRequest -UseBasicParsing -Uri $uri -OutFile $tmp

    Write-Info "Running silent installer (per-user)..."
    & $tmp /quiet InstallAllUsers=0 PrependPath=1 Include_launcher=1 Include_test=0 SimpleInstall=1 | Out-Null
    Start-Sleep -Seconds $CFG.installer_sleep_seconds

    $det = Detect-Python
    if (-not $det.Found) { throw "Python installation failed (fallback)." }
    Write-Ok "Python installed via python.org fallback."
    return $det
}


#   "Resolve-Python312-Exe"     Resolve the 3.12 interpreter path (or 'py -3.12'); robust to accidental array input.
#
function Resolve-Python312-Exe {
    param([object]$det)

    if ($det -is [object[]]) {
        $det = ($det | Where-Object { $_ -is [hashtable] } | Select-Object -Last 1)
    }
    if ($det -isnot [hashtable]) { throw "Resolve-Python312-Exe: invalid detector object." }

    if ($det.PyLauncher) {
        try {
            $exe = (& py -3.12 -c "import sys; print(sys.executable)")
            if ($exe -and (Test-Path -LiteralPath $exe)) { return $exe }
            $exe2 = (& py -c "import sys; print(sys.executable)")
            if ($exe2 -and (Test-Path -LiteralPath $exe2)) { return $exe2 }
        } catch {}
        return "py -3.12"
    }

    if ($det.PyExe) { return $det.PyExe }

    return (Join-Path $env:LocalAppData "Programs\Python\Python312\python.exe")
}


#   "Ensure-Packages"
#
function Ensure-Packages {
    param(
        [Parameter(Mandatory=$true)][string]  $VenvPy,
        [Parameter(Mandatory=$true)][string[]]$Pkgs
    )
    if (-not $Pkgs -or $Pkgs.Count -eq 0) { return }
    Write-Notify ("Ensuring {0} package(s): {1}" -f $Pkgs.Count, ($Pkgs -join ", "))
    & "$VenvPy" -m pip install @($CFG.pip_common_args + $Pkgs)
    Write-Ok "Ensure-Packages completed."
}


#
#
################################################################
################################################################    #   [[ END "1. FUNCTIONS" ]].






################################################################
#
#
#       2.      MAIN ROUTINE... 
################################################################
################################################################

Write-Notify "Beginning Python setup procedure..."
Ensure-Directory $ProjectDir

$det = Ensure-Python312
$pyToUse = Resolve-Python312-Exe -det $det

$pyIsLauncher = ($pyToUse -is [string]) -and ($pyToUse -like "py*")
if (-not $pyIsLauncher -and -not (Test-Path -LiteralPath $pyToUse)) { throw "Resolved Python path not found: $pyToUse" }

if ($pyIsLauncher) { Write-Info "Using Python via launcher: $pyToUse" } else { Write-Info "Using Python interpreter: $pyToUse" }



#       2.1.    CREATE / ENSURE VIRTUAL ENVIRONMENT (venv).
$venvDir = Join-Path $ProjectDir $CFG.venv_dir_name
$venvPy  = Join-Path $venvDir "Scripts\python.exe"

if (-not (Test-Path -LiteralPath $venvPy)) {
    Write-Info "Creating venv at $venvDir ..."
    if ($pyIsLauncher) {
        cmd /c "$pyToUse -m venv `"$venvDir`""
    } else {
        & "$pyToUse" -m venv "$venvDir"
    }
    if (Test-Path -LiteralPath $venvPy) {
        Write-Ok "venv created at $venvDir"
    } else {
        throw "venv creation failed: $venvPy not found."
    }
} else {
    Write-Ok "Found existing venv."
}


#       2.2.    VERIFY / UPDATE "pip" (idempotent).
Write-Info "Checking pip status..."
$needsPipUpgrade = $false
try {
    $outdatedJson = & "$venvPy" -m pip list --outdated --format=json 2>$null
    if ($outdatedJson) {
        $outdated = $outdatedJson | ConvertFrom-Json
        if ($outdated | Where-Object { $_.name -ieq 'pip' }) { $needsPipUpgrade = $true }
    }
} catch {
    Write-Warn "Could not check pip outdated status; proceeding with safe upgrade attempt."
    $needsPipUpgrade = $true
}

if ($needsPipUpgrade) {
    Write-Info "Upgrading pip in venv..."
    & "$venvPy" -m pip install --upgrade pip | Out-Null
    Write-Ok  "pip up-to-date."
} else {
    Write-Ok  "pip already up-to-date."
}


#       2.3.    [ OPTIONAL ] INSTALL PACKAGES FROM "requirements.txt" (if provided).
if ([string]::IsNullOrWhiteSpace($RequirementsPath)) {
    $defaultReq = Join-Path $ProjectDir $CFG.requirements_filename
    if (Test-Path -LiteralPath $defaultReq) { $RequirementsPath = $defaultReq }
}
if (-not [string]::IsNullOrWhiteSpace($RequirementsPath)) {
    if (Test-Path -LiteralPath $RequirementsPath) {
        Write-Info "Verifying packages from: $RequirementsPath"
        & "$venvPy" -m pip install -r "$RequirementsPath"
        Write-Ok "Requirements verified/installed."
    } else {
        Write-Warn "Requirements file not found at: $RequirementsPath (skipping)."
    }
} else {
    Write-Ok "No requirements provided; package install skipped."
}



#       2.5.    [ OPTIONAL ] ENSURE THE EXPLICIT PACKAGES (if any provided).
if ($EnsurePackages -and $EnsurePackages.Count -gt 0) {
    Ensure-Packages -VenvPy $venvPy -Pkgs $EnsurePackages
}


#       2.6.    VERIFY THE MINIMUM-REQUIRED PACKAGES ARE PRESENT (from the Python stdlib).
#       2.4.    VERIFY THE REQUIRED-PACKAGES ARE INSTALLED (must be importalbe).
foreach ($pkg in $CFG.required_packages) {
    Write-Info ("Verifying required package: {0}" -f $pkg)
    & "$venvPy" -c "import importlib.util,sys; sys.exit(0 if importlib.util.find_spec('$pkg') else 1)"
    $ok = ($LASTEXITCODE -eq 0)
    if (-not $ok) {
        Write-Notify ("Installing '{0}' ..." -f $pkg)
        & "$venvPy" -m pip install @($CFG.pip_common_args + @($pkg))
        & "$venvPy" -c "import importlib.util,sys; sys.exit(0 if importlib.util.find_spec('$pkg') else 1)"
        $ok = ($LASTEXITCODE -eq 0)
    }
    if ($ok) {
        Write-Ok ("'{0}' import verified." -f $pkg)
    } else {
        $script:__final_ok  = $false
        $script:__final_err = ("Required package '{0}' is not importable. Install necessary drivers or wheels, then re-run." -f $pkg)
        Write-Err $script:__final_err
    }
}




#       2.7.    SANITY-TEST     -- TIME AND CHECK PIPE/IPC FUNCTIONALITY.
Write-Info "Running unbuffered pipe test..."
$sw = [System.Diagnostics.Stopwatch]::StartNew()
try {
    $payload = $CFG.pipe_test_payload
    $output  = $payload | & "$venvPy" -u -c "import sys; print('PY:'+sys.stdin.read(), end='')"
    $sw.Stop()
    if ($output -eq ('PY:' + $payload)) {
        Write-Ok ("Pipe test OK in {0} ms (received: {1})" -f $sw.ElapsedMilliseconds, ($output -join ' '))
    } else {
        $script:__final_ok  = $false
        $script:__final_err = "Pipe test unexpected output: " + ($output -join ' ')
        Write-Warn ("Pipe test unexpected output in {0} ms:`n{1}" -f $sw.ElapsedMilliseconds, ($output -join "`n"))
    }
} catch {
    $sw.Stop()
    $script:__final_ok  = $false
    $script:__final_err = $_.Exception.Message
    Write-Warn ("Pipe test failed after {0} ms: {1}" -f $sw.ElapsedMilliseconds, $script:__final_err)
}



#
#
################################################################
################################################################    #   [[ END "2. MAIN ROUTINE" ]].






################################################################
#
#
#       3.      FINAL OPERATIONS...
################################################################
################################################################   

Write-Host ""  # spacer
Write-Info "Done."
Write-Info ("Use this interpreter for CreateProcessW: '{0}'" -f $venvPy)
Write-Info "(Pass -u on the command line and flush in Python for real-time pipe I/O)"
Write-Host ""  # spacer


#   FINAL DIAGNOSTIC MESSAGES...   
#
if ($script:__final_ok) 
{
    Write-Ok  "SUCCESS."
    Write-Notify 0 "setup complete."
    exit 0
} 
else {
    Write-Err    ("FAILURE (code {0})." -f $script:__final_err)
    Write-Notify 1 "setup procedure was incomplete."
    exit 1
}






################################################################
#
#
#
################################################################
################################################################    #   [[ END "3. FINAL OPERATIONS" ]].