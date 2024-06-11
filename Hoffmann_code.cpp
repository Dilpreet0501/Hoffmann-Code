#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <string>
#include <vector>
#include <bitset>

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
    std::string path;
    std::priority_queue<BinaryTree*, std::vector<BinaryTree*>, std::greater<>> minHeap;
    std::unordered_map<char, std::string> huffmanCode;
    std::unordered_map<std::string, char> reverseHuffmanCode;

    std::unordered_map<char, int> frequencyFromText(const std::string& text) {
        std::unordered_map<char, int> frequencyMap;
        for (char ch : text) {
            frequencyMap[ch]++;
        }
        return frequencyMap;
    }

    void buildHeap(const std::unordered_map<char, int>& frequencyMap) {
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

    void buildTreeCodeHelper(BinaryTree* root, const std::string& currentBits) {
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

    std::string buildEncodedText(const std::string& text) {
        std::string encodedText;
        for (char ch : text) {
            encodedText += huffmanCode[ch];
        }
        return encodedText;
    }

    std::string buildPaddedText(const std::string& encodedText) {
        int paddingValue = 8 - (encodedText.length() % 8);
        std::string paddedInfo = std::bitset<8>(paddingValue).to_string();
        std::string paddedText = paddedInfo + encodedText;
        paddedText.append(paddingValue, '0');
        return paddedText;
    }

    std::vector<unsigned char> buildByteArray(const std::string& paddedText) {
        std::vector<unsigned char> byteArray;
        for (size_t i = 0; i < paddedText.length(); i += 8) {
            std::string byteString = paddedText.substr(i, 8);
            byteArray.push_back(static_cast<unsigned char>(std::bitset<8>(byteString).to_ulong()));
        }
        return byteArray;
    }

    std::string removePadding(const std::string& text) {
        std::string paddedInfo = text.substr(0, 8);
        int extraPadding = std::bitset<8>(paddedInfo).to_ulong();
        return text.substr(8, text.length() - 8 - extraPadding);
    }

    std::string decompressText(const std::string& text) {
        std::string decodedText;
        std::string currentBits;
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

    std::string compress() {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << path << std::endl;
            return "";
        }
        std::string text((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        auto frequencyMap = frequencyFromText(text);
        buildHeap(frequencyMap);
        buildBinaryTree();
        buildTreeCode();

        std::string encodedText = buildEncodedText(text);
        std::string paddedText = buildPaddedText(encodedText);
        std::vector<unsigned char> byteArray = buildByteArray(paddedText);

        std::string outputPath = path.substr(0, path.find_last_of('.')) + ".bin";
        std::ofstream output(outputPath, std::ios::binary);
        output.write(reinterpret_cast<char*>(byteArray.data()), byteArray.size());
        output.close();

        std::cout << "Compressed successfully" << std::endl;
        return outputPath;
    }

    void decompress(const std::string& inputPath) {
        std::ifstream file(inputPath, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << inputPath << std::endl;
            return;
        }

        std::string bitString;
        char byte;
        while (file.get(byte)) {
            bitString += std::bitset<8>(static_cast<unsigned char>(byte)).to_string();
        }
        file.close();

        std::string actualText = removePadding(bitString);
        std::string decompressedText = decompressText(actualText);

        std::string outputPath = inputPath.substr(0, inputPath.find_last_of('.')) + "_decompressed.txt";
        std::ofstream output(outputPath);
        output << decompressedText;
        output.close();
        std:: cout<<"Decompressed successfully"<<std::endl;
    }
};

int main() {
    std::string path;
    std::cout << "ENTER THE PATH OF YOUR FILE....";
    std::cin >> path;

    HuffmanCode huffman(path);
    std::string outputPath = huffman.compress();
    huffman.decompress(outputPath);

    return 0;
}


