#include "StringTree.h"

StringTree::StringTree() :
	head{ nullptr }
{}

StringTree::~StringTree() {
	deleteNode(head);
}

StringTree::StringTree(const StringTree& other) :
	head{},
	size_{ other.size_ }
{
	if (other.head)
		makeTree(other.head, head, nullptr);
	else
		head = other.head;
}


StringTree::StringTree(StringTree&& other) :
	head{ other.head },
	size_{ other.size_ }
{}

StringTree::StringTree(std::initializer_list<std::string> words) :
	head{nullptr}
{
	for (auto& string : words) {
		insert(string);
	}
}

StringTree::StringTree(std::vector<std::string> words) :
	head{ nullptr }
{
	for (auto& string : words) {
		insert(string);
	}
}

StringTree& StringTree::operator=(const StringTree& other) {
	head = nullptr;
	size_ = other.size_;
	makeTree(other.head, head, nullptr);
	return *this;
}

StringTree& StringTree::operator=(std::initializer_list<std::string> words) {
	head = nullptr;
	for (auto& string : words) {
		insert(string);
	}
	return *this;
}


StringTree& StringTree::operator=(std::vector<std::string> words) {
	head = nullptr;
	for (auto& string : words) {
		insert(string);
	}
	return *this;
}

StringTree::StringTreeIter StringTree::begin() const {
	return StringTreeIter{ head };
}

StringTree::StringTreeIter StringTree::end() const {
	StringTreeIter s{};
	s.currEndNode = nullptr;
	return s;

}

StringTree StringTree::operator[](const std::string& tag) const {
	StringTreeNode* currNode = head;
	for (auto c : tag) {
		if (!currNode) {
			break;
		}
		c -= ' ';
		if (c > 0 && c <= 94) {
			currNode = currNode->children[c];
		}
	}
	return StringTree{ currNode };
}

void StringTree::insert(const std::string& word) {

	StringTreeNode* parent = nullptr;
	StringTreeNode** currNode = &head;

	char prevCharacter = '\0';
	for (auto c : word) {
		if (!(*currNode)) {
			*currNode = new StringTreeNode{};
			(*currNode)->parent = parent;
			(*currNode)->c = prevCharacter;
		}

		c -= ' ';
		if (c <= 94 && c >= 0) {
			parent = *currNode;
			currNode = &((*currNode)->children[c]);
			prevCharacter = c + ' ';
		}
	}
	//add node to the form the last letter
	if (!(*currNode)) {
		*currNode = new StringTreeNode{};
		(*currNode)->end = true;
		(*currNode)->parent = parent;
		(*currNode)->c = prevCharacter;
	}
	++size_;
}

size_t StringTree::size() {
	return size_;
}

bool StringTree::empty() const {
	return head == nullptr;
}

std::string StringTree::to_string() {
	std::string s;
	to_string(s, head);
	return s;
}

StringTree::StringTree(StringTreeNode* head_) :
	head{ nullptr }
{
	if (head_ != nullptr && !head_->end) {
		makeTree(head_, head, nullptr);
		head->c = '\0';
		size_ = countWords(head);
	}
}

void StringTree::deleteNode(StringTreeNode* node) {
	if (node) {
		for (auto& node : node->children) {
			deleteNode(node);
		}
		delete node;
	}
}

void StringTree::makeTree(StringTreeNode* baseNode, StringTreeNode*& currNode, StringTreeNode* parent) {
	if (!currNode) {
		currNode = new StringTreeNode{};
		currNode->end = baseNode->end;
		currNode->c = baseNode->c;
		currNode->parent = parent;
	}
	for (auto i = 0; i != baseNode->children.size(); ++i) {
		if (baseNode->children[i])
			makeTree(baseNode->children[i], currNode->children[i], currNode);
	}
}

void StringTree::to_string(std::string& s, StringTreeNode* currNode) {
	for (char c = 0; c != currNode->children.size(); ++c) {
		StringTreeNode* nextNode = currNode->children[c];
		if (nextNode) {
			s += (' ' + c);
			to_string(s, nextNode);
		}
	}
}


size_t StringTree::countWords(StringTreeNode* currNode) {
	size_t size = 0;
	for (auto& child : currNode->children) {
		if (child) {
			if (child->end)
				++size;
			size += countWords(child);
		}
	}
	return size;
}

StringTree::StringTreeNode* StringTree::findNextEnd(StringTreeNode* currNode) {
	if (!currNode->children.empty()) {
		for (const auto& node : currNode->children) {
			if (node != nullptr) {
				if (node->end)
					return node;
				else
					return findNextEnd(node);
			}
		}
	}
	return climUntilNext(currNode);
}

StringTree::StringTreeNode* StringTree::climUntilNext(StringTreeNode* currNode)
{
	if (currNode->parent) {
		for (auto iter = currNode->parent->children.begin() + (currNode->c - ' ') + 1; iter != currNode->parent->children.end(); ++iter) {
			if (*iter != nullptr) {
				if ((*iter)->end)
					return *iter;
				else
					return findNextEnd(*iter);
			}
		}
		return climUntilNext(currNode->parent);
	}
	else
		return nullptr;
}

StringTree::StringTreeIter::StringTreeIter() :
	currEndNode{}
{}

StringTree::StringTreeIter::StringTreeIter(const StringTreeIter& other) :
	currEndNode{ other.currEndNode }
{}

std::string StringTree::StringTreeIter::operator*() {
	std::string word = "";
	for (StringTreeNode* currNode = currEndNode; currNode != nullptr; currNode = currNode->parent) {
		if (currNode->c != '\0')
			word = currNode->c + word;
	}
	return word;
}

StringTree::StringTreeIter& StringTree::StringTreeIter::operator++() {
	currEndNode = findNextEnd(currEndNode);
	return *this;
}

bool StringTree::StringTreeIter::operator==(const StringTreeIter& other) {
	return currEndNode == other.currEndNode;
}

bool StringTree::StringTreeIter::operator!=(const StringTreeIter& other) {
	return !(*this == other);
}

StringTree::StringTreeIter::StringTreeIter(StringTreeNode* head) {
	currEndNode = findNextEnd(head);
}
