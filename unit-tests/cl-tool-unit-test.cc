#include <cstdlib>
#include <iostream>
#include "cl-platform-info.hh"

using std::cout;
using std::cerr;
using std::endl;

int main()
{
    cerr << "extension match(\"The quick brown fox jumps over the lazy dog.\", \"The\"): " << has_extension("The quick brown fox jumps over the lazy dog.", "The") << endl;
    cerr << "extension match(\"The quick brown fox jumps over the lazy dog.\", \"the\"): " << has_extension("The quick brown fox jumps over the lazy dog.", "the") << endl;
    cerr << "extension match(\"The quick brown fox jumps over the lazy dog.\", \"dog\"): " << has_extension("The quick brown fox jumps over the lazy dog.", "dog") << endl;
    cerr << "extension match(\"The quick brown fox jumps over ve the lazy dog.\", \"ve\"): "  << has_extension("The quick brown fox jumps over ve the lazy dog.", "ve") << endl;
    cerr << "extension match(\"The quick brown fox jumps over the lazy dog.\", \"he\"): "  << has_extension("The quick brown fox jumps over the lazy dog.", "he") << endl;
    cerr << "extension match(\"The quick brown fox jumps over the lazy dog.\", \"cat\"): "  << has_extension("The quick brown fox jumps over the lazy dog.", "cat") << endl;
    cerr << "extension match(\"The quick brown fox jumps over the lazy dog.\", \"\"): "  << has_extension("The quick brown fox jumps over the lazy dog.", "") << endl;
    cerr << "extension match(\"ca\", \"cat\"): "  << has_extension("ca", "cat") << endl;
    cerr << "extension match(\"\", \"cat\"): "  << has_extension("", "cat") << endl;
    cerr << "extension match(\"\", \"\"): "  << has_extension("", "") << endl;
    cerr << "extension match(\" \", \"\"): "  << has_extension(" ", "") << endl;

    return EXIT_SUCCESS;
}
