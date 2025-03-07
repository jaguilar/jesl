#include <jesl/ring.h>

#include <catch2/catch_all.hpp>
#include <vector>

TEST_CASE("TestRing") {
  jesl::Ring<int, 5> r;
  r.insert(2);
  r.insert(3);
  REQUIRE(r.size() == 2);
  REQUIRE(r.capacity() == 5);

  auto to_vec = [&] { return std::vector<int>(r.begin(), r.end()); };
  REQUIRE_THAT(to_vec(), Catch::Matchers::Equals(std::vector<int>{2, 3}));

  r.insert(5);
  r.insert(6);
  r.insert(7);
  r.insert(8);
  REQUIRE_THAT(to_vec(),
               Catch::Matchers::Equals(std::vector<int>{3, 5, 6, 7, 8}));

  REQUIRE(r.front() == 3);
  REQUIRE(r.back() == 8);
  REQUIRE(r.size() == 5);
}