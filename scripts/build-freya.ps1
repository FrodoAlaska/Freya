### Variables ###

$working_dir = Get-Location

$build_config = "Debug" 
$build_dir    = "..\build"
$build_flags  = ""

$can_run_testbed = $false 

### Variables ###

### Functions ###

function Log-Msg {
  param (
    [string]$msg,
    [string]$log_level 
  )

  $color = "White"
  switch ($log_level) {
    "WARN"  { $color = "Yellow" }
    "INFO"  { $color = "Blue" }
    "ERROR" { $color = "Red" }
    Default {}
  }

  Write-Host $msg -Foregroundcolor $color
}

function Show-Help {
  Log-Msg -msg "[Usage]: .\build-freya.ps1 [options]"                      -log_level "WARN"
  Log-Msg -msg "An easy to use build script to build Freya on Windows"     -log_level "WARN"
  Log-Msg -msg "   --clean          = Have a new fresh build"              -log_level "WARN"
  Log-Msg -msg "   --debug          = Build for the debug configuration"   -log_level "WARN"
  Log-Msg -msg "   --rel            = Build for the release configuration" -log_level "WARN"
  Log-Msg -msg "   --jobs [threads] = Threads to use when building"        -log_level "WARN"
  Log-Msg -msg "   --run-testbed    = Run the testbed examples"            -log_level "WARN"
  Log-Msg -msg "   --help           = Display this help message"           -log_level "WARN"
}

function Check-Build-Dir {
  param (
    [string]$dir
  )

  # The directory exists. Everything is good
  
  if(Test-Path $dir) {
    $build_dir = $dir
    return
  }

  # Create the directory otherwise
  
  mkdir $dir
  $build_dir = $dir
  
  cmake -S ..\ -B $dir
  Check-Exit-Code -msg "Failed to generate CMake files..."
}

function Check-Exit-Code {
  param (
    [string]$msg
  )

  if($LASTEXITCODE -ne 0) {
    Log-Msg -msg "[ERROR]: $msg" -log_level "ERROR" 
    cd $working_dir
    exit $LASTEXITCODE
  }
}

### Functions ###

### Parse the arguments ### 

# No arguments given
if ($Args.Count -le 1) {
  Show-Help
  exit 1
}

for ($i = 0; $i -lt $Args.Count; $i++) {
  $arg = $($args[$i]);
  
  switch ($arg) {
    "--clean"       { $build_flags += "--clean-first" }
    "--debug"       { Check-Build-Dir $build_dir; $build_config = "Debug" }
    "--rel"         { Check-Build-Dir $build_dir; $build_config = "Release" }
    "--jobs"        { $i++; $build_flags += "--parallel $($args[$i])" }
    "--run-testbed" { $can_run_testbed = $true }
    "--help"        { Show-Help; exit 1 }
    Default { 
      Log-Msg -msg "Invalid argument '$arg' passed" -log_level "ERROR"; 
      Show-Help; 
      exit 1
    }
  }
}

### Parse the arguments ### 

### Build ### 

Log-Msg -msg "Building for the '$build_config' configuration in directory '$build_dir'" -log_level "INFO"
cd $build_dir

cmake --build .\ --config $build_config $build_flags 
Check-Exit-Code -msg "Failed to build.."

cd $working_dir

### Build ### 

### Run ### 

if ($can_run_testbed) {
  # Check if the testbed's directory exists

  if(-Not (Test-Path $build_dir\testbed)) {
    mkdir $build_dir\testbed
    cmake -S ..\testbed\ -B $build_dir\testbed
  
    Check-Exit-Code -msg "Failed to generate CMake files for the testbed..."
  }

  # Run the testbed
  
  cd $build_dir\testbed
  & ".\$build_config\freya_testbed.exe"
  cd $working_dir

  # Some very professional error checking
  Check-Exit-Code -msg "Failed to run testbed..."
}

### Run ### 
