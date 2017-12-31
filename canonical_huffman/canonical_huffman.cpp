#include<iostream>
#include<cmath>
#include<unordered_map>
#include<map>
#include<set>
#include<sstream>

using namespace std;

#define HEAP_CAPACITY 10000
#define BEGIN_CHAR (char) 0
#define END_CHAR (char) 127

int DICT_SIZE = (int) END_CHAR - BEGIN_CHAR + 1;

class Node
{
public:

    char symbol;
    int freq;
    Node *left, *right;
};

Node* create_node(char symbol, int freq, Node* left = nullptr, Node* right = nullptr)
{
    Node *node = new Node;
    node->symbol = symbol;
    node->freq = freq;
    node->left = left;
    node->right = right;
    return node;
}


class MinHeap
{
public:

    int heap_size;
    Node **heap_arr;

    MinHeap(map<char, int> symbol_map)
    {
        this->heap_size =  symbol_map.size();
        if(heap_size > HEAP_CAPACITY)
        {
            cout<<"Heap size can't exceed "<<HEAP_CAPACITY<<" ."<<endl;
            return;
        }

        this->heap_arr = new Node*[HEAP_CAPACITY];
        map<char, int>:: iterator itr;

        for(itr = symbol_map.begin(); itr != symbol_map.end(); ++itr)
        {
            int index = distance(symbol_map.begin(), itr);
            this->heap_arr[index] = create_node(itr->first, itr->second);
        }

        this->build_heap();
    }

    int get_left_child(int i)
    {
        return 2 * i + 1;
    }

    int get_right_child(int i)
    {
        return 2 * i + 2;
    }

    int get_parent(int i)
    {
        return ceil((float) i / 2) - 1;
    }

    int build_heap()    // Build heap in O(n) time.
    {
        int last_parent_index = this->heap_size / 2 - 1;

        for(int i = last_parent_index; i >= 0; --i)
        {
            this->min_heapify(i);
        }

        return 0;
    }

    int min_heapify(int index)    // Recursively maintain heap property in O(logn) time.
    {
        int min_index = index;
        int left_child = this->get_left_child(index);
        int right_child = this->get_right_child(index);

        if(left_child <= this->heap_size - 1 && this->heap_arr[left_child]->freq < this->heap_arr[min_index]->freq)
        {
            min_index = left_child;
        }

        if(right_child <= this->heap_size - 1 && this->heap_arr[right_child]->freq < this->heap_arr[min_index]->freq)
        {
            min_index = right_child;
        }

        if(min_index != index)
        {
            this->swap_heap_nodes(index, min_index);
            this->min_heapify(min_index);
        }

        return 0;
    }

    Node* extract_min()   // Extract root of the heap in 0(logn) time.
    {
        Node *min_node = this->heap_arr[0];
        this->swap_heap_nodes(0, this->heap_size - 1);
        this->heap_size--;
        this->min_heapify(0);

        return min_node;
    }

    int add_node(char symbol, int freq, Node* left = nullptr, Node* right = nullptr)
    {
        if(this->heap_size == HEAP_CAPACITY)
        {
            cout<<"Heap capacity limit reached. "<<endl;
            return 0;
        }

        this->heap_arr[this->heap_size++] = create_node(symbol, freq, left, right);

        for(int i = this->heap_size - 1; i > 0 && this->heap_arr[this->get_parent(i)]->freq > this->heap_arr[i]->freq;)
        {
            int j = this->get_parent(i);
            this->swap_heap_nodes(i, j);
            i = j;
        }

        return 0;
    }

    int swap_heap_nodes(int i, int j)
    {
        Node *temp = this->heap_arr[i];
        this->heap_arr[i] = this->heap_arr[j];
        this->heap_arr[j] = temp;

        return 0;
    }

    int print_heap()
    {
        for(int i = 0; i < this->heap_size; ++i)
        {
            cout<<this->heap_arr[i]->symbol<<" - "<<this->heap_arr[i]->freq<<endl;
        }

        return 0;
    }
};


