# LeoFuzzer Corpus Layout

A corpus is a directory containing input files for one target.

V0.1 corpus traversal is intentionally simple:

- one directory
- regular files only
- no recursion
- stable sorted order
- hidden files are allowed if they are regular files

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
- corpus class directories such as valid, invalid, edge, regression
- per-target suite files
