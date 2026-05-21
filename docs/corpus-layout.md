# LeoFuzzer Corpus Layout

A corpus is a directory containing input files for one target.

V0.1 corpus traversal is intentionally simple:

- one directory
- regular files only
- no recursion
- stable sorted order
- hidden files are ignored

Hidden files are ignored so repository scaffolding files such as `.gitkeep` do not become accidental test inputs.

## Example

    corpus/
      samples/
        hello.txt
        crash.txt
        timeout.txt

Run:

    leofuzz --target bin/echo-target --corpus corpus/samples

Machine-readable run:

    leofuzz --target bin/echo-target --corpus corpus/samples --tsv

## Future Work

Later versions may add:

- recursive traversal
- include/exclude filters
- explicit hidden-file handling
- corpus class directories such as valid, invalid, edge, regression
- per-target suite files
