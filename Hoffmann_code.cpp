#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <string>
#include <vector>
#include <bitset>
using namespace std;
// BinaryTree Node
class BinaryTree {
public:
    char value;
    int frequency;
    BinaryTree* left;
    BinaryTree* right;

    BinaryTree(char val, int freq) : value(val), frequency(freq), left(nullptr), right(nullptr) {}

    bool operator>(const BinaryTree& other) const {
        return frequency > other.frequency;
    }
};

// HuffmanCode class
class HuffmanCode {
private:
    string path;
    priority_queue<BinaryTree*, vector<BinaryTree*>, greater<>> minHeap;
   unordered_map<char, string> huffmanCode;
    unordered_map<string, char> reverseHuffmanCode;

    unordered_map<char, int> frequencyFromText(const string& text) {
       unordered_map<char, int> frequencyMap;
        for (char ch : text) {
            frequencyMap[ch]++;
        }
        return frequencyMap;
    }

    void buildHeap(const unordered_map<char, int>& frequencyMap) {
    for (const auto& pair : frequencyMap) {
        char key = pair.first;
        int frequency = pair.second;
        minHeap.push(new BinaryTree(key, frequency));
    }
}

    BinaryTree* buildBinaryTree() {
        while (minHeap.size() > 1) {
            BinaryTree* node1 = minHeap.top(); minHeap.pop();
            BinaryTree* node2 = minHeap.top(); minHeap.pop();
            BinaryTree* mergedNode = new BinaryTree('\0', node1->frequency + node2->frequency);
            mergedNode->left = node1;
            mergedNode->right = node2;
            minHeap.push(mergedNode);
        }
        return minHeap.top();
    }

    void buildTreeCodeHelper(BinaryTree* root, const string& currentBits) {
        if (!root) return;
        if (root->value != '\0') {
            huffmanCode[root->value] = currentBits;
            reverseHuffmanCode[currentBits] = root->value;
            return;
        }
        buildTreeCodeHelper(root->left, currentBits + "0");
        buildTreeCodeHelper(root->right, currentBits + "1");
    }

    void buildTreeCode() {
        BinaryTree* root = minHeap.top();
        buildTreeCodeHelper(root, "");
    }

    string buildEncodedText(const string& text) {
        string encodedText;
        for (char ch : text) {
            encodedText += huffmanCode[ch];
        }
        return encodedText;
    }

    string buildPaddedText(const string& encodedText) {
        int paddingValue = 8 - (encodedText.length() % 8);
       string paddedInfo = bitset<8>(paddingValue).to_string();
       string paddedText = paddedInfo + encodedText;
        paddedText.append(paddingValue, '0');
        return paddedText;
    }

    vector<unsigned char> buildByteArray(const string& paddedText) {
       vector<unsigned char> byteArray;
        for (size_t i = 0; i < paddedText.length(); i += 8) {
            string byteString = paddedText.substr(i, 8);
            byteArray.push_back(static_cast<unsigned char>(bitset<8>(byteString).to_ulong()));
        }
        return byteArray;
    }

    string removePadding(const string& text) {
       string paddedInfo = text.substr(0, 8);
        int extraPadding = bitset<8>(paddedInfo).to_ulong();
        return text.substr(8, text.length() - 8 - extraPadding);
    }

    string decompressText(const string& text) {
        string decodedText;
        string currentBits;
        for (char bit : text) {
            currentBits += bit;
            if (reverseHuffmanCode.find(currentBits) != reverseHuffmanCode.end()) {
                decodedText += reverseHuffmanCode[currentBits];
                currentBits = "";
            }
        }
        return decodedText;
    }

public:
    HuffmanCode(const std::string& filePath) : path(filePath) {}

    string compress() {
       ifstream file(path);
        if (!file.is_open()) {
            cerr << "Failed to open file: " << path << endl;
            return "";
        }
        string text((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        file.close();

        auto frequencyMap = frequencyFromText(text);
        buildHeap(frequencyMap);
        buildBinaryTree();
        buildTreeCode();

        string encodedText = buildEncodedText(text);
        string paddedText = buildPaddedText(encodedText);
        vector<unsigned char> byteArray = buildByteArray(paddedText);

        string outputPath = path.substr(0, path.find_last_of('.')) + ".bin";
        ofstream output(outputPath, ios::binary);
        output.write(reinterpret_cast<char*>(byteArray.data()), byteArray.size());
        output.close();

        cout << "Compressed successfully" << endl;
        return outputPath;
    }

    void decompress(const string& inputPath) {
        ifstream file(inputPath, ios::binary);
        if (!file.is_open()) {
            cerr << "Failed to open file: " << inputPath << endl;
            return;
        }

        string bitString;
        char byte;
        while (file.get(byte)) {
            bitString += bitset<8>(static_cast<unsigned char>(byte)).to_string();
        }
        file.close();

        string actualText = removePadding(bitString);
        string decompressedText = decompressText(actualText);

        string outputPath = inputPath.substr(0, inputPath.find_last_of('.')) + "_decompressed.txt";
        ofstream output(outputPath);
        output << decompressedText;
        output.close();
         cout<<"Decompressed successfully"<<std::endl;
    }
};

int main() {
    string path;
    cout << "ENTER THE PATH OF YOUR FILE....";
    cin >> path;

    HuffmanCode huffman(path);
    string outputPath = huffman.compress();
    huffman.decompress(outputPath);

    return 0;
}