MinHeap build_huffman_tree(MinHeap min_heap)
{
    while(min_heap.heap_size > 1)
    {
        Node *left_node = min_heap.extract_min();
        Node *right_node = min_heap.extract_min();
        min_heap.add_node('$', left_node->freq + right_node->freq, left_node, right_node);
    }

    return min_heap;
}

int get_huffman_code(Node* huffman_node, map<char, string>& encoded_symbols, string code = "")
{
    if(!huffman_node->left && !huffman_node->right)    // Symbols are only stored at leaf nodes.
    {
        //cout<<huffman_node->symbol<<" "<<code<<endl;
        encoded_symbols[huffman_node->symbol] = code;
        return 0;
    }

    get_huffman_code(huffman_node->left, encoded_symbols, code + "0");
    get_huffman_code(huffman_node->right, encoded_symbols, code + "1");

    return 0;
}

bool comparator(pair<char, string> left, pair<char, string> right)
{
    return left.second.size() == right.second.size() ? left.first < right.first : left.second.size() < right.second.size();

}

int get_bit_size(int n)
{
    int bits = 1;
    while(pow(2, bits) <= n)
    {
        ++bits;
    }

    return bits;
}

string get_binary_string(int n, int bit_size = -1)
{
    stringstream stream;
    string reverse_binary, binary_str;

    do    {
        stream << (char) n % 2;
        n /= 2;
    }   while(n);

    if(bit_size != -1 && stream.str().size() < bit_size)
    {
        int padding_size = bit_size - stream.str().size();
        while(padding_size--)
        {
            stream << '0';
        }
    }

    reverse_binary = stream.str();
    binary_str.assign(reverse_binary.rbegin(), reverse_binary.rend());


    return binary_str;
}

map<char, string> get_canonical_code(map<char, string> huffman_code)    // Convert Huffman Codes to Cannonical Codes.
{
    set<pair<char, string>, bool(*)(pair<char, string>, pair<char, string>)>ordered_huffman (huffman_code.begin(), huffman_code.end(), &comparator);
    int current_val, previous_bit_length;
    current_val = 0;
    previous_bit_length = (int) ordered_huffman.begin()->second.size();

    map<char, string> canonical_code;

    for(pair<char, string> current: ordered_huffman)
    {
        int shift_bits = current.second.size() - previous_bit_length;
        current_val = current_val << shift_bits;
        canonical_code[current.first] = get_binary_string(current_val, current.second.size());
        ++current_val;
        previous_bit_length = current.second.size();
    }

    return canonical_code;
}

int* get_bit_length_codes(map<char, string> canonical_code)    // Convert Canonical Code for each symbol to its respective bit length.
{
    int *bit_codes = new int[DICT_SIZE];

    for(int i = 0; i < DICT_SIZE; ++i)
    {
        bit_codes[i] = canonical_code[(char) ((int) BEGIN_CHAR + i)].size();
    }

    return bit_codes;
}

string get_symbols_for_bit_length(int *bit_codes, int bit_length)   // Get symbols for a value of the bit_length ordered increasingly.
{
    string symbols = "";

    for(int i = 0; i < DICT_SIZE; ++i)
    {
        if(bit_codes[i] == bit_length)
        {
            symbols += (char) BEGIN_CHAR + i;
        }
    }

    return symbols;
}

map<string, char> get_canonical_codebook(int *bit_codes)    // Obtain (canonical code, symbol) Codebook for decoding the text.
{
    map<string, char> codebook;
    int current_val, previous_bit_size;
    current_val = 0;
    previous_bit_size = 0;

    for(int bit_length = 1; bit_length < DICT_SIZE; ++bit_length)
    {
        string symbols = get_symbols_for_bit_length(bit_codes, bit_length);
        previous_bit_size = bit_length - 1;

        for(int i = 0; i < symbols.size(); ++i)
        {
            int shift_bits = bit_length - previous_bit_size;
            current_val = current_val << shift_bits;
            string binary_str = get_binary_string(current_val, bit_length);
            codebook[binary_str] = symbols[i];
            previous_bit_size = bit_length;
            ++current_val;
        }
    }

    return codebook;
}

