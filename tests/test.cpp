
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "../hackross.h"
#include <z3++.h>

using namespace z3;

std::vector<int> resolve_model(model m, Grid g) {
  std::vector<int> letter_values;
  for (int i = 0; i < static_cast<int>(m.size()); ++i) {
    auto v = m.get_const_decl(i);
    REQUIRE(v.arity() == 0);
    auto res = m.eval(g._nodes[i]);
    REQUIRE(res.is_bv());
    REQUIRE(res.is_const());
    letter_values.push_back(res.get_numeral_int());
  }

  return letter_values;
}

TEST_CASE("Setting nodes to specific values", "") {
  context c;
  Grid g(2, c);
  solver s(c);
  s.add(g.limit_node(0, std::vector<int>{3}));

  std::cout << s << "\n";
  std::cout << s.to_smt2() << "\n";
  REQUIRE(s.check() == sat);

  model m = s.get_model();
  std::cout << "model: " << m << "\n";
  REQUIRE(m.size() == 1);
  auto v = m[0];
  REQUIRE(v.arity() == 0);
  REQUIRE(m.get_const_interp(v) == 3);
}

TEST_CASE("Constraining a position to word", "simple") {
  SECTION("Should handle simple word") {
    context c;
    Grid g(2, c);
    solver s(c);
    std::vector<int> indices{0, 1, 2, 3};
    s.add(g.constrain_indices_to_word(indices, "abcd"));

    REQUIRE(s.check() == sat);
    model m = s.get_model();
    std::cout << "model: " << m << "\n";
    REQUIRE(m.size() == 4);
    REQUIRE(m.num_consts() == 4);

    auto letter_values = resolve_model(m, g);
    std::vector<int> letters{1, 2, 3, 4}; // a b c d
    REQUIRE_THAT(letter_values, Catch::Matchers::Equals(letters));
  }
}

TEST_CASE("Constraining a position to word intersecting", "intersecting") {
  context c;
  Grid g(2, c);
  solver s(c);

  std::vector<int> clue1{0, 1};
  std::vector<int> clue2{0, 2};

  s.add(g.limit_node(3, std::vector<int>{0}));
  s.add(g.constrain_indices_to_word(clue1, "ab"));
  s.add(g.constrain_indices_to_word(clue2, "ac"));

  REQUIRE(s.check() == sat);
  model m = s.get_model();
  std::cout << "model: " << m << "\n";
  REQUIRE(m.size() == 4);
  REQUIRE(m.num_consts() == 4);

  auto letter_values = resolve_model(m, g);
  std::vector<int> letters{1, 2, 3, 0}; // a b c d
  REQUIRE_THAT(letter_values, Catch::Matchers::Equals(letters));
}

TEST_CASE("Constraing a position to word inconsistent", "") {
  context c;
  Grid g(2, c);
  solver s(c);

  std::vector<int> clue1{0, 1};
  std::vector<int> clue2{0, 2};

  s.add(g.limit_node(3, std::vector<int>{0}));
  s.add(g.constrain_indices_to_word(clue1, "ab"));
  s.add(g.constrain_indices_to_word(clue2, "xy"));

  REQUIRE(s.check() == unsat);
}

TEST_CASE("Constraining a row to a word", "") {
  context c;
  Grid g(2, c);
  solver s(c);

  std::vector<int> pretend_row{0, 1, 2, 3};

  s.add(g.constrain_seq_to_word(pretend_row, "ab"));

  std::cout << s << "\n";
  std::cout << s.to_smt2() << "\n";

  REQUIRE(s.check() == sat);
  model m = s.get_model();
  std::cout << "model: " << m << "\n";
  REQUIRE(m.size() == 4);
  REQUIRE(m.num_consts() == 4);

  auto letter_values = resolve_model(m, g);
  std::vector<int> letters{0, 1, 2, 0}; // #ab#
  REQUIRE_THAT(letter_values, Catch::Matchers::Equals(letters));
}

