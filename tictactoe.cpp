// tictactoe.cpp : Defines the entry point for the console application.
// (c) Daniël Zondervan / Tic-Tac-Toe using Monte Carlo (subject #4) // 03/08/2018

#include "stdafx.h"
#include "ttt.h"

#include <iostream>
#include <map>
#include <string>

using namespace std;

unsigned const n_trials = 1000; // the number of trials perfomed by the monte carlo algorithm
int const mc_match = 1; // score of machine
int const mc_other = 2; // score player

enum class PlayerType : uint_fast8_t
{
	Human,
	Computer
};

// take a board, make random moves (alternating between players) and return the end 'trial' board
State mc_trial(const State &board)
{
	auto trial = board; // the trial starting board that was given
	auto moves = get_moves(trial); // the positions a move could be made in

	// while there are available moves, continue making moves
	while (!moves.empty())
	{
		// now get a random move we could perform
		const auto move = select_randomly(moves.cbegin(), moves.cend());

		// now get the player that will perform this move
		auto player = get_current_player(trial);

		// now perform the move on this trial for the player
		trial = do_move(trial, *move, player);

		// get new moves
		moves = get_moves(trial);
	}

	// we are done with this trial, return it
	return trial;
}

// takes a completed trial, and counts the scores for the given result
void mc_update_scores(Scores &scores, const State &board, const Player &player)
{
	/*
	 * o	= machine	- mcmatch	(1)
	 * x	= human		+ mcother	(2)
	*/

	const auto winner = get_winner(board);

	for (int i = 0; i < 9; ++i)
	{
		const auto spot = board[i];
		if (spot == Player::None) // if spot empty, no score
			continue;

		const bool winning_spot = spot == winner;
		const bool is_machine = spot == player;
		const auto scoring = is_machine ? mc_match : mc_other;

		// add the score
		scores[i] += winning_spot ? scoring : -scoring;
	}
}

// takes a scores grid and a board, find all the empty spots and return a random empty spot with a maximum score amongst empty spots
// if no empty spots are present, an error is thrown
Move get_best_move(const Scores &scores, const State &board)
{
	// count empty spots
	vector<tuple<int, int>> empty_spots;
	for (int i = 0; i < 9; ++i)
	{
		const auto spot = board[i];
		if (spot == Player::None)
			empty_spots.push_back(make_tuple(i, scores[i]));
	}
	if (empty_spots.empty())
		throw invalid_argument("Given board had no empty spots");

	// get the maximum score
	int max_score = get<1>(empty_spots[0]);

	for (auto spot : empty_spots)
	{
		const auto score = get<1>(spot);
		if (score > max_score)
			max_score = score;
	}

	// get spots with maximum score
	vector<tuple<int, int>> max_score_spots;
	for (auto spot : empty_spots)
	{
		const auto score = get<1>(spot);
		if (score >= max_score)
			max_score_spots.push_back(spot);
	}

	// return a random spot
	return get<0>(*select_randomly(max_score_spots.cbegin(), max_score_spots.cend()));
}

Move mc_move(const State &board, const Player &player)
{
	// make a new scores grid
	Scores scores = new_starting_scores();

	// for i, run n trials
	for (int i = 0; i < n_trials; ++i)
	{
		// do the trial, then update the scores
		const auto trial = mc_trial(board);
		mc_update_scores(scores, trial, player);
	}
	const auto best_move = get_best_move(scores, board);
	return best_move;
}

/*
 * Setup the game, start with an empty board
 * Let the human start first, the computer will use the monte carlo algorithm to predict best moves
 * In theory, the computer should never lose the 3x3 tic-tac-toe, unless the human intentionally loses
 */
int main()
{
	srand(time(nullptr));

	map<Player, PlayerType> player_type;
	player_type[Player::X] = PlayerType::Human;
	player_type[Player::O] = PlayerType::Computer;

	State board =
	{
		Player::None, Player::None, Player::None,
		Player::None, Player::None, Player::None,
		Player::None, Player::None, Player::None
	};

	cout << board << endl;

	auto moves = get_moves(board);

	while (!moves.empty())
	{
		const auto p = get_current_player(board);
		if (player_type[p] == PlayerType::Human) {
			cout << "+-+-+-+" << endl <<
				"|0|1|2|" << endl <<
				"+-+-+-+" << endl <<
				"|3|4|5|" << endl <<
				"+-+-+-+" << endl <<
				"|6|7|8|" << endl <<
				"+-+-+-+" << endl << endl;
			cout << "Enter a move ( ";
			for (Move m : moves) cout << m << " ";
			cout << "): ";
			Move m;
			cin >> m;
			board = do_move(board, m, p);
		}
		else
		{
			board = do_move(board, mc_move(board, p), p);
		}

		cout << board << endl;
		moves = get_moves(board);
	}

	// We are done playing. Announce the winner (or draw)
	const auto winner = get_winner(board);
	if (winner == Player::None)
		cout << "DRAW!" << endl;
	else
		cout << "We have a winner! " << winner << endl;

	system("pause");

	return 0;
}
