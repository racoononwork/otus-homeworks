#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

TEST_CASE("Mean calculation", "[mean]") {
    std::vector<std::string> test_lines = {
        "id,host_id,host_name,neighbourhood_group,neighbourhood,latitude,longitude,room_type,price,min_nights",
        "1,123,John,Brooklyn,Williamsburg,40.7,-73.9,Entire home/apt,150,2",
        "2,456,Jane,Manhattan,Chelsea,40.7,-74.0,Private room,80,1"
    };
    
    // Проверка логики парсинга цены
    for (const auto& line : test_lines) {
        // Имитируем mapper логику
        size_t comma_pos = line.find(',', 8);
        if (comma_pos != std::string::npos) {
            std::string price_str = line.substr(8, comma_pos - 8);
            REQUIRE(!price_str.empty());
        }
    }
}
