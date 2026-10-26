# Drives the HCP65 Board Tester (a native Win32 window) with injected mouse and keyboard input and PrintWindow
# screenshots.  Dot-source it in Windows PowerShell 5.1:  . .claude\skills\run-hcp65tester\driver.ps1
#
# Positions on the circuit canvas are grid units (Logicim's: grid 0,0 is the canvas centre, ten pixels a unit at
# zoom 1).  The canvas layout constants assume the window's default 1280x800 client area; Start-Tester checks it.

Add-Type -AssemblyName System.Drawing
if (-not ("HcpU" -as [type])) {
Add-Type @"
using System;
using System.Runtime.InteropServices;
public static class HcpU {
  [StructLayout(LayoutKind.Sequential)] public struct RECT { public int L, T, R, B; }
  [StructLayout(LayoutKind.Sequential)] public struct POINT { public int X, Y; }
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
  [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr h, out RECT r);
  [DllImport("user32.dll")] public static extern bool ClientToScreen(IntPtr h, ref POINT p);
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
  [DllImport("user32.dll")] public static extern bool SetCursorPos(int x, int y);
  [DllImport("user32.dll")] public static extern void mouse_event(uint f, int x, int y, uint d, IntPtr e);
  [DllImport("user32.dll")] public static extern void keybd_event(byte vk, byte scan, uint f, IntPtr e);
  [DllImport("user32.dll")] public static extern uint MapVirtualKey(uint code, uint type);
  [DllImport("user32.dll")] public static extern bool PrintWindow(IntPtr h, IntPtr hdc, uint f);
  [DllImport("user32.dll")] public static extern bool SetProcessDPIAware();
}
"@
}
[HcpU]::SetProcessDPIAware() | Out-Null

$HcpUnit = (Resolve-Path "$PSScriptRoot\..\..\..").Path
# The canvas centre in client pixels (grid 0,0 at zoom 1, unscrolled) and the left toolbar's zoom buttons.
$HcpCentre = @(740, 347)
$HcpZoomIn = @(20, 148)
$HcpResetZoom = @(20, 224)
# Each tool call is a new shell, so the zoom the driver set is kept in a file, not only in the session.
$HcpZoomFile = "$env:TEMP\hcp65-driver-zoom.txt"
$Script:HcpPixelsPerGrid = if (Test-Path $HcpZoomFile) { [int](Get-Content $HcpZoomFile) } else { 10 }
function Save-PixelsPerGrid([int]$Pixels) { $Script:HcpPixelsPerGrid = $Pixels; Set-Content -Path $HcpZoomFile -Value $Pixels }

function Get-TesterWindow { (Get-Process "HCP65 Tester" -ErrorAction Stop | Where-Object { $_.MainWindowHandle -ne 0 } | Select-Object -First 1).MainWindowHandle }

# Launches a built configuration from the project folder (it loads Resources\ relative to it) and waits for the window.
# A tester already running is stopped first, so there's only ever one window to drive.
function Start-Tester([string]$Config = "Debug", [string]$Platform = "x64") {
  if (Get-Process "HCP65 Tester" -ErrorAction SilentlyContinue) { Stop-Tester; Start-Sleep -Seconds 1 }
  $dir = if ($Platform -eq "x64") { "$HcpUnit\x64\$Config" } else { "$HcpUnit\$Config" }
  Start-Process -FilePath "$dir\HCP65 Tester.exe" -WorkingDirectory "$HcpUnit\HCP65Tester" | Out-Null
  for ($i = 0; $i -lt 50; $i++) {
    Start-Sleep -Milliseconds 200
    $p = Get-Process "HCP65 Tester" -ErrorAction SilentlyContinue | Where-Object { $_.MainWindowHandle -ne 0 }
    if ($p) { break }
  }
  Start-Sleep -Seconds 1
  $r = New-Object HcpU+RECT; [HcpU]::GetClientRect((Get-TesterWindow), [ref]$r) | Out-Null
  if (($r.R -ne 1280) -or ($r.B -ne 800)) { Write-Warning "Client area is $($r.R)x$($r.B), not 1280x800: the canvas constants are off." }
  Save-PixelsPerGrid 10
  Focus-Tester
}

function Stop-Tester { Stop-Process -Name "HCP65 Tester" -Confirm:$false -ErrorAction SilentlyContinue }

function Focus-Tester { [HcpU]::SetForegroundWindow((Get-TesterWindow)) | Out-Null; Start-Sleep -Milliseconds 200 }

