// hackross.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <algorithm>
#include <iostream>
#include <vector>
#include <z3++.h>


class Grid {
public:
  z3::context &_context;
  size_t _size;
  z3::expr_vector _nodes;
  z3::expr_vector _letter_values;

  Grid(size_t size, z3::context &context);

  z3::expr limit_node(int index, std::vector<int> letters);

  /*
   * Applies the word constraints for each row/column
   */
  z3::expr constrain_grid_to_words(std::vector<std::string> words);

  /*
   * Constrains the sequence (row or colum) to match at least one of the words
   */
  z3::expr constrain_seq_to_words(std::vector<int> indices,
                                  std::vector<std::string> words);

  /*
   * constrain_seq_to_word will try to fit the word at all viable positions in
   * the sequence So for a two letter word and a sequence index of {0, 1, 2, 3}
   * We will have {[0,1], [1,2], [2,3]}
   */
  z3::expr constrain_seq_to_word(std::vector<int> indices, std::string word);

  /*
   * constrain_indices_to_word is much simpler than constrain_seq_to_word.
   * This function only applies the word exactly in those indices
   * so word.size() must equal indices.size()
   */
  z3::expr constrain_indices_to_word(std::vector<int> indices,
                                     std::string word);

private:
  int char_to_letter_index(char c);
};

