# LeoFuzzer Result Format

LeoFuzzer writes human-readable summaries first.

Preferred V0.1 outputs:

- summary.txt
- runs.tsv
- crashes/
- timeouts/

## Target Output

Interactive runs may inherit target stdout and stderr.

Machine-readable runs must be quiet and reproducible.

When `--tsv` is used, LeoFuzzer defaults to quiet target output unless explicitly overridden with:

    --target-output inherit

Supported target output modes:

- inherit: child stdout/stderr remain attached to the parent terminal
- quiet: child stdout/stderr are redirected to `/dev/null`

Later versions may add plist output for Leopard-native tooling.
