// hackross.cpp : Defines the entry point for the application.
//

#include "hackross.h"
#include <vector>

// 8bits = 1byte. Going less than 8 causes WORSE performance (probably due to
// unaligned accesses)
constexpr int BIT_VEC_SIZE = 8;

namespace {
void populate_bits(uint8_t n, bool *b) {
  for (int i = 0; i < BIT_VEC_SIZE; ++i) {
    b[i] = (n & (1 << i)) != 0;
  }
}
} // namespace

Grid::Grid(size_t size, z3::context &context)
    : _context(context), _size(size), _nodes(_context),
      _letter_values(_context) {
  for (size_t i = 0; i < size * size; ++i) {
    _nodes.push_back(
        _context.bv_const(std::to_string(i).c_str(), BIT_VEC_SIZE));
  }

  for (int i = 0; i < 27; ++i) { // 0 - 26 (0, a-z)
    bool b[BIT_VEC_SIZE];
    uint8_t n = static_cast<char>(i);
    populate_bits(n, b);
    bool const *ba = &b[0];
    _letter_values.push_back(_context.bv_val(BIT_VEC_SIZE, ba));
  }
}

z3::expr Grid::limit_node(int index, std::vector<int> letters) {
  const auto &node = _nodes[index];
  z3::expr_vector exprs(_context);

  for (auto it = letters.begin(); it != letters.end(); ++it) {
    exprs.push_back(_letter_values[*it] == node);
  }

  return z3::mk_or(exprs);
}

z3::expr Grid::constrain_indices_to_word(std::vector<int> indices,
                                         std::string word) {
  assert(indices.size() == word.size());

  z3::expr_vector exprs(_context);
  for (size_t i = 0; i < indices.size(); ++i) {
    int node_index = indices[i];
    int letter_index = char_to_letter_index(word[i]);
    exprs.push_back(_nodes[node_index] == _letter_values[letter_index]);
  }

  return z3::mk_and(exprs);
}

z3::expr Grid::constrain_seq_to_word(std::vector<int> indices,
                                     std::string word) {
  z3::expr_vector exprs(_context);
  exprs.push_back(_context.bool_val(false)); // for early exits

  if (word.size() == indices.size()) {
    // exact fit
    exprs.push_back(constrain_indices_to_word(indices, word));
  }

  if (word.size() + 1 == indices.size()) {
    // #word and word# can be supported
    exprs.push_back(constrain_indices_to_word(indices, word + "#"));
    exprs.push_back(constrain_indices_to_word(indices, "#" + word));
  }

  if (word.size() + 1 < indices.size()) {
    // enough room to move around #word#
    std::vector<int> moving_spot(word.size() + 2);

    for (size_t i = 0; (i + word.size() + 2) <= indices.size(); ++i) {
      std::copy(indices.begin() + i, indices.begin() + i + word.size() + 2,
                moving_spot.begin());
      exprs.push_back(constrain_indices_to_word(moving_spot, "#" + word + "#"));
    }
  }

  return z3::mk_or(exprs);
}

z3::expr Grid::constrain_seq_to_words(std::vector<int> indices,
                                      std::vector<std::string> words) {
  z3::expr_vector exprs(_context);
  for (const auto &word : words) {
    if (word.size() > indices.size()) {
      continue;
    }
    exprs.push_back(constrain_seq_to_word(indices, word));
  }
  return z3::mk_or(exprs);
}

z3::expr Grid::constrain_grid_to_words(std::vector<std::string> words) {
  z3::expr_vector exprs(_context);

  std::vector<int> indices(_size);

  // rows
  int size = static_cast<int>(_size);
  for (int i = 0; i < size * size; i += size) {
    std::generate(indices.begin(), indices.end(),
                  [n = i]() mutable { return n++; });
    exprs.push_back(constrain_seq_to_words(indices, words));
  }

  // columns
  for (int i = 0; i < size; ++i) {
    std::generate(indices.begin(), indices.end(), [n = i, size]() mutable {
      n += size;
      return n - size;
    });
    exprs.push_back(constrain_seq_to_words(indices, words));
  }

  return z3::mk_and(exprs);
}

int Grid::char_to_letter_index(char c) {
  if (c == '#') {
    return 0;
  } else {
    return c - 'a' + 1;
  }
}