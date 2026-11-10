# File Compression and Decompression Tool (DSA)

A C++ mini project that uses **Huffman Coding** for lossless file compression and decompression.

## DSA Concepts
- Greedy algorithm
- Binary tree
- Min-priority queue
- Hash map
- File handling

## Features
- Compress a file into a custom `.huff`-style binary file
- Restore the original file from the compressed file
- Displays original and compressed file sizes
- Handles binary input files

## How to Compile
Requires a C++17 compiler.

```bash
g++ -std=c++17 src/main.cpp -o compression_tool
```

## How to Run
```bash
./compression_tool
```
On Windows:
```powershell
g++ -std=c++17 src/main.cpp -o compression_tool.exe
.\compression_tool.exe
```

Choose `1` to compress or `2` to decompress, then enter the input and output paths.

## Example
- Input: `samples/sample.txt`
- Compression output: `output/sample.huff`
- Decompression output: `output/restored.txt`

## File Format
The compressed file stores the original byte count, a 256-entry frequency table, and the Huffman-encoded bit stream. This makes decoding possible without a separate code table.

## Project Structure
```text
File-Compression-Tool-DSA/
├── src/main.cpp
├── samples/sample.txt
├── output/              # generated files go here
├── README.md
└── .gitignore
```

## Limitations
- The program uses a simple custom format intended for learning and demonstration.
- Compression may not reduce size for small or high-entropy files because of the frequency-table header.
