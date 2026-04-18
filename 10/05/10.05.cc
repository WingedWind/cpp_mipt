#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>
#include <memory>

// =============================================================================
// 10.41 : Hash Functions
// =============================================================================

using HashFn = std::uint32_t (*)(std::string_view);

static std::uint32_t rs_hash(std::string_view text) {
  std::uint32_t a = 63689;
  std::uint32_t b = 378551;
  std::uint32_t hash = 0;
  for (unsigned char ch : text) {
    hash = hash * a + ch;
    a *= b;
  }
  return hash;
}

static std::uint32_t js_hash(std::string_view text) {
  std::uint32_t hash = 1315423911;
  for (unsigned char ch : text) hash ^= ((hash << 5) + ch + (hash >> 2));
  return hash;
}

static std::uint32_t pjw_hash(std::string_view text) {
  constexpr std::uint32_t bits = 32;
  constexpr std::uint32_t three_quarters = (bits * 3) / 4;
  constexpr std::uint32_t one_eighth = bits / 8;
  constexpr std::uint32_t high_bits = 0xFFFFFFFFu << (bits - one_eighth);
  std::uint32_t hash = 0;
  for (unsigned char ch : text) {
    hash = (hash << one_eighth) + ch;
    if (auto test = hash & high_bits; test != 0)
      hash = (hash ^ (test >> three_quarters)) & ~high_bits;
  }
  return hash;
}

static std::uint32_t elf_hash(std::string_view text) {
  std::uint32_t hash = 0;
  for (unsigned char ch : text) {
    hash = (hash << 4) + ch;
    if (auto x = hash & 0xF0000000u; x != 0) {
      hash ^= x >> 24;
      hash &= ~x;
    }
  }
  return hash;
}

static std::uint32_t bkdr_hash(std::string_view text) {
  constexpr std::uint32_t seed = 131;
  std::uint32_t hash = 0;
  for (unsigned char ch : text) hash = hash * seed + ch;
  return hash;
}

static std::uint32_t sdbm_hash(std::string_view text) {
  std::uint32_t hash = 0;
  for (unsigned char ch : text) hash = ch + (hash << 6) + (hash << 16) - hash;
  return hash;
}

static std::uint32_t djb_hash(std::string_view text) {
  std::uint32_t hash = 5381;
  for (unsigned char ch : text) hash = ((hash << 5) + hash) + ch;
  return hash;
}

static std::uint32_t dek_hash(std::string_view text) {
  std::uint32_t hash = static_cast<std::uint32_t>(text.size());
  for (unsigned char ch : text) hash = ((hash << 5) ^ (hash >> 27)) ^ ch;
  return hash;
}

static std::uint32_t ap_hash(std::string_view text) {
  std::uint32_t hash = 0xAAAAAAAAu;
  for (auto i = 0uz; i < text.size(); ++i) {
    auto ch = static_cast<unsigned char>(text[i]);
    if ((i & 1u) == 0) {
      hash ^= ((hash << 7) ^ ch * (hash >> 3));
    } else {
      hash ^= (~((hash << 11) + (ch ^ (hash >> 5))));
    }
  }
  return hash;
}

struct HashInfo {
  std::string name;
  HashFn function;
};

static std::vector<std::string> make_strings(std::size_t count) {
  auto values = std::vector<std::string>();
  values.reserve(count);

  for (auto i = 0uz; i < count; ++i) {
    values.push_back("string_" + std::to_string(i) + "_" + std::to_string(i * 2654435761u));
  }
	// You need strings like: fjis, vlpw, gjwn, pgoh, pqxx, qtsk
	// Randomly generated
	// Unique
	// Of same length 
	// You can choose length = 10 / 50 / 100 for example
  return values;
}

static std::size_t count_collisions(const std::vector<std::string>& values, HashFn hash) {
  auto hashes = std::unordered_set<std::uint32_t>();
  auto collisions = 0uz;

  for (const auto& value : values) {
    auto [_, inserted] = hashes.insert(hash(value));
    if (!inserted) ++collisions;
  }

  return collisions;
}

static void write_csv(const std::vector<HashInfo>& hashes, const std::vector<std::size_t>& sizes,
                      const std::string& path) {
  auto out = std::ofstream(path);
  out << "count";
  for (const auto& hash : hashes) out << ',' << hash.name;
  out << '\n';

  for (auto count : sizes) {
    auto values = make_strings(count);
    out << count;
    for (const auto& hash : hashes) out << ',' << count_collisions(values, hash.function);
    out << '\n';
  }
}

static void print_summary(const std::vector<HashInfo>& hashes, std::size_t count) {
  auto values = make_strings(count);
  auto ranking = std::vector<std::pair<std::size_t, std::string>>();

  for (const auto& hash : hashes)
    ranking.push_back({count_collisions(values, hash.function), hash.name});

  std::ranges::sort(ranking);

  std::cout << "\nRanking (" << count << " strings):\n";
  for (const auto& [collisions, name] : ranking)
    std::cout << "  " << name << ": " << collisions << " collisions\n";

  std::cout << "\nbest:  " << ranking.front().second << " (" << ranking.front().first
            << " collisions)\n";
  std::cout << "worst: " << ranking.back().second << " (" << ranking.back().first
            << " collisions)\n";

  // Collision curve shape justification:
  // For N strings hashed into a 2^32 space, the expected number of collisions
  // follows the birthday paradox: E[collisions] ≈ N^2 / (2 * 2^32).
  // For small N the curve grows quadratically (near zero), but as N approaches
  // sqrt(2^32) ≈ 65536 collisions become frequent and the curve steepens sharply.
  // Poor hash functions that cluster outputs into a small effective range show
  // collisions much earlier (the effective space is much smaller than 2^32).
  std::cout << "\nCurve shape: quadratic growth (birthday paradox), steepening as N -> sqrt(2^32)\n"
            << "Run 'python3 plot.py' after executing this program to view the graphs.\n";
}

int main() {
  auto hashes = std::vector<HashInfo>{
      {"RSHash", rs_hash},   {"JSHash", js_hash},     {"PJWHash", pjw_hash},
      {"ELFHash", elf_hash}, {"BKDRHash", bkdr_hash}, {"SDBMHash", sdbm_hash},
      {"DJBHash", djb_hash}, {"DEKHash", dek_hash},   {"APHash", ap_hash},
  };

  auto sizes = std::vector<std::size_t>{100, 500, 1000, 2000, 5000, 10000, 100000};

  write_csv(hashes, sizes, "hash_collisions.csv");
  print_summary(hashes, sizes.back());
}

/*
 * Score is 8/10
 * 
 * If you want you may redo and change upto 10/10
 * 
 * 1. The strings must be random not sequential, use the random std library
 * Use the teacher's template: https://github.com/i-s-m-mipt/Education/blob/master/projects/examples/source/10.42.cpp
 * 
 */
