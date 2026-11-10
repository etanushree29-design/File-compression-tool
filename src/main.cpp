#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <string>
#include <bitset>
#include <cstdint>
#include <limits>
#include <filesystem>

using namespace std;

// Huffman tree node
struct Node {
    unsigned char ch;
    uint64_t freq;
    Node* left;
    Node* right;

    Node(unsigned char c, uint64_t f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
    Node(uint64_t f, Node* l, Node* r) : ch(0), freq(f), left(l), right(r) {}

    bool isLeaf() const { return left == nullptr && right == nullptr; }
};

struct Compare {
    bool operator()(Node* a, Node* b) const {
        return a->freq > b->freq; // min-priority queue
    }
};

void deleteTree(Node* node) {
    if (!node) return;
    deleteTree(node->left);
    deleteTree(node->right);
    delete node;
}

void buildCodes(Node* root, const string& prefix,
                unordered_map<unsigned char, string>& codes) {
    if (!root) return;
    if (root->isLeaf()) {
        codes[root->ch] = prefix.empty() ? "0" : prefix;
        return;
    }
    buildCodes(root->left, prefix + "0", codes);
    buildCodes(root->right, prefix + "1", codes);
}

Node* buildTree(const vector<uint64_t>& frequencies) {
    priority_queue<Node*, vector<Node*>, Compare> pq;
    for (int i = 0; i < 256; ++i) {
        if (frequencies[i] > 0)
            pq.push(new Node(static_cast<unsigned char>(i), frequencies[i]));
    }
    if (pq.empty()) return nullptr;

    while (pq.size() > 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();
        pq.push(new Node(left->freq + right->freq, left, right));
    }
    return pq.top();
}

bool compressFile(const string& inputPath, const string& outputPath) {
    ifstream in(inputPath, ios::binary);
    if (!in) {
        cerr << "Error: Cannot open input file.\n";
        return false;
    }

    vector<unsigned char> data((istreambuf_iterator<char>(in)), {});
    vector<uint64_t> freq(256, 0);
    for (unsigned char c : data) ++freq[c];

    Node* root = buildTree(freq);
    unordered_map<unsigned char, string> codes;
    buildCodes(root, "", codes);

    ofstream out(outputPath, ios::binary);
    if (!out) {
        cerr << "Error: Cannot create output file.\n";
        deleteTree(root);
        return false;
    }

    // File format: 8-byte original size, 256 frequency values (8 bytes each), then encoded bits.
    uint64_t originalSize = static_cast<uint64_t>(data.size());
    out.write(reinterpret_cast<const char*>(&originalSize), sizeof(originalSize));
    for (uint64_t f : freq)
        out.write(reinterpret_cast<const char*>(&f), sizeof(f));

    unsigned char byte = 0;
    int bitCount = 0;
    for (unsigned char c : data) {
        for (char bit : codes[c]) {
            byte = static_cast<unsigned char>((byte << 1) | (bit == '1'));
            ++bitCount;
            if (bitCount == 8) {
                out.put(static_cast<char>(byte));
                byte = 0;
                bitCount = 0;
            }
        }
    }
    if (bitCount > 0) {
        byte <<= (8 - bitCount); // pad remaining bits with zero
        out.put(static_cast<char>(byte));
    }

    out.close();
    deleteTree(root);

    cout << "Compression completed.\n";
    cout << "Original size: " << originalSize << " bytes\n";
    cout << "Compressed size: " << (filesystem::exists(outputPath) ? filesystem::file_size(outputPath) : 0)
         << " bytes (includes header)\n";
    return true;
}

bool decompressFile(const string& inputPath, const string& outputPath) {
    ifstream in(inputPath, ios::binary);
    if (!in) {
        cerr << "Error: Cannot open compressed file.\n";
        return false;
    }

    uint64_t originalSize = 0;
    in.read(reinterpret_cast<char*>(&originalSize), sizeof(originalSize));
    if (!in) {
        cerr << "Error: Invalid compressed file.\n";
        return false;
    }

    vector<uint64_t> freq(256, 0);
    for (uint64_t& f : freq) {
        in.read(reinterpret_cast<char*>(&f), sizeof(f));
        if (!in) {
            cerr << "Error: Invalid compressed file header.\n";
            return false;
        }
    }

    Node* root = buildTree(freq);
    ofstream out(outputPath, ios::binary);
    if (!out) {
        cerr << "Error: Cannot create output file.\n";
        deleteTree(root);
        return false;
    }

    if (originalSize == 0) {
        out.close();
        deleteTree(root);
        cout << "Decompression completed (empty file).\n";
        return true;
    }
    if (!root) {
        cerr << "Error: Missing Huffman tree.\n";
        deleteTree(root);
        return false;
    }

    Node* current = root;
    uint64_t written = 0;
    char raw;
    while (in.get(raw) && written < originalSize) {
        unsigned char byte = static_cast<unsigned char>(raw);
        for (int i = 7; i >= 0 && written < originalSize; --i) {
            int bit = (byte >> i) & 1;
            current = bit == 0 ? current->left : current->right;
            if (!current) {
                cerr << "Error: Corrupted compressed data.\n";
                deleteTree(root);
                return false;
            }
            if (current->isLeaf()) {
                out.put(static_cast<char>(current->ch));
                ++written;
                current = root;
            }
        }
    }

    out.close();
    deleteTree(root);
    if (written != originalSize) {
        cerr << "Warning: Output may be incomplete.\n";
        return false;
    }
    cout << "Decompression completed.\n";
    cout << "Restored size: " << written << " bytes\n";
    return true;
}

int main() {
    cout << "=== File Compression Tool (Huffman Coding) ===\n";
    cout << "1. Compress file\n2. Decompress file\nEnter choice: ";
    int choice;
    cin >> choice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    string inputPath, outputPath;
    cout << "Enter input file path: ";
    getline(cin, inputPath);
    cout << "Enter output file path: ";
    getline(cin, outputPath);

    bool success = false;
    if (choice == 1)
        success = compressFile(inputPath, outputPath);
    else if (choice == 2)
        success = decompressFile(inputPath, outputPath);
    else
        cout << "Invalid choice.\n";

    return success ? 0 : 1;
}
