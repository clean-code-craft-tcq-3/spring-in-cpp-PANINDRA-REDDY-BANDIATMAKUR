/**
 * @file asaphus_coding_challenge.cpp
 * @version 1.1
 * @copyright Copyright (c) 2022 Asaphus Vision GmbH
 *
 * The goal is to implement the mechanics of a simple game and write test cases for them.
 * The rules of the game are:
 * - There are two types of boxes, green and blue. 
 * - Both can absorb tokens of a given weight, which they add to their own total weight. 
 * - Both are initialized with a given initial weight.
 * - After a box absorbs a token weight, it outputs a score. 
 * - Green and blue boxes calculate the score in different ways:
 * - A green box calculates the score as the square of the mean of the 3 weights that it most recently absorbed (square of mean of all absorbed weights if there are fewer than 3).
 * - A blue box calculates the score as Cantor's pairing function of the smallest and largest weight that it has absorbed so far, i.e. pairing(smallest, largest), where pairing(0, 1) = 2
 * - The game is played with two green boxes with initial weights 0.0 and 0.1, and two blue boxes with initial weights 0.2 and 0.3.
 * - There is a list of input token weights. Each gets used in one turn.
 * - There are two players, A and B. Both start with a score of 0. 
 * - The players take turns alternatingly. Player A starts.
 * - In each turn, the current player selects one of the boxes with the currently smallest weight, and lets it absorb the next input token weight. Each input weight gets only used once.
 * - The result of the absorption gets added to the current player's score.
 * - When all input token weights have been used up, the game ends, and the player with highest score wins.
 *
 * Task:
 * - Create a git repo for the implementation. Use the git repository to have a commit history.
 * - Implement all missing parts, marked with "TODO", including the test cases.
 * - Split the work in reasonable commits (not just one commit).
 * - Make sure the test cases succeed.
 * - Produce clean, readable code.
 *
 * Notes:
 * - Building and running the executable: g++ --std=c++14 asaphus_coding_challenge.cpp -o challenge && ./challenge
 * - Feel free to add a build system like CMake, meson, etc.
 * - Feel free to add more test cases, if you would like to test more.
 * - This file includes the header-only test framework Catch v2.13.9.
 * - A main function is not required, as it is provided by the test framework.
 */

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <limits>
#include <list>
#include <memory>
#include <numeric>
#include <vector>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

class Box {
 public:
  explicit Box(double initial_weight) : weight_{initial_weight}, absorbed_weights{} {}
  static std::unique_ptr<Box> makeGreenBox(double initial_weight);
  static std::unique_ptr<Box> makeBlueBox(double initial_weight);
  bool operator<(const Box& rhs) const { return weight_ < rhs.weight_; }
  
  //The following function will calculate the score corresponding to the box type
  //This function will be overriden in the derived class
  virtual double calcScore(void) const { return 0.0f; }
  void boxAbsorb(const uint32_t &weight);

 protected:
  double weight_;
  std::vector<uint32_t> absorbed_weights;
};

// Individual classes of green and blue box
class GreenBox : public Box
{
  public:
    explicit GreenBox(double initial_weight) : Box{initial_weight} {}

    double calcScore(void) const override
    {
      double mean_weight{0.0f};
      std::vector<uint32_t> recent_weights{};
      size_t abs_size {absorbed_weights.size()};

      if(abs_size < 3)
      {
        recent_weights = absorbed_weights;
      }
      else
      {
        recent_weights.push_back(absorbed_weights.at(abs_size-3));
        recent_weights.push_back(absorbed_weights.at(abs_size-2));
        recent_weights.push_back(absorbed_weights.at(abs_size-1));
      }

      for(const auto &weight : recent_weights)
      {
        mean_weight += static_cast<double>(weight);
      }
      
      mean_weight /= recent_weights.size();

      return (mean_weight * mean_weight);
    }
};

