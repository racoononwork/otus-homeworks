#include <gtest/gtest.h>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "../include/data_structs.hpp" // your header

static std::vector<int> bytes_vec(const IP &ip) {
    auto b = ip.bytes();
    return {b[0], b[1], b[2], b[3]};
}

TEST(SplitTests, BasicSplit) {
    auto v = split(std::string("1.2.3.4"), '.');
    ASSERT_EQ(v.size(), 4u);
    EXPECT_EQ(v[0], "1");
    EXPECT_EQ(v[1], "2");
    EXPECT_EQ(v[2], "3");
    EXPECT_EQ(v[3], "4");
}

TEST(SplitTests, EmptyBetweenDelims) {
    auto v = split(std::string("a::b"), ':');
    ASSERT_EQ(v.size(), 3u);
    EXPECT_EQ(v[0], "a");
    EXPECT_EQ(v[1], "");
    EXPECT_EQ(v[2], "b");
}

TEST(SplitTests, LeadingTrailing) {
    auto v1 = split(std::string(".1.2"), '.');
    EXPECT_EQ(v1.front(), "");

    auto v2 = split(std::string("1.2."), '.');
    EXPECT_EQ(v2.back(), "");
}

TEST(ParseIntTests, ValidNumbers) {
    auto r = parse_int("0");
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(*r, 0);

    r = parse_int("123");
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(*r, 123);

    r = parse_int("-42");
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(*r, -42);
}

TEST(ParseIntTests, InvalidStrings) {
    auto r = parse_int("abc");
    EXPECT_FALSE(r.has_value());

    r = parse_int("12abc");
    EXPECT_FALSE(r.has_value());

    r = parse_int("");
    EXPECT_FALSE(r.has_value());
}

TEST(ParseOctetTests, ValidOctet) {
    auto r = parse_octet(std::string("0"));
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(*r, 0);
    r = parse_octet(std::string("255"));
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(*r, 255);
}

TEST(ParseOctetTests, InvalidOctet) {
    EXPECT_FALSE(parse_octet(std::string("")).has_value());
    EXPECT_FALSE(parse_octet(std::string("256")).has_value());
    EXPECT_FALSE(parse_octet(std::string("-1")).has_value());
    EXPECT_FALSE(parse_octet(std::string("12abc")).has_value());
}

TEST(MakeIPTests, ValidIP) {
    auto r = make_ip(split(std::string("192.168.0.1"), '.'));
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(bytes_vec(*r), std::vector<int>({192,168,0,1}));
}

TEST(MakeIPTests, WrongLength) {
    EXPECT_FALSE(make_ip(split(std::string("1.2.3"), '.')).has_value());
    EXPECT_FALSE(make_ip(split(std::string("1.2.3.4.5"), '.')).has_value());
}

TEST(MakeIPTests, NonNumericOctet) {
    EXPECT_FALSE(make_ip(split(std::string("1.2.three.4"), '.')).has_value());
}

TEST(MakeIPTests, OutOfRangeOctet) {
    EXPECT_FALSE(make_ip(split(std::string("1.2.300.4"), '.')).has_value());
}

TEST(SortTests, ReverseLexicographic) {
    std::vector<IP> v{
        IP({1,2,3,4}),
        IP({10,0,0,1}),
        IP({1,255,0,0}),
        IP({2,0,0,0})
    };

    IP::sort_reverse_lex(v);

    std::vector<std::vector<int>> _expected{
        {10,0,0,1},
        {2,0,0,0},
        {1,255,0,0},
        {1,2,3,4}
    };

    for (size_t i = 0; i < v.size(); ++i) {
        EXPECT_EQ(bytes_vec(v[i]), _expected[i]);
    }
}

TEST(ParsePipeTests, FromFileAllParsed) {
    std::ifstream ifs("data/ip.tsv");
    ASSERT_TRUE(ifs.is_open());

    auto cin_buf = std::cin.rdbuf();
    std::cin.rdbuf(ifs.rdbuf());

    auto ips = parse_ip_from_pipe();

    std::cin.rdbuf(cin_buf);
    EXPECT_FALSE(ips.empty());

    for (auto const &ip : ips) {
        auto b = ip.bytes();
        for (int oct : b) {
            EXPECT_GE(oct, 0);
            EXPECT_LE(oct, 255);
        }
    }
}

TEST(ParsePipeTests, EmptyInput) {
    std::istringstream iss("");
    auto cin_buf = std::cin.rdbuf();
    std::cin.rdbuf(iss.rdbuf());

    auto ips = parse_ip_from_pipe();

    std::cin.rdbuf(cin_buf);
    EXPECT_TRUE(ips.empty());
}

TEST(IPFormatTests, ToString) {
    IP ip({1,2,3,4});
    EXPECT_EQ(ip.to_string(), "1.2.3.4");
}

TEST(ParseOctetTests, RvalueInput) {
    std::string s = "10";
    auto r = parse_octet(std::move(s));
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(*r, 10);
}
