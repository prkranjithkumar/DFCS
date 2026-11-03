<#
.SYNOPSIS
Validates a DFCS unit test spec CSV against the schema and (optionally) its design document.

.EXAMPLE
./Test-UnitTestSpec.ps1 -Path docs/test-specs/ST-005-unit-tests.csv
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Path,
    [string]$Design,
    [string]$Schema
)

$ErrorActionPreference = 'Stop'
if (-not $Schema) { $Schema = Join-Path (Split-Path -Parent $MyInvocation.MyCommand.Path) '..\schemas\unit-test-spec.schema.json' }
$errs = New-Object System.Collections.Generic.List[string]
$warns = New-Object System.Collections.Generic.List[string]
function Add-Err([string]$m) { $script:errs.Add($m) }
function Add-Warn([string]$m) { $script:warns.Add($m) }

function Read-Rfc4180([string]$Text) {
    $rows = New-Object System.Collections.Generic.List[object]
    $fields = New-Object System.Collections.Generic.List[string]
    $sb = New-Object System.Text.StringBuilder
    $inQ = $false; $hasNl = $false; $pending = $false
    for ($i = 0; $i -lt $Text.Length; $i++) {
        $c = $Text[$i]
        if ($inQ) {
            if ($c -eq '"') {
                if ($i + 1 -lt $Text.Length -and $Text[$i + 1] -eq '"') { [void]$sb.Append('"'); $i++ }
                else { $inQ = $false }
            } else {
                if ($c -eq "`n" -or $c -eq "`r") { $hasNl = $true }
                [void]$sb.Append($c)
            }
        } elseif ($c -eq '"') { $inQ = $true; $pending = $true }
        elseif ($c -eq ',') { $fields.Add($sb.ToString()); [void]$sb.Clear(); $pending = $true }
        elseif ($c -eq "`r") { }
        elseif ($c -eq "`n") {
            $fields.Add($sb.ToString()); [void]$sb.Clear()
            $rows.Add([pscustomobject]@{ Fields = $fields.ToArray(); HasNewline = $hasNl })
            $fields = New-Object System.Collections.Generic.List[string]; $hasNl = $false; $pending = $false
        } else { [void]$sb.Append($c); $pending = $true }
    }
    if ($inQ) { Add-Err 'Unterminated quoted field at end of file' }
    if ($pending -or $sb.Length -gt 0 -or $fields.Count -gt 0) {
        $fields.Add($sb.ToString())
        $rows.Add([pscustomobject]@{ Fields = $fields.ToArray(); HasNewline = $hasNl })
    }
    , $rows
}

if (-not (Test-Path -LiteralPath $Path)) { Write-Error "CSV not found: $Path" }
$schemaDoc = Get-Content -LiteralPath $Schema -Raw -Encoding UTF8 | ConvertFrom-Json
$cols = @($schemaDoc.columns)
$names = @($cols | ForEach-Object { $_.name })

$fileName = Split-Path $Path -Leaf
if ($fileName -cnotmatch $schemaDoc.file.namePattern) { Add-Err "File name '$fileName' does not match $($schemaDoc.file.namePattern)" }
$fileStory = ($fileName -split '-unit-tests')[0]

$bytes = [System.IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $Path))
if ($bytes.Length -ge 3 -and $bytes[0] -eq 0xEF -and $bytes[1] -eq 0xBB -and $bytes[2] -eq 0xBF) { Add-Warn 'UTF-8 BOM present; use UTF-8 without BOM' }
$text = [System.Text.Encoding]::UTF8.GetString($bytes).TrimStart([char]0xFEFF)
$rawRows = Read-Rfc4180 $text

if ($rawRows.Count -eq 0) { Add-Err 'File is empty'; $rawRows = @() }
else {
    $header = $rawRows[0].Fields
    if (($header -join '|') -cne ($names -join '|')) { Add-Err "Header mismatch. Expected: $($names -join ',')" }
}

$rows = New-Object System.Collections.Generic.List[object]
for ($r = 1; $r -lt $rawRows.Count; $r++) {
    $f = $rawRows[$r].Fields
    $line = $r + 1
    if ($f.Count -eq 1 -and [string]::IsNullOrWhiteSpace($f[0])) { Add-Err "Row ${line}: blank row"; continue }
    if ($f[0].TrimStart().StartsWith('#')) { Add-Err "Row ${line}: comment rows are not allowed"; continue }
    if ($f.Count -ne $names.Count) { Add-Err "Row ${line}: $($f.Count) columns, expected $($names.Count)"; continue }
    if ($rawRows[$r].HasNewline) { Add-Err "Row ${line}: newline inside a cell" }
    $h = [ordered]@{ _line = $line }
    for ($c = 0; $c -lt $names.Count; $c++) { $h[$names[$c]] = $f[$c] }
    $rows.Add([pscustomobject]$h)
}