function Move-Client([int]$x, [int]$y) {
  $p = New-Object HcpU+POINT; $p.X = $x; $p.Y = $y
  [HcpU]::ClientToScreen((Get-TesterWindow), [ref]$p) | Out-Null
  [HcpU]::SetCursorPos($p.X, $p.Y) | Out-Null; Start-Sleep -Milliseconds 60
}
function Click-Client([int]$x, [int]$y) {
  Move-Client $x $y
  [HcpU]::mouse_event(0x2, 0, 0, 0, [IntPtr]::Zero); Start-Sleep -Milliseconds 60
  [HcpU]::mouse_event(0x4, 0, 0, 0, [IntPtr]::Zero); Start-Sleep -Milliseconds 100
}

# Grid units to client pixels at the current zoom.  The zoom buttons zoom about the centre, so it stays put.
function Grid-ToClient([double]$gx, [double]$gy) {
  @([int]($HcpCentre[0] + $Script:HcpPixelsPerGrid * $gx), [int]($HcpCentre[1] + $Script:HcpPixelsPerGrid * $gy))
}

# Zoom 1, then up by whole steps: Set-Zoom 2 is twenty pixels a grid unit.
function Set-Zoom([int]$Zoom) {
  Click-Client $HcpResetZoom[0] $HcpResetZoom[1]
  for ($i = 1; $i -lt $Zoom; $i++) { Click-Client $HcpZoomIn[0] $HcpZoomIn[1] }
  Save-PixelsPerGrid (10 * $Zoom)
}

function Move-Grid([double]$gx, [double]$gy) { $c = Grid-ToClient $gx $gy; Move-Client $c[0] $c[1] }
function Click-Grid([double]$gx, [double]$gy) { $c = Grid-ToClient $gx $gy; Click-Client $c[0] $c[1] }

# Button one held through each grid point in turn, in small steps, as a wire is drawn: Drag-Grid @(-7,-3) @(-3,-3)
function Drag-Grid {
  $pts = $args
  Move-Grid $pts[0][0] $pts[0][1]
  [HcpU]::mouse_event(0x2, 0, 0, 0, [IntPtr]::Zero); Start-Sleep -Milliseconds 60
  for ($i = 1; $i -lt $pts.Count; $i++) {
    $a = $pts[$i - 1]; $b = $pts[$i]
    for ($s = 1; $s -le 8; $s++) { Move-Grid ($a[0] + ($b[0] - $a[0]) * $s / 8) ($a[1] + ($b[1] - $a[1]) * $s / 8) }
  }
  [HcpU]::mouse_event(0x4, 0, 0, 0, [IntPtr]::Zero); Start-Sleep -Milliseconds 100
}

# A key by virtual key code.  Raw input reads the scan code, so it must be sent; extended keys (Delete, arrows) need -Extended.
function Send-VKey([byte]$Vk, [switch]$Shift, [switch]$Extended) {
  $sc = [byte][HcpU]::MapVirtualKey($Vk, 0); $shiftSc = [byte][HcpU]::MapVirtualKey(0x10, 0)
  $ext = if ($Extended) { 1 } else { 0 }
  if ($Shift) { [HcpU]::keybd_event(0x10, $shiftSc, 0, [IntPtr]::Zero); Start-Sleep -Milliseconds 30 }
  [HcpU]::keybd_event($Vk, $sc, $ext, [IntPtr]::Zero); Start-Sleep -Milliseconds 30
  [HcpU]::keybd_event($Vk, $sc, $ext -bor 2, [IntPtr]::Zero); Start-Sleep -Milliseconds 30
  if ($Shift) { [HcpU]::keybd_event(0x10, $shiftSc, 2, [IntPtr]::Zero) }
  Start-Sleep -Milliseconds 100
}

# A letter, shifted if it's a capital: Send-Char 'F' starts placing a flip flop at the pointer.
function Send-Char([char]$c) { Send-VKey ([byte][char]::ToUpper($c)) -Shift:([char]::IsUpper($c)) }

function Send-Delete { Send-VKey 0x2E -Extended }

# Places a component by its canvas key (see CCircuitCanvas::Printable) at a grid point.
function Place-Grid([char]$Key, [double]$gx, [double]$gy) {
  Move-Grid $gx $gy; Send-Char $Key
  Move-Grid ($gx + 1) $gy; Move-Grid $gx $gy
  Click-Grid $gx $gy
}

# The whole window, frame included, as a PNG.  The client area starts 8 pixels in and 31 down in the image.
function Save-Screenshot([string]$Path) {
  $h = Get-TesterWindow; $r = New-Object HcpU+RECT; [HcpU]::GetWindowRect($h, [ref]$r) | Out-Null
  $bmp = New-Object System.Drawing.Bitmap ($r.R - $r.L), ($r.B - $r.T)
  $g = [System.Drawing.Graphics]::FromImage($bmp); $hdc = $g.GetHdc()
  [HcpU]::PrintWindow($h, $hdc, 2) | Out-Null
  $g.ReleaseHdc($hdc); $g.Dispose(); $bmp.Save($Path); $bmp.Dispose()
}