TEST_CASE("Constraining a row to a word - exact fit", "") {
  context c;
  Grid g(2, c);
  solver s(c);

  std::vector<int> pretend_row{0, 1, 2, 3};

  s.add(g.constrain_seq_to_word(pretend_row, "abcd"));

  std::cout << s << "\n";
  std::cout << s.to_smt2() << "\n";

  REQUIRE(s.check() == sat);
  model m = s.get_model();
  std::cout << "model: " << m << "\n";
  REQUIRE(m.size() == 4);
  REQUIRE(m.num_consts() == 4);

  auto letter_values = resolve_model(m, g);
  std::vector<int> letters{1, 2, 3, 4}; // a b c d
  REQUIRE_THAT(letter_values, Catch::Matchers::Equals(letters));
}

TEST_CASE("Constraining a row to a word - off-by-one fit", "") {
  context c;
  Grid g(2, c);
  solver s(c);

  std::vector<int> pretend_row{0, 1, 2, 3};

  s.add(g.constrain_seq_to_word(pretend_row, "abc"));

  std::cout << s << "\n";
  std::cout << s.to_smt2() << "\n";

  REQUIRE(s.check() == sat);
  model m = s.get_model();
  std::cout << "model: " << m << "\n";
  REQUIRE(m.size() == 4);
  REQUIRE(m.num_consts() == 4);

  auto letter_values = resolve_model(m, g);
  std::vector<int> letters1{1, 2, 3, 0}; // a b c #
  std::vector<int> letters2{0, 1, 2, 3}; // # a b c
  REQUIRE_THAT(letter_values, Catch::Matchers::Equals(letters1) || Catch::Matchers::Equals(letters2));
}

TEST_CASE("Constraining a row to words", "") {
  context c;
  Grid g(2, c);
  solver s(c);

  std::vector<int> pretend_row{0, 1, 2, 3};

  s.add(g.constrain_seq_to_words(pretend_row, std::vector<std::string>{"abcd", "efgh"}));

  std::cout << s << "\n";
  std::cout << s.to_smt2() << "\n";

  REQUIRE(s.check() == sat);
  model m = s.get_model();
  std::cout << "model: " << m << "\n";
  REQUIRE(m.size() == 4);
  REQUIRE(m.num_consts() == 4);

  auto letter_values = resolve_model(m, g);
  std::vector<int> letters1{1, 2, 3, 4}; // a b c d
  std::vector<int> letters2{5, 6, 7, 8}; // e f g h
  REQUIRE_THAT(letter_values, Catch::Matchers::Equals(letters1) || Catch::Matchers::Equals(letters2));
}

TEST_CASE("Constrain grid to words", "") {
  context c;
  Grid g(2, c);
  solver s(c);

  s.add(g.constrain_grid_to_words(std::vector<std::string>{"at", "to", "zu", "pi", "mr", "ws"}));
  std::cout << s << "\n";
  std::cout << s.to_smt2() << "\n";

  REQUIRE(s.check() == sat);
  model m = s.get_model();
  std::cout << "model: " << m << "\n";
  REQUIRE(m.size() == 4);
  REQUIRE(m.num_consts() == 4);

  auto letter_values = resolve_model(m, g);
  std::vector<int> letters{1, 20, 20, 15}; // a t t o
  REQUIRE_THAT(letter_values, Catch::Matchers::Equals(letters));
}

TEST_CASE("Large grid test", "") {
  // 6x6 grid and 720 words of 6 chars
  context c;
  Grid g(6, c);
  solver s(c);

  std::string seed = "abcdef";
  std::vector<std::string> words;
  words.reserve(720);
  do {
    words.push_back(seed);
  } while (std::next_permutation(seed.begin(), seed.end()));

  s.add(g.constrain_grid_to_words(words));

  REQUIRE(s.check() == sat);
  model m = s.get_model();

  auto letter_values = resolve_model(m, g);
}

TEST_CASE("Realistic large scale", "") {
  // 9x9, 5k words of varying sizes
  // (not really, since words longer than 5 letters are filtered)
  // so this test is useless ish
  std::ifstream wordsfile("words_medium.txt");
  std::vector<std::string> words;
  std::string word;
  while (wordsfile) {
    wordsfile >> word;
    words.push_back(word);
  }

  CHECK(words.size() > 5000);
  context c;
  Grid g(5, c);
  solver s(c);

  s.add(g.constrain_grid_to_words(words));
  REQUIRE(s.check() == sat);
  model m = s.get_model();

  auto letter_values = resolve_model(m, g);
}