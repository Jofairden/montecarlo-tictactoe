// ttt.h

#pragma once

#include <tuple>
#include <array>
#include <vector>
#include <ctime>
#include <random>
#include <iterator>
#include <iostream>

// uint_fast8_t: give me an unsigned int of at least 8 bits. Pick a larger type if it will make my program faster, because of alignment considerations. Optimize for speed.
// another possibility would be using a tribool
enum class Player : uint_fast8_t
{
	X, 
	O, 
	None
};

using Move = int;
using State = std::array<Player, 9>;
using Scores = std::array<int, 9>;

inline Scores new_starting_scores()
{
	Scores scores;

	for (int i = 0; i < 9; ++i)
		scores[i] = 0;

	return scores;
}

// used to get a random element from a container
template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
	std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
	std::advance(start, dis(g));
	return start;
}

template<typename Iter>
Iter select_randomly(Iter start, Iter end) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	return select_randomly(start, end, gen);
}

std::ostream &operator<<(std::ostream &os, const State &state);
std::ostream &operator<<(std::ostream &os, const Player &player);

Player get_current_player(const State &state);
State do_move(const State &state, const Move &m, const Player &p);
Player get_winner(const State &state);
std::vector<Move> get_moves(const State &state);