foreach ($row in $rows) {
    $ln = $row._line
    $level = $row.Level
    foreach ($col in $cols) {
        $v = [string]$row.($col.name)
        $empty = [string]::IsNullOrWhiteSpace($v)
        $need = ($col.required -eq 'always') -or ($col.required -eq 'unit' -and $level -eq 'unit')
        if ($need -and $empty) { Add-Err "Row ${ln} [$($row.'TC ID')]: '$($col.name)' is required"; continue }
        if ($level -eq 'deferred' -and $col.emptyWhenDeferred -and -not $empty) { Add-Err "Row ${ln} [$($row.'TC ID')]: '$($col.name)' must be empty for deferred rows" }
        if ($empty) { continue }
        if ($v -ne $v.Trim()) { Add-Warn "Row ${ln}: '$($col.name)' has leading/trailing whitespace" }
        if ($col.enum -and ($col.enum -cnotcontains $v)) { Add-Err "Row ${ln}: '$($col.name)' = '$v' not in [$($col.enum -join ', ')]" }
        if ($col.pattern -and $v -cnotmatch $col.pattern) { Add-Err "Row ${ln}: '$($col.name)' = '$v' does not match $($col.pattern)" }
    }
    if ($row.'Story ID' -and $fileStory -and $row.'Story ID' -cne $fileStory) { Add-Err "Row ${ln}: Story ID '$($row.'Story ID')' differs from file name story '$fileStory'" }
    if ($level -eq 'unit') {
        if ($row.'Case ID' -and -not $row.'Case ID'.StartsWith("$($row.'TC ID').")) { Add-Err "Row ${ln}: Case ID '$($row.'Case ID')' does not start with '$($row.'TC ID').'" }
        if ($row.Type -eq 'fault' -and $row.'Fault Injection' -ne 'Y') { Add-Err "Row ${ln} [$($row.'Case ID')]: Type=fault requires Fault Injection=Y" }
        if ($row.'Safety Path' -and $row.'Fault Injection' -ne 'Y') { Add-Err "Row ${ln} [$($row.'Case ID')]: Safety Path set requires Fault Injection=Y" }
        if ($row.Expected -match '(?i)works? correctly|behaves? as expected|is fine') { Add-Warn "Row ${ln} [$($row.'Case ID')]: Expected is not exact/observable" }
        if ($row.Mocks -and $row.Mocks -ne 'none' -and $row.Mocks -notmatch '\((mock|fake|stub)\)') { Add-Warn "Row ${ln} [$($row.'Case ID')]: Mocks entries should be Name(mock|fake|stub); got '$($row.Mocks)'" }
    }
}

$unitRows = @($rows | Where-Object { $_.Level -eq 'unit' })
foreach ($key in 'Case ID', 'Test Name') {
    $unitRows | Group-Object { $_.$key } | Where-Object { $_.Name -and $_.Count -gt 1 } |
        ForEach-Object { Add-Err "Duplicate $key '$($_.Name)' (rows $(($_.Group | ForEach-Object { $_._line }) -join ', '))" }
}
$unitRows | Group-Object { $_.'TC ID' } | ForEach-Object {
    $types = @($_.Group | ForEach-Object { $_.Type })
    foreach ($t in 'nominal', 'boundary', 'invalid') {
        if ($types -notcontains $t) { Add-Err "$($_.Name): no '$t' case" }
    }
}

