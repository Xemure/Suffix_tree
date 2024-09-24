#pragma once
#include <iostream>	// �����, �.�. ���� ������ ������������
#include <map>		// ��� �������� ����� ������� � �������� �����
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
	Node* node;		// ������� �������� ����
	int edge = -1;	// ������� �������� ����� � �������� ����
	int length;		// ����� �� ��������� �����
};

class SufTree {
	const char* input;				// ����� ��� ����������� T
	int last_read = -1;				// ���������� ����������� ������
	Node* lastCreatedNode = nullptr;

	int node_count = 0;					// ������� ��� ������� id-����.
	Node* root = new Node(node_count++);

	int remainder = 0;					// ���������� ��������� ��� �� ����������� ����, �� ��� �������������� � ������.
	ActivePoint ap = { root, -1, 0 };	// {������� ����, �������� ����� � ����, ����� �� ��������� �����}

	int modT = 0;
public:

	SufTree(const char* inp) :input(inp){}
	SufTree(const SufTree&) = delete;			// ��������� �����������, ����� ������������ ��������
	SufTree& operator=(const SufTree&) = delete;// ��������� �������������.

	// ��� ��� �����������
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
	// ������ ���������� ������
	void extend(int i);
	void finalize();
	
	// ����� �����, �.�. ����� ������ ���������� ������������� �������. 
	int edgeLen(Edge& e) {
		return e.end >= 0 ? e.end - e.start + 1 : last_read - e.start + 1;
	}
#pragma region ���������� ������
private:
	/// <summary>
	/// ��������� ���� sufIdx �������� ����� �������� �������� �� ������� ������
	/// </summary>
	/// <param name="n">��������� �� ����</param>
	/// <param name="len">����� ����� �� ����� �� ����</param>
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
	// ������ ������������
	/// <summary>
	/// ������������� �������� ����� (����� ��������� ��������)
	/// </summary>
	void showActivePoint() {
		if (ap.edge >= 0)
			cout << '(' << ap.node->id << ", " << input[ap.edge] << ", " << ap.length << ')';
		else
			cout << '(' << ap.node->id << ", " << "undef, " << ap.length << ')';
	}
	/// <summary>
	/// ������������� �����
	/// </summary>
	/// <param name="e">������ �� �����</param>
	/// <param name="level">������� ����� � ������</param>
	void showEdge(Edge& e, int level)
	{
		for (int i = 0; i < edgeLen(e); i++)
			cout << input[e.start + i];
		if(e.isLeaf())
			cout << '|' << e.sufIdx;	// ������ ����� ����������� ����� � ����
		cout << endl;
	}
	/// <summary>
	/// ������������ ����
	/// </summary>
	/// <param name="n">��������� �� ����</param>
	/// <param name="level">������� ���� � ������</param>
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
