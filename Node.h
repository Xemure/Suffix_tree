#pragma once
#include <map>
#include <set>
#include "Edge.h"

class Node {
public:
	int id;				// for visualization
	int pathLen = 0;
	std::map<char, Edge> children;
	bool leaf = false;
	Node* suffix_link = nullptr;
	std::set<int> idx;
	std::set<int> revIdx;

	Node(int id) : id(id) {}
};


