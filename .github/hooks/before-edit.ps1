$hookInput = [Console]::In.ReadToEnd()

try {
    $payload = $hookInput | ConvertFrom-Json -ErrorAction Stop
} catch {
    exit 0
}

$toolName = $payload.tool_name
if (-not $toolName) {
    $toolName = $payload.toolName
}

if ($toolName -in @('edit', 'apply_patch', 'create_file')) {
    $message = 'Hello, I am going to edit'
    [Console]::Error.WriteLine($message)
    @{ systemMessage = $message } | ConvertTo-Json -Compress
}