
#include"stdafx.h"
#include "Gumoku.h"
#include "ErrorCode.h"
#include"gamePiece.h"
#include"Combination.h"
#include<time.h>
#include<iostream>
#include <sstream>
#include <algorithm>
using namespace std;

Gumoku::Gumoku(){

	//we are assigning values for variables here because those variables belong to base class 
	//read from file:
	int message;
	gameName_ = "Gumoku.txt";
	message = readGame(gameName_);
	if (message == funReturn::success) {
		cout << "read saved game success!!!" << endl;
		cout << "*******************continuted************************" << endl;
		cout << endl;
		auto_play = false;
		return;
	}
	//if no data has been found use the default sets.


	broadDimension_x = 19;
	broadDimension_y = 19;
	turnx = true;
	number_of_blanks = broadDimension_x*broadDimension_y;
	player_x = "Player 1: ";
	player_o = "Player 2: ";
	pro_wino = true;
	pro_winx = true;
	auto_play = false;
	maxlength = 2;
	number_connect_ = 5;
	for (unsigned int y = 0; y < broadDimension_y; ++y){
		for (unsigned int x = 0; x < broadDimension_x; ++x){
			gamePiece game1;
			game1.x_ = x;
			game1.y_ = y;
			computerADj(game1.adj, x, y, broadDimension_x, broadDimension_y);
			gamePiece_.push_back(game1);

			int index = broadDimension_x*y + x;
			pointLeft_.insert(index);
		}
	}

	if (broadDimension_x > 10){
		maxlength = 3;
	}
	if (broadDimension_x > 100) {
		maxlength = 4;
	}
	
};

//extra part: initialized with input
Gumoku::Gumoku(int size, int number_connect) {
	broadDimension_x = size;
	broadDimension_y = size;
	turnx = true;
	number_of_blanks = broadDimension_x*broadDimension_y;
	player_x = "Player 1: ";
	player_o = "Player 2: ";
	pro_wino = true;
	pro_winx = true;
	auto_play = false;
	maxlength = 2;
	number_connect_ = number_connect;
	gameName_ = "Gumoku.txt";
	for (unsigned int y = 0; y < broadDimension_y; ++y) {
		for (unsigned int x = 0; x < broadDimension_x; ++x) {
			gamePiece game1;
			game1.x_ = x;
			game1.y_ = y;
			computerADj(game1.adj, x, y, broadDimension_x, broadDimension_y);
			gamePiece_.push_back(game1);

			int index = broadDimension_x*y + x;
			pointLeft_.insert(index);
		}
	}

	if (broadDimension_x > 10) {
		maxlength = 3;
	}
	if (broadDimension_x > 100) {
		maxlength = 4;
	}

}

// overload the default copy method
Gumoku::Gumoku(const Gumoku &game){
	//non-static membles cannot be initialized in the list
	gamePiece_ = game.gamePiece_;
	broadDimension_x = game.broadDimension_x;
	broadDimension_y = game.broadDimension_y;
	number_of_blanks = game.number_of_blanks;
	number_connect_ = game.number_connect_;

};

Gumoku::~Gumoku()
{
};

bool Gumoku::done(){

	unsigned int size = broadDimension_x*broadDimension_y;

	for (unsigned int i = 0; i < size; ++i){
		if (subdone(i)){
			return true;
		}
	}
	return false;

}

// check if any block fulfill the winning condition
bool Gumoku::subdone(unsigned int index){

	int size = broadDimension_x*broadDimension_y;
	string symbol = gamePiece_[index].piece_displaced;
	if (symbol == " "){
		return false;
	}
	
	for (int i = 0; i < 8; ++i){
		if (gamePiece_[index].adj[i] == -1){
			break;
		}

		int next = gamePiece_[index].adj[i];
		int prev = index;
		unsigned int n = 1;
		while(gamePiece_[next].piece_displaced == symbol&& n<number_connect_) {
			int *p;
			int diff = next - prev;
			p = std::find(gamePiece_[prev].adj, gamePiece_[prev].adj + 8, next);
			if (p != gamePiece_[prev].adj + 8) {

				prev = next;
				next = diff + next;
				++n;
				if (!(next >= 0 && next < size)) {
				break;
				}
			}
			else {
				break;
			}
		}
		
		if (n == number_connect_) {
			return true;
		}
	}

	return false;

}

bool Gumoku::draw(){
	
	if (done()||pro_wino||pro_winx){
		return false;
	}
	return true;
	
}


unsigned int Gumoku::suggest_for_X(){

	int n = number_of_blanks/2;
	if (n <= 0){
		pro_winx = false;
		return funReturn::can_not_win;
	}
	else if (n >= 5){
		
		Gumoku game_virtual=*this;
		auto it = pointLeft_.begin();
		while (it != pointLeft_.end()){
			game_virtual.gamePiece_[*it].piece_displaced = "B";
			++it;
		}
		if (game_virtual.done()){

			return 0;

		}
		else{
			pro_winx = false;

			return funReturn::can_not_win;
		}

	}
	vector<int> blanks;
	vector<int> comb;
	set<int>::iterator it = pointLeft_.begin();

	while (it != pointLeft_.end()){
		blanks.push_back(*it);
		++it;
	}
	vector<int>::iterator it2;

	printcomb(blanks, n, comb);

	Gumoku game_virtual=*this;

	for (it2 = comb.begin(); it2 != comb.end(); ++it2){
		
		if ((it2 - comb.begin()) % n == 0){
			
			if (game_virtual.done()){
				return 0;
			}
			game_virtual = *this;	
		}
		game_virtual.gamePiece_[*it2].piece_displaced = "B";
	}

	pro_winx = false;
	return funReturn::can_not_win;

}