string get_encoded_text(string text, map<char, string> canonical_codes)    // Replace symbols by their respective Canonical Codes.
{
    string encoded_text = "";

    for(int i = 0; i < text.size(); ++i)
    {
        encoded_text += canonical_codes[text[i]];
    }

    return encoded_text;
}

string get_decoded_text(string encoded_text, map<string, char> canonical_codes)    // Decode the text using the Canonical Codebook.
{
    int start, length;
    string decoded_text = "";
    start = 0;
    length = 1;

    while(start + length <= encoded_text.size())
    {
        string current = encoded_text.substr(start, length);
        if(canonical_codes.find(current) != canonical_codes.end())
        {
            decoded_text += canonical_codes[current];
            start += length;
            length = 1;
            continue;
        }

        ++length;
    }

    return decoded_text;
}

map<char, int> get_frequency_map(string text)    // Obtain map for each symbol and its frequency of occurrence in the text.
{
    map<char, int> frequency_map;

    for(int i = 0; i < text.size(); ++i)
    {
        if(frequency_map.find(text[i]) == frequency_map.end())
        {
            frequency_map[text[i]] = 1;
            continue;
        }

        ++frequency_map[text[i]];
    }

    return frequency_map;
}

int main()
{
    map<char, int> symbols;
    map<char, string> huffman_codes, canonical_codes;
    map<string, char> canonical_codebook;
    int *ordered_codes;
    string text, encoded_text, decoded_text;

    //text = "prefix-free codes for the win!!!";

    cout<<"Enter text to encode."<<endl;
    getline(cin, text);


    symbols = get_frequency_map(text);
    MinHeap h = MinHeap(symbols);
    //h.print_heap();

    h = build_huffman_tree(h);
    get_huffman_code(h.heap_arr[0], huffman_codes);
    canonical_codes = get_canonical_code(huffman_codes);
    ordered_codes = get_bit_length_codes(canonical_codes);
    canonical_codebook = get_canonical_codebook(ordered_codes);

    encoded_text = get_encoded_text(text, canonical_codes);
    decoded_text = get_decoded_text(encoded_text, canonical_codebook);

    map<char, string>:: iterator itr;
    map<string, char>:: iterator codebook_itr;
    map<char, int>:: iterator symbols_itr;

    cout<<endl<<"Symbol vs Frequency"<<endl;
    for(symbols_itr = symbols.begin(); symbols_itr != symbols.end(); ++symbols_itr)
    {
        cout<<symbols_itr->first<<" : "<<symbols_itr->second<<endl;
    }

    cout<<endl<<"Huffman Codes"<<endl;
    for(itr = huffman_codes.begin(); itr != huffman_codes.end(); ++itr)
    {
        cout<<itr->first<<" : "<<itr->second<<endl;
    }

    cout<<endl<<"Canonical Codes"<<endl;
    for(itr = canonical_codes.begin(); itr != canonical_codes.end(); ++itr)
    {
        cout<<itr->first<<" : "<<itr->second<<endl;
    }

    cout<<endl<<"Encoded Text : "<<encoded_text<<endl<<endl;

    cout<<"Ordered Bit Length Codes"<<endl;
    for(int i = 0; i < DICT_SIZE; ++i)
    {
        if(ordered_codes[i] != 0)
        {
            cout<<(char) (BEGIN_CHAR + i)<<" : "<<ordered_codes[i]<<endl;
        }
    }

    cout<<endl<<"Canonical Codebook"<<endl;
    for(codebook_itr = canonical_codebook.begin(); codebook_itr != canonical_codebook.end(); ++codebook_itr)
    {
        cout<<codebook_itr->first<<" : "<<codebook_itr->second<<endl;
    }

    cout<<endl<<"Decoded Text : "<<decoded_text<<endl;

    return 0;
}



