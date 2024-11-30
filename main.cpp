#include <iostream>
#include <map>
#include <string>
#include <iomanip>
#include <cmath>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <bitset>
#include <vector>

using high_precision = boost::multiprecision::cpp_dec_float_100;

struct Interval {
    high_precision low;
    high_precision high;
};

std::map<char, high_precision> calculateProbabilities(const std::string& input) {
    std::map<char, high_precision> probabilities;
    for (char ch : input) {
        probabilities[ch]++;
    }
    for (auto& pair : probabilities) {
        pair.second /= input.size();
    }
    return probabilities;
}

std::map<char, Interval> buildIntervals(const std::map<char, high_precision>& probabilities) {
    std::map<char, Interval> intervals;
    high_precision low = 0;
    for (const auto& pair : probabilities) {
        intervals[pair.first] = {low, low + pair.second};
        low += pair.second;
    }
    return intervals;
}

std::pair<high_precision, high_precision> encode(const std::string& input, const std::map<char, Interval>& intervals) {
    high_precision low = 0;
    high_precision high = 1;
    
    std::cout << "Encoding process details:\n";
    for (char ch : input) {
        high_precision range = high - low;
        high_precision new_high = low + range * intervals.at(ch).high;
        high_precision new_low = low + range * intervals.at(ch).low;
        
        std::cout << "For '" << ch << "': [" << new_low << ", " << new_high << "]\n";
        std::cout << "Char: " << ch << "\n";
        std::cout << "Low: " << new_low << "\n";
        std::cout << "High: " << new_high << "\n";
        std::cout << "Range: " << range << "\n";
        
        high = new_high;
        low = new_low;
    }
    return {low, high};
}

const high_precision EPSILON = high_precision("1e-50");

bool isGreaterOrEqual(high_precision a, high_precision b) {
    return (a > b) || (abs(a - b) < EPSILON);
}

bool isLess(high_precision a, high_precision b) {
    return (a < b) && (abs(a - b) >= EPSILON);
}

std::string decode(high_precision encodedValue, long long messageLength, const std::map<char, Interval>& intervals) {
    std::string decodedMessage;
    high_precision value = encodedValue;
    
    std::cout << "\nStarting decode with value: " << std::fixed << std::setprecision(50) << value << "\n";
    
    for (long long i = 0; i < messageLength; ++i) {
        bool found = false;
        std::cout << "Step " << i << ", looking for value: " << value << "\n";
        
        for (const auto& pair : intervals) {
            if (isGreaterOrEqual(value, pair.second.low) && isLess(value, pair.second.high)) {
                decodedMessage += pair.first;
                value = (value - pair.second.low) / (pair.second.high - pair.second.low);
                if (isGreaterOrEqual(value, 1)) {
                    value = 1 - EPSILON;
                }
                if (value < 0) {
                    value = 0;
                }
                std::cout << "Found symbol '" << pair.first << "', new value: " << value << "\n";
                found = true;
                break;
            }
        }
        
        if (!found) {
            std::cout << "No matching interval found for value: " << value << "\n";
            break;
        }
    }
    return decodedMessage;
}

using big_int = boost::multiprecision::cpp_int;

std::string toBinary(big_int number, long long length) {
    std::string binary;
    while (number > 0) {
        binary = (number % 2 == 0 ? "0" : "1") + binary;
        number /= 2;
    }
    while (binary.length() < length) {
        binary = "0" + binary;
    }
    return binary;
}

std::string addHammingCode(const std::string& data) {
    int m = data.length();
    int r = 0;
    

    while((1 << r) < m + r + 1) {
        r++;
    }
    
    int n = m + r;
    

    std::string hammingCode(n, '0');
    

    int dataIndex = 0;
    for(int i = 1; i <= n; i++) {
        if((i & (i-1)) != 0) {  
            if(dataIndex < data.length()) {
                hammingCode[i-1] = data[dataIndex++];
            }
        }
    }
    
    // начальная строка с нулями
    std::cout << "Initial Hamming code with control bits set to 0:\n";
    std::cout << hammingCode << "\n";
    
    
    for(int i = 0; i < r; i++) {
        int pos = (1 << i);  // позиция контрольного бита 
        int parity = 0;
        std::cout << "p" << pos << " checks bits: ";
        
        
        for(int j = pos; j <= n; j++) {
            if(j & pos) {
                parity ^= (hammingCode[j-1] - '0');
                std::cout << j << "(" << hammingCode[j-1] << ") ";
            }
        }
        
        std::cout << "Resulting XOR for p" << pos << ": " << parity << "\n";
        
        
        hammingCode[pos-1] = parity + '0';
    }
    
    std::cout << "Final Hamming code with correct parity bits: " << hammingCode << "\n";
    
    return hammingCode;
}

int main() {
    std::string input = "KURBATOVMAKSIMANDREEVIC";
    
    auto probabilities = calculateProbabilities(input);
    
    std::cout << "Symbol probabilities:\n";
    for (const auto& pair : probabilities) {
        std::cout << "Symbol: " << pair.first << ", Probability: " << pair.second << "\n";
    }

   
    auto intervals = buildIntervals(probabilities);

   
    std::cout << "\nSymbol intervals:\n";
    for (const auto& pair : intervals) {
        std::cout << "Symbol: " << pair.first << ", Interval: [" << pair.second.low << ", " << pair.second.high << ")\n";
    }


    auto [low, high] = encode(input, intervals);
    high_precision range = high - low;
    double range_double = range.convert_to<double>();
    long long q = static_cast<long long>(ceil(-log2(range_double)));
    high_precision midpoint = (low + high) / 2;
    big_int p = (midpoint * pow(2, q)).convert_to<big_int>();

    std::cout << std::fixed << std::setprecision(33);
    std::cout << "\n___________________________________________\nLow: " << low << "\n";
    std::cout << "High: " << high << "\n";
    std::cout << "Range: " << range << "\n";
    std::cout << "q: " << q << "\n";
    std::cout << "p (code in demical form): " << p << "\n";
    std::cout << "Midpoint: " << midpoint << "\n";
    std::cout << "Binary form before padding: " << toBinary(p, 0) << "\n";
    double len_of_arg_code = static_cast<double>(q)/input.size();
    
    std::string binaryRepresentation = toBinary(p, q);

    
    std::cout << "Encoded binary form result: " << binaryRepresentation <<": "<< q <<" signs"  << "\n";
    std::cout << "Bits for 1 sign = " <<std::setprecision(3) << len_of_arg_code << "\n";
   
    std::string hammingCode = addHammingCode(binaryRepresentation);
    double RATE = static_cast<double>(q)/hammingCode.size();
    std::cout << "Rate: " << RATE << "\n";
    return 0;
}