class BlueBox : public Box
{
  public:
    explicit BlueBox(double initial_weight) : Box{initial_weight} {}

    double calcScore(void) const override
    {
      uint32_t min_Element = *std::min_element(absorbed_weights.begin(), absorbed_weights.end());
      uint32_t max_Element = *std::max_element(absorbed_weights.begin(), absorbed_weights.end());

      return pairing(min_Element, max_Element);
    }
  
  private:
    double pairing(uint32_t &min, uint32_t &max) const
    {
      double sum = static_cast<double>(min + max);
      return (((sum * (sum + 1.0f)) / 2.0f) + max);
    }
};

//The following function will absorb the input weight into the box and increment its total weight
void Box::boxAbsorb(const uint32_t &weight)
{
  absorbed_weights.push_back(weight);
  weight_ += static_cast<double>(weight);
}

std::unique_ptr<Box> Box::makeGreenBox(double initial_weight)
{
  //creating an unique pointer for the green box
  std::unique_ptr<Box> green_box = std::make_unique<GreenBox>(initial_weight);

  return green_box;
}

std::unique_ptr<Box> Box::makeBlueBox(double initial_weight)
{
  //creating an unique pointer for the blue box
  std::unique_ptr<Box> blue_box = std::make_unique<BlueBox>(initial_weight);

  return blue_box;
}

//The following function will return the index of smallest of the boxes
uint16_t findSmallestBox(const std::vector<std::unique_ptr<Box> >& boxes)
{
  uint16_t small_idx {0u};

  for(size_t idx{1}; idx < boxes.size(); idx++)
  {
    if(*boxes[idx] < *boxes[small_idx])
    {
      small_idx = static_cast<uint16_t>(idx);
    } 
  }

  return small_idx;
}

class Player {
 public:
  void takeTurn(uint32_t input_weight,
                const std::vector<std::unique_ptr<Box> >& boxes) {

    uint16_t small_idx {0u};

    small_idx = findSmallestBox(boxes);

    boxes[small_idx]->boxAbsorb(input_weight);
    score_ += boxes[small_idx]->calcScore();
  }

  double getScore() const { return score_; }

 private:
  double score_{0.0};
};

std::pair<double, double> play(const std::vector<uint32_t>& input_weights) {
  std::vector<std::unique_ptr<Box> > boxes;
  boxes.emplace_back(Box::makeGreenBox(0.0));
  boxes.emplace_back(Box::makeGreenBox(0.1));
  boxes.emplace_back(Box::makeBlueBox(0.2));
  boxes.emplace_back(Box::makeBlueBox(0.3));

  //local variable
  uint16_t count {0u};

  //Creating the player A and B instances
  Player player_A;
  Player player_B;

  for(const auto &weight : input_weights)
  {

    if((count % 2u) == 0u)
    {
      player_A.takeTurn(weight, boxes);
    }
    else
    {
      player_B.takeTurn(weight, boxes);
    }

    if(count < 65535u)
    {
      count++;
    } 
  }

  std::cout << "Scores: player A " << player_A.getScore() << ", player B "
            << player_B.getScore() << std::endl;
  return std::make_pair(player_A.getScore(), player_B.getScore());
}

TEST_CASE("Final scores for first 4 Fibonacci numbers", "[fibonacci4]") {
  std::vector<uint32_t> inputs{1, 1, 2, 3};
  auto result = play(inputs);
  REQUIRE(result.first == 13.0);
  REQUIRE(result.second == 25.0);
}

TEST_CASE("Final scores for first 8 Fibonacci numbers", "[fibonacci8]") {
  std::vector<uint32_t> inputs{1, 1, 2, 3, 5, 8, 13, 21};
  auto result = play(inputs);
  REQUIRE(result.first == 155.0);
  REQUIRE(result.second == 366.25);
}

TEST_CASE("Test absorption of green box", "[green]") {
  // TODO
}

TEST_CASE("Test absorption of blue box", "[blue]") {
  // TODO
}