# Design cross-check
if (-not $Design) {
    $dir = Join-Path (Split-Path (Resolve-Path -LiteralPath $Path) -Parent) '..\design'
    if ($fileStory -and (Test-Path $dir)) { $Design = (Get-ChildItem $dir -Filter "$fileStory-*.md" | Select-Object -First 1).FullName }
}
if (-not $Design -or -not (Test-Path -LiteralPath $Design)) { Add-Warn 'Design document not found; coverage checks skipped' }
else {
    $d = Get-Content -LiteralPath $Design -Encoding UTF8
    function Get-Block([string[]]$Lines, [string]$Start, [string]$End) {
        $s = -1
        for ($i = 0; $i -lt $Lines.Count; $i++) {
            if ($s -lt 0 -and $Lines[$i] -match $Start) { $s = $i; continue }
            if ($s -ge 0 -and $Lines[$i] -match $End) { return $Lines[$s..($i - 1)] }
        }
        if ($s -ge 0) { return $Lines[$s..($Lines.Count - 1)] }
        @()
    }
    $sec6 = Get-Block $d '^##\s+6\.' '^##\s+\d+\.'
    $sec2 = Get-Block $d '^##\s+2\.' '^##\s+\d+\.'
    $sec21 = Get-Block $d '^###\s+2\.1' '^###?\s'
    if ($sec21.Count -eq 0) { $sec21 = $sec2 }
    if ($sec6.Count -eq 0) { Add-Err 'Design section 6 not found' }

    $tbl = @($sec6 | Where-Object { $_ -match '^\s*\|' })
    $hdr = if ($tbl.Count) { @($tbl[0].Trim().Trim('|') -split '\|' | ForEach-Object { $_.Trim().ToLower() }) } else { @() }
    $fiIdx = [array]::IndexOf($hdr, ($hdr | Where-Object { $_ -match 'fault injection' } | Select-Object -First 1))
    $fwIdx = [array]::IndexOf($hdr, ($hdr | Where-Object { $_ -match 'framework' } | Select-Object -First 1))
    $designTcs = @{}
    foreach ($l in $tbl) {
        $cells = @($l.Trim().Trim('|') -split '\|' | ForEach-Object { $_.Trim() })
        if ($cells[0] -cmatch '(TC-[A-Z]+-\d{3})') {
            $tcKey = $Matches[1]
            $designTcs[$tcKey] = [pscustomobject]@{
                Fault = ($fiIdx -ge 0 -and $cells.Count -gt $fiIdx -and $cells[$fiIdx] -match '^Yes')
                Framework = if ($fwIdx -ge 0 -and $cells.Count -gt $fwIdx) { $cells[$fwIdx] } else { '' }
            }
        }
    }
    $csvTcs = @($rows | ForEach-Object { $_.'TC ID' } | Sort-Object -Unique)
    foreach ($tc in $designTcs.Keys) {
        if ($csvTcs -notcontains $tc) { Add-Err "Design TC $tc has no row in the CSV" }
        $tcRows = @($unitRows | Where-Object { $_.'TC ID' -eq $tc })
        if ($designTcs[$tc].Fault -and $tcRows.Count -gt 0 -and -not ($tcRows | Where-Object { $_.'Fault Injection' -eq 'Y' })) { Add-Err "${tc}: design marks fault injection but no row has Fault Injection=Y" }
        if ($designTcs[$tc].Framework -match 'pytest' -and $tcRows.Count -gt 0) { Add-Warn "${tc}: design framework is pytest; expected Level=deferred" }
    }
    foreach ($tc in $csvTcs) {
        if (-not $designTcs.ContainsKey($tc)) {
            $flagged = @($rows | Where-Object { $_.'TC ID' -eq $tc -and $_.Notes -match '(?i)\bnew\b' })
            if ($flagged.Count -eq 0) { Add-Err "${tc}: not in design section 6 and not flagged 'new' in Notes" }
        }
    }
    $csvReqs = @($rows | ForEach-Object { $_.Requirements -split ';' } | Where-Object { $_ } | Sort-Object -Unique)
    $firstCells = @($sec21 | Where-Object { $_ -match '^\s*\|' } | ForEach-Object { ($_.Trim().Trim('|') -split '\|')[0] })
    $reqs21 = @([regex]::Matches(($firstCells -join "`n"), '\b(?:FR|NFR)-\d{3}\b') | ForEach-Object { $_.Value } | Sort-Object -Unique)
    $reqs2 = @([regex]::Matches(($sec2 -join "`n"), '\b(?:FR|NFR)-\d{3}\b') | ForEach-Object { $_.Value } | Sort-Object -Unique)
    foreach ($q in $reqs21) { if ($csvReqs -notcontains $q) { Add-Err "Design requirement $q is not mapped to any case" } }
    foreach ($q in $csvReqs) { if ($reqs2 -notcontains $q) { Add-Warn "$q is not in design section 2" } }
}

$warns | ForEach-Object { Write-Host "WARN  $_" -ForegroundColor Yellow }
$errs | ForEach-Object { Write-Host "ERROR $_" -ForegroundColor Red }
Write-Host ("Rows: {0} (unit {1}, deferred {2}) | Errors: {3} | Warnings: {4}" -f $rows.Count, $unitRows.Count, ($rows.Count - $unitRows.Count), $errs.Count, $warns.Count)
if ($errs.Count -gt 0) { exit 1 }
