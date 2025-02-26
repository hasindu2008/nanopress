# Lossless Nanopore Compression

This is a fork of https://github.com/sashajenner/honours, which is the repository associated [Sasha Jenner's final year undergraduate Computer Science honours project for the
University of Sydney](https://github.com/sashajenner/honours/blob/master/thesis/pdf/sasha_thesis_final.pdf). This project focused on lossless compression of nanopore data and the data used were nanopore R9.4.1 chemistry. 
The purpose of this fork is to evaluate different compression methods on R10.4.1 data.

From the readme of the original repository:

## Objective

Design lossless compression methods with better space saving than the
state-of-the-art; zstd-svb-zd (a.k.a VBZ).

## Contributions

- First systematic analysis of nanopore data
- New state-of-the-art
- First comprehensive benchmark of existing and novel methods

## Data

A downsampled human DNA data set (NA12878) with 500 000 reads was used for
analysis and benchmarking.

Download: https://slow5.page.link/na12878_prom_sub_slow5.

## Benchmark

Sequential read compression and decompression is performed. To ensure the
methods are lossless, the decompressed data is compared to the uncompressed
data for equality.

The following metrics are recorded:
- Compressed size
- Compression time
- Decompression time

1. Compile the benchmark.
```
	make -C press
```
2. Run it on a data set.
```
	cd press
	./test SLOW5_DATA
```
Or, use the example data set with 3 reads.
```
	./test ../data/three-reads.blow5
```

Some tests done on R10.4.1 data:

## Benchmark on R10.4.1 data

See [here](docs/r10.4.1-b5925335.md).

