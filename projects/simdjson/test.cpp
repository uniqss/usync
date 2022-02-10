#include <iostream>
#include <string>
using namespace std;

#include "simdjson.h"
using namespace simdjson;
int main(void) {
    ondemand::parser parser;
    padded_string json = padded_string::load("./twitter.json");
    ondemand::document tweets = parser.iterate(json);
    std::cout << uint64_t(tweets["search_metadata"]["count"]) << " results." << std::endl;
    std::cout << tweets["search_metadata"]["max_id_str"].get_string() << std::endl;
    std::string query = tweets["search_metadata"]["query"].get_string().value().data();
    std::cout << query << std::endl;
}
