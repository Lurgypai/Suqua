#pragma once
#include <initializer_list>
#include <string>
#include <array>
#include <vector>

class StringTree {
private:
	struct StringTreeNode {
		char c;
		std::array<StringTreeNode*, 94> children;
		StringTreeNode* parent;
		bool end;	//wether or not this is the end of a wrod
	};

public:

	class StringTreeIter {
		friend class StringTree;
	public:
		StringTreeIter();
		StringTreeIter(const StringTreeIter& other);
		std::string operator*();
		StringTreeIter& operator++();
		bool operator==(const StringTreeIter& other);
		bool operator!=(const StringTreeIter& other);
	private:
		StringTreeIter(StringTreeNode* head);
		StringTreeNode* currEndNode;
	};

	StringTree();
	~StringTree();
	StringTree(const StringTree& other);
	StringTree(StringTree&& other);
	StringTree(std::initializer_list<std::string> words);
	StringTree(std::vector<std::string> words);
	StringTree& operator=(const StringTree& other);
	StringTree& operator=(std::initializer_list<std::string> words);
	StringTree& operator=(std::vector<std::string> words);
	StringTreeIter begin() const;
	StringTreeIter end() const;

	StringTree operator[](const std::string& tag) const;
	void insert(const std::string& word);
	size_t size();
	bool empty() const;

	std::string to_string();
private:
	StringTree(StringTreeNode* head_);

	void deleteNode(StringTreeNode* node);
	void makeTree(StringTreeNode* baseNode, StringTreeNode*& currNode, StringTreeNode* parent);
	void to_string(std::string& s, StringTreeNode* currNode);
	static size_t countWords(StringTreeNode* currNode);
	static StringTreeNode* findNextEnd(StringTreeNode* currNode);
	static StringTreeNode* climUntilNext(StringTreeNode* currNode);

	StringTreeNode* head;
	size_t size_;
};