unsigned int Gumoku::suggest_for_O(){
	int n = number_of_blanks / 2;
	if (n <= 0){
		pro_wino = false;
		return funReturn::can_not_win;
	}
	else if (n >= 5){

		Gumoku game_virtual=*this;
		auto it = pointLeft_.begin();
		while (it != pointLeft_.end()){
			game_virtual.gamePiece_[*it].piece_displaced = "W";
			++it;
		}
		if (game_virtual.done()){

			return 0;

		}
		else{
			pro_wino = false;

			return funReturn::can_not_win;
		}

	}


	vector<int> blanks;
	vector<int> comb;
	set<int>::iterator it = pointLeft_.begin();

	while (it != pointLeft_.end()){
		blanks.push_back(*it);
		++it;
	}

	vector<int>::iterator it2;

	printcomb(blanks, n, comb);

	Gumoku game_virtual=*this;

	for (it2 = comb.begin(); it2 != comb.end(); ++it2){

		if ((it2 - comb.begin()) % n == 0){

			if (game_virtual.done()){
				return 0;
			}
			game_virtual = *this;
		}
		game_virtual.gamePiece_[*it2].piece_displaced = "W";
	}
	pro_wino = false;
	return funReturn::can_not_win;


}


int Gumoku::turn(){
	if (turnx){

		cout << "Player 1's turn:" << endl;

		unsigned int index;
		unsigned int x;
		unsigned int y;
		while (true){
			
			if (auto_play){
				if (index == funReturn::can_not_win){
					if (pointLeft_.size() > 1){
						index =* pointLeft_.begin();
					}
					else{
						return funReturn::game_draw;
					}
				}
				x = gamePiece_[index].x_;
				y = gamePiece_[index].y_;
			}
			else{

				int message = prompt(x, y);
				if (message == funReturn::ask_to_quit){
					cout << "Would you like to save the game? (yes or no)" << endl;
					string str;
					bool save = false;
					while (true){
						while (!getline(cin, str)){
							cout << "Please input yes or no: " << endl;
							cin.clear();
						}
						if (str == "yes" || str == "Yes" || str == "YES"){
							save = true;
							break;
						}
						else if (str == "NO" || str == "no" || str == "No"){
							save = false;
							cout << "leave the game without save...." << endl;
							break;
						}
						cout << "Please input yes or no: " << endl;
					}

					message = saveGame(gameName_, save);
					if (message != funReturn::success){
						cout << "saved the game failed" << endl;
					}

					return funReturn::quit_the_game;
				}
				index = broadDimension_x*y + x;
			}
				if (gamePiece_[index].piece_displaced == " "){

					gamePiece_[index].piece_displaced = "B";
					gamePiece_[index].piece_color = piece_color::black;
					gamePiece_[index].piece_name = "black_stone";
					number_of_blanks--;
					pointLeft_.erase(index);
					cout << *this << endl;
					cout << endl;
					player_x = player_x + to_string(x+1) + ", " + to_string(y+1) + "; ";
					cout << player_x << endl;
					cout << "-------------------------" << endl;
					turnx = false;
					return funReturn::success;
				}
				else{
					cout << x << ", " << y << " has already been taken! Plese enter a valid coordinates!" << endl;
				}
			
		}

	}
	else{

		cout << "Player 2's turn:" << endl;

		unsigned int index;
		unsigned int x;
		unsigned int y;
		while (true){
			if (auto_play){
				if (index == funReturn::can_not_win){
					if (pointLeft_.size() > 1){
						index = *pointLeft_.begin();
					}
					else{
						return funReturn::game_draw;
					}
				}
				x = gamePiece_[index].x_;
				y = gamePiece_[index].y_;
			}
			else{

				int message = prompt(x, y);
				if (message == funReturn::ask_to_quit){

					cout << "Would you like to save the game? (yes or no)" << endl;
					string str;
					bool save = false;
					while (true){
						while (!getline(cin, str)){
							cout << "Please input yes or no: " << endl;
							cin.clear();
						}
						if (str == "yes" || str == "Yes" || str == "YES"){
							save = true;
							break;
						}
						else if (str == "NO" || str == "no" || str == "No"){
							save = false;
							cout << "leave the game without save...." << endl;
							break;
						}
						cout << "Please input yes or no: " << endl;
					}

					message = saveGame(gameName_, save);
					if (message != funReturn::success){
						cout << "saved the game failed" << endl;
					}

					return funReturn::quit_the_game;
				}
				index = broadDimension_x*y + x;
			}
				if (gamePiece_[index].piece_displaced == " "){

					gamePiece_[index].piece_displaced = "W";
					gamePiece_[index].piece_color = piece_color::white;
					gamePiece_[index].piece_name = "white_stone";
					number_of_blanks--;
					pointLeft_.erase(index);
					cout << *this << endl;
					cout << endl;
					player_o = player_o + to_string(x+1) + ", " + to_string(y+1) + "; ";
					cout << player_o << endl;
					cout << "-------------------------" << endl;
					turnx = true;
					return funReturn::success;
				}
				else{
					cout << x << ", " << y << " has already been taken! Plese enter a valid coordinates!" << endl;
				}
			
		}
	}
}

void Gumoku::print(){cout<<*this<<endl;}