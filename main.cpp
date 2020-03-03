#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#define IC_ENGLISH 0.0667
#define ALPHABET_SIZE 26
const double approximate_key_length[] = {0.0660, 0.0520, 0.0473, 0.0449, 0.0435, 0.0426, 0.0419, 0.0414, 0.0410,
                                         0.0407};



void print(const double &i) {
    std::cout << i << ' ';
}

double calculate_ic(std::string text) {
    int count[26];
    memset(count, 0, sizeof(int) * ALPHABET_SIZE);


    for (int i = 0; i < text.length(); i++) {
        count[text[i] - 'a']++;
    }
    double num = 0;
    double den = 0;
    double ic;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        num += (count[i] * (count[i] - 1));
        den += count[i];
    }

    if (den == 0)
        return 0;
    else
        return num / (den * (den - 1));
}

std::string decrypt(std::string text, std::string key) {
    std::string plaintext = "";
    int j = 0;
    for (int i = 0; i < text.length(); i++) {
        int convert = (text[i] - key[j++] + ALPHABET_SIZE) % ALPHABET_SIZE;

        convert += 'a';

        plaintext.push_back(convert);
        if (j == key.length())
            j = 0;
    }

    return plaintext;
}



int find_key_len(std::string ciphertext, int key_len) {

    std::string substr = "";

    double mean_ic = 0;
    for(int k = 0; k <ciphertext.length(); k++) {
        std::string substr = "";

        for(int i = 0; i < ciphertext.length(); i++) {
            if(i % (key_len + k) == 0)
                substr += ciphertext[i];
        }

        mean_ic += calculate_ic(substr);

    }

    mean_ic /= (ciphertext.length());


    std::cout << mean_ic;

}




int find_key_len(std::string ciphertext) {
    double threshold = 0.01;
    double ic = 0;
    int key_len = 0;
    while(ic + threshold < IC_ENGLISH && key_len < ciphertext.length()) {
        ic = 0;
        key_len++;
        std::vector<std::string> n_strings(key_len);
        for (int i = 0; i < ciphertext.length(); i++) {
            n_strings[i % key_len] += ciphertext[i];
        }
        for (int i = 0; i < key_len; i++) {
            ic += calculate_ic(n_strings[i]);
        }
        ic /= key_len;

        std::cout << "Index of coincidence for key of length " << key_len << " is: " << ic << std::endl;

    }


    std::cout << "Found probable key length of " << key_len << std::endl;
    return key_len;
}

std::string find_key(std::string ciphertext, int key_len) {
    const double ic_english[] = {0.082, 0.015, 0.028, 0.042, 0.127, 0.022, 0.020, 0.061, 0.070, 0.002, 0.008, 0.040,
                                 0.024, 0.067, 0.075, 0.019, 0.001, 0.060, 0.063, 0.091, 0.028, 0.010, 0.023, 0.001,
                                 0.020, 0.001};

    double frequency[ALPHABET_SIZE];
    double dot_product[ALPHABET_SIZE];



    std::vector<double> ic_v;
    ic_v.assign(ic_english, ic_english + ALPHABET_SIZE);

    int letter_count =  ciphertext.length() / (key_len);
    std::string probable_key = "";
    double largest_val = 0;
    int largest_index = 0;
    for (int i = 0; i < key_len; i++) {
        memset(frequency, 0, sizeof(double) * ALPHABET_SIZE);
        memset(dot_product, 0, sizeof(double) * ALPHABET_SIZE);
        largest_val = 0;

        //compute frequencies of letters in positions mod key_len
        for (int j = i; j < ciphertext.length(); j += key_len) {
            ++frequency[ciphertext[j] - 'a'] /= (letter_count);
        }


        //calculate dot product of frequencies and index of coincidence for english
        for (int k = 0; k < ALPHABET_SIZE; k++) {

            for (int l = 0; l < ALPHABET_SIZE; l++) {
                dot_product[k] += (frequency[l] * ic_v[l]);

                if (dot_product[k] > largest_val) {
                    largest_val = dot_product[k];
                    largest_index = k;
                }
            }


            //shift array by one to get a sub j
            std::rotate(ic_v.begin(), ic_v.begin() + ic_v.size() - 1, ic_v.end());

        }




        probable_key.push_back((char) largest_index + 'a');

        std::cout << "\nMIC Table: { ";
        std::for_each(std::begin(dot_product), std::end(dot_product), print);
        std::cout << "}" << std::endl;

        std::cout << "Max dot product value: " << largest_val << " at index: " << largest_index << std::endl;
        std::cout << "Probable character: " << probable_key.back() << std::endl << std::endl;





    }

    std::cout << "Fully formed key: " << probable_key << std::endl;

    return probable_key;
}

int main(int argc, char *argv[]) {

    std::ifstream t("/Users/jmartz/mathematical_cryptography2020/ciphertext.txt");
    std::stringstream buffer;
    buffer << t.rdbuf();


    std::string input_str = buffer.str();

    std::cout << "Ciphertext: " << input_str << std::endl << std::endl;

    transform(input_str.begin(), input_str.end(), input_str.begin(), ::tolower);



    std::string decrypted_text = decrypt(input_str, find_key(input_str, find_key_len(input_str)));
    std::cout << "Decrypted text: " << decrypted_text << std::endl;

    double d_ic = calculate_ic(decrypted_text);
    std::cout << "The mutual index of coincidence for the decrypted text is: " << d_ic << std::endl;
    if(d_ic > 0.06)
        std::cout << "Based on the index of coincidence it is likely the correct key was found." << std::endl;
    else
        std::cout << "Based on the index of coincidence we cannot say with certainty that the correct key was found." << std::endl;



    return 0;
}

