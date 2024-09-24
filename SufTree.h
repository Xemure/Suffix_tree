#pragma once
#include <iostream>	// Нужно, т.к. есть методы визуализации
#include <map>		// для хранения ребер ведущих к дочерним узлам
#include <list>
#include "Edge.h"
#include "Node.h"
using namespace std;

/// <summary>
/// For Ukkonen algorithm
/// Represents Active Point (state machine)
/// </summary>
class ActivePoint {
public:
	Node* node;		// Текущий активный узел
	int edge = -1;	// Текущее активное ребро в активном узле
	int length;		// Длина по активному ребру
};

class SufTree {
	const char* input;				// Буфер где заполняется T
	int last_read = -1;				// Количество прочитанных байтов
	Node* lastCreatedNode = nullptr;

	int node_count = 0;					// Счетчик для задания id-узла.
	Node* root = new Node(node_count++);

	int remainder = 0;					// Количестов суффиксов еще не добавленных явно, но уже присутствующих в дереве.
	ActivePoint ap = { root, -1, 0 };	// {текущий узел, активное ребро в узле, длина по активному ребру}

	int modT = 0;
public:

	SufTree(const char* inp) :input(inp){}
	SufTree(const SufTree&) = delete;			// Запретили копирование, чтобы пользовались ссылками
	SufTree& operator=(const SufTree&) = delete;// Запретили присванивание.

	// все для деструктора
	void deleteEdge(Edge& e) {
		if (e.node)
			deleteNode(e.node);
	}

	void deleteNode(Node* n) {
		for (auto& c : n->children)
		{
			deleteEdge(c.second);
		}
		delete n;
	}
	~SufTree() {
		deleteNode(root);
	}
	// Методы построения дерева
	void extend(int i);
	void finalize();
	
	// Лучше здесь, т.к. нужен индекс последнего прочитаннного смивола. 
	int edgeLen(Edge& e) {
		return e.end >= 0 ? e.end - e.start + 1 : last_read - e.start + 1;
	}
#pragma region Прикладные методы
private:
	/// <summary>
	/// Заполняет поле sufIdx листовых ребер индексом суффикса во входном потоке
	/// </summary>
	/// <param name="n">указатель на узел</param>
	/// <param name="len">длина ребер от корня до узла</param>
	void fillLeaves(Node* n, int len)
	{
		for (auto& p : n->children)
		{
			int _len = len + edgeLen(p.second);
			if (p.second.isLeaf()) {
				p.second.sufIdx = last_read - _len + 1;
			}
			if (p.second.node)
				fillLeaves(p.second.node, _len);
		}
	}
	void fillPath(Node* n, int pLen)
	{
		n->pathLen = pLen;
		for (auto& p : n->children)
		{
			if (p.second.node)
			{
				fillPath(p.second.node, pLen + edgeLen(p.second));
			}	
		}
	}
	void fillNodes(Node* n)
	{
		for (auto& p : n->children)
		{
			if (p.second.node)
			{
				if (p.second.node->leaf)
				{
					if (n->pathLen >= 2 && p.second.sufIdx < modT)
					{
						p.second.node->idx.insert(p.second.sufIdx);
					}
					else
					{
						p.second.node->revIdx.insert(modT - (2 * modT - p.second.sufIdx - 1) - 1);
					}
				}
				fillNodes(p.second.node);
				n->idx.insert(p.second.node->idx.begin(), p.second.node->idx.end());
				n->revIdx.insert(p.second.node->revIdx.begin(), p.second.node->revIdx.end());
			}
			else
			{
				if (n->pathLen >= 2)
				{
					if (p.second.sufIdx < modT)
					{
						n->idx.insert(p.second.sufIdx);
					}
					else
					{
						n->revIdx.insert(modT - (2 * modT - p.second.sufIdx - 1) - 1);
					}
				}
			}
		}

	}
	int maxPalindromeLen(Node* n, int len)
	{	
		for (auto& p : n->children)
		{
			if (p.second.node)
			{
				int res = maxPalindromeLen(p.second.node, len);
				if (res > len)
				{
					len = res;
				}
			}
		}
		if (n->pathLen > len && n->pathLen >= 2)
		{
			for (auto& i : n->idx)
			{
				for (auto& j : n->revIdx)
				{
					if (j == modT - i - n->pathLen)
					{
						len = n->pathLen;
						goto theEnd;
					}
				}
			}
		}
		theEnd:
		return len;
	}
	void getMaxPalindromes(Node *n, list<int>& res, int len)
	{
		if (n->pathLen == len)
		{
			for (auto& i : n->idx)
			{
				for (auto& j : n->revIdx)
				{
					if (j == modT - i - n->pathLen)
					{
						res.push_back(i);
						break;
					}
				}
			}
		}
		else if (n->pathLen < len)
		{
			for (auto& p : n->children)
			{
				if (p.second.node)
				{
					getMaxPalindromes(p.second.node, res, len);
				}
			}
		}
	}
public:
	void FillLeaves()
	{
		fillLeaves(root, 0);
	}
	void FillPath()
	{
		fillPath(root, 0);
	}
	void FillNodes()
	{
		fillNodes(root);
	}
	int MaxPalindromeLen()
	{
		return maxPalindromeLen(root, 0);
	}
	void GetPalindromes(list<int>& res, int len)
	{
		getMaxPalindromes(root, res, len);
	}
#pragma endregion
private:
	// методы визуализации
	/// <summary>
	/// Виузализирует актвиную точку (часть состояния автомата)
	/// </summary>
	void showActivePoint() {
		if (ap.edge >= 0)
			cout << '(' << ap.node->id << ", " << input[ap.edge] << ", " << ap.length << ')';
		else
			cout << '(' << ap.node->id << ", " << "undef, " << ap.length << ')';
	}
	/// <summary>
	/// Визуализирует ребро
	/// </summary>
	/// <param name="e">Ссылка на ребро</param>
	/// <param name="level">Уровень ребра в дереве</param>
	void showEdge(Edge& e, int level)
	{
		for (int i = 0; i < edgeLen(e); i++)
			cout << input[e.start + i];
		if(e.isLeaf())
			cout << '|' << e.sufIdx;	// Значит ребро представлет собой и лист
		cout << endl;
	}
	/// <summary>
	/// Визуализация узла
	/// </summary>
	/// <param name="n">указатель на узел</param>
	/// <param name="level">уровень узла в дереве</param>
	void showNode(Node* n, int level) {
		string shift(level, '\t');
		cout << shift << n->id << " (pathLen:" << n->pathLen << ")";
		cout << "( ";
		for (auto& p : n->idx)
			cout << p << " ";
		cout << ") [ ";
		for (auto& p : n->revIdx)
			cout << p << " ";
		cout << "]";
		if (n->suffix_link)
			cout << " --> " << n->suffix_link->id;
		cout << endl;
		for (auto e : n->children) {
			cout <<shift << "    ";
			showEdge(e.second, level);
			if (e.second.node != nullptr)
				showNode(e.second.node, level+1);
		}
	}

public: 
	void ShowTree()
	{
		cout << "TextLen: " << modT << endl;
		showNode(root, 0);
	}

};
