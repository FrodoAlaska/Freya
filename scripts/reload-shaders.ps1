# Iterate through every shader

foreach ($file in Get-ChildItem -Path "..\assets\shaders\" -File) {
  # TODO: Perhaps have different shaders outputing to different directories
  sokol-shdc -i $file.FullName -o "..\freya\src\renderer\shaders\$($file.BaseName).h" -l glsl430:glsl300es --no-log-cmdline
}
