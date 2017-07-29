#include <vector>
#include <stack>
#include <iostream>
#include <iomanip>
#include <unordered_map>

using namespace std;

class TowerMatrix {
public:
	TowerMatrix(int m, int n);
	TowerMatrix(const TowerMatrix& t);
	const vector<pair<int, int>>& possible_move() const { return moves; }
	const vector<stack<int>>& get_data() const { return data; }
	vector<stack<int>>& get_dt() { return data; }
	const int get_disk_num() const { return disk_num; }
	void reset();
	void move_by_pair(const pair<int, int>&);
	void search_move();
	int compare(int a, int b);
	double calc_center();
	void set_status(const TowerMatrix&);
	bool operator==(const TowerMatrix&);
private:
	vector<stack<int>> data;
	vector<pair<int, int>> moves;
	int disk_num;
};

template <>
struct hash<TowerMatrix>
	: public unary_function<TowerMatrix, size_t>
{ // hashes a typeinfo object
	size_t operator()(TowerMatrix val) const {
		int hashcode = 0;
		vector<int> position(val.get_disk_num());
		for (int i = 0; i < val.get_data().size(); ++i) {
			while (!val.get_data()[i].empty()) {
				position[val.get_data()[i].top()-1] = i;
				val.get_dt()[i].pop();
			}
		}
		for (int i = 0; i < position.size(); ++i) {
			hashcode += position[i] * pow(val.get_data().size(), i);
		}
		return hashcode;
	}
};

int main() {
	constexpr int container_size = 1000000;

	int m, n, t;
	cout << "M=";
	cin >> m;
	cout << "N=";
	cin >> n;
	cout << "T=";
	cin >> t;

	TowerMatrix initial{ m,n };
	vector<TowerMatrix> stat_list_next = { initial };
	stat_list_next.reserve(container_size);
	//vector<double> prob_list_next = { 1 };
	unordered_map<TowerMatrix, double> prob_map_next;
	prob_map_next.reserve(container_size);
	prob_map_next[initial] = 1;

	TowerMatrix temp_tower = TowerMatrix{ m,n };
	TowerMatrix next_tower = TowerMatrix{ m,n };
	double temp_prob;
	bool add_new = true;

	vector<TowerMatrix> stat_list;
	stat_list.reserve(container_size);
	unordered_map<TowerMatrix, double> prob_map;
	prob_map.reserve(container_size);
	for (int i = 1; i <= t; ++i) {
		stat_list = stat_list_next;
		//prob_list = prob_list_next;
		prob_map = prob_map_next;
		stat_list_next.clear();
		//prob_list_next.clear();
		prob_map_next.clear();
		cout << "T = " << i << endl;
		for (int j = 0; j < stat_list.size(); ++j) {
			temp_tower.reset();
			temp_tower.set_status(stat_list[j]);
			temp_tower.search_move();
			//temp_prob = prob_list[j] / temp_tower.possible_move().size();
			temp_prob = prob_map[stat_list[j]] / temp_tower.possible_move().size();

			for (pair<int, int> move : temp_tower.possible_move()) {
				next_tower.set_status(temp_tower);
				next_tower.move_by_pair(move);

				/*
				add_new = true;
				for (int k = 0; k < stat_list_next.size(); ++k) {
					if (stat_list_next[k] == next_tower) {
						prob_list_next[k] += temp_prob;
						add_new = false;
						break;
					}
				}
				if (add_new) {
					stat_list_next.push_back(next_tower);
					prob_list_next.push_back(temp_prob);
				}
				*/
				if (prob_map_next[next_tower] == 0)
					stat_list_next.push_back(next_tower);
				prob_map_next[next_tower] += temp_prob;
			}
		}
	}

	vector<double> center_list;
	center_list.reserve(stat_list_next.size());
	for (int i = 0; i < stat_list_next.size(); ++i)
		center_list.push_back(stat_list_next[i].calc_center());

	double ave = 0; 
	double px2 = 0;
	for (int i = 0; i < center_list.size(); ++i) {
		//ave += center_list[i] * prob_list_next[i];
		ave += center_list[i] * prob_map_next[stat_list_next[i]];
		//px2 += center_list[i] * center_list[i] * prob_list_next[i];
		px2 += center_list[i] * center_list[i] * prob_map_next[stat_list_next[i]];
	}
	double std = sqrt(px2 - ave * ave);
	
	cout << fixed << setprecision(10)
		<< "M = " << m << ", N = " << n << endl
		<< "Mean = " << ave << ", Standard Deviation = " << std << endl;

	char ch;
	cout << "Enter any character to exit: ";
	cin >> ch;
	return 0;
}

TowerMatrix::TowerMatrix(int m, int n)
	:data{ vector<stack<int>>(m) }, disk_num{ n }
{
	for (int i = disk_num; i > 0; --i)
		data[0].push(i);
}

TowerMatrix::TowerMatrix(const TowerMatrix& t)
	:data{ t.get_data() }, moves{ t.possible_move() }, disk_num{ t.get_disk_num() } 
{
}

void TowerMatrix::reset()
{
	for (int i = 0; i < data.size(); ++i)
		while (!data[i].empty())
			data[i].pop();
	for (int i = disk_num; i > 0; --i)
		data[0].push(i);
}

void TowerMatrix::move_by_pair(const pair<int, int>& move)
{
	data[move.second].push(data[move.first].top());
	data[move.first].pop();
}

void TowerMatrix::search_move()
{
	moves.clear();
	for (int i = 0; i < data.size()-1;++i)
		switch (compare(i, i + 1)) {
		case 1:
			moves.push_back(pair<int, int>(i, i + 1));
			break;
		case 2:
			moves.push_back(pair<int, int>(i + 1, i));
			break;
		default:
			break;
		}
}

int TowerMatrix::compare(int a, int b)
{
	if (!data[a].empty()) {
		if (!data[b].empty()) {
			if (data[a].top() < data[b].top())
				return 1;
			else
				return 2;
		}
		else
			return 1;
	}
	else
	{
		if (!data[b].empty())
			return 2;
		else
			return 0;
	}
}

double TowerMatrix::calc_center()
{
	TowerMatrix copy{ *this };//save status
	double masspos = 0;
	int mass = 0;
	for (int i = 0; i < data.size(); ++i) 
		while (!data[i].empty()) {
			mass += data[i].top();
			masspos += data[i].top()*i;
			data[i].pop();
		}
	this->set_status(copy);//recover status
	if (mass != 0)
		return masspos / mass;
	else
		return -1;
}

void TowerMatrix::set_status(const TowerMatrix & t)
{
	data = t.get_data();
	moves = t.possible_move();
	disk_num = t.get_disk_num();
}

bool TowerMatrix::operator==(const TowerMatrix & t)
{
	return data == t.get_data()&&moves == t.possible_move()&&disk_num == t.get_disk_num();
}

bool operator==(const TowerMatrix& t1, const TowerMatrix& t2) {
	return t1.get_data() == t2.get_data() && t1.possible_move() == t2.possible_move() && t1.get_disk_num() == t2.get_disk_num();
}