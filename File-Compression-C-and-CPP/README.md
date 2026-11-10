# File Compression Tool (C & C++)
Huffman Coding based file compression and decompression project.

## Compile
C: `gcc C/main.c -o huffman_c`
C++: `g++ -std=c++17 CPP/main.cpp -o huffman_cpp`

Run the executable, choose compress/decompress, and enter input/output file paths.

## Concepts
Huffman Coding, Greedy Algorithm, Binary Tree, Min Heap/Priority Queue, File Handling.

The binary format stores original size and a 256-entry frequency table before the encoded bits. This is a learning project; small files may grow due to the header.
