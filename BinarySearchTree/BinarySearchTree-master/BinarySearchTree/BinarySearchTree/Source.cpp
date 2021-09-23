#include <iostream>
#include <queue>
#include <stack>


class BinarySearchTree {
public:
	struct TreeNode {
		int key;
		TreeNode *left, *right;
		explicit TreeNode(int node_key = 0, TreeNode *left_node_ptr = nullptr, TreeNode *right_node_ptr = nullptr);
		bool is_leaf() const;
		bool is_full_node() const;
	};
	struct KeyReps {
		int key;
		int reps;
		explicit KeyReps(int key_i = 0);
	};
	
	explicit BinarySearchTree(TreeNode *root_node = nullptr);
	BinarySearchTree(const BinarySearchTree &) = delete;
	// BinarySearchTree(BinarySearchTree &&) = delete;
	void operator=(const BinarySearchTree &) = delete;
	// void operator=(const BinarySearchTree &&) = delete;
	~BinarySearchTree();

	void deallocate_nodes();
	TreeNode *find_parent(int key) const;
	void insert(int key);
	void level_order_to_cout() const;
	bool search(int key) const;	// USED ONLY IN MAIN
	KeyReps get_max_key_reps() const;
	void delete_key(int key);

private:
	TreeNode *inorder_pred(TreeNode *start_node) const;
	TreeNode *inorder_pred_parent(TreeNode *start_node) const;
	void insert_below_parent(int key, TreeNode *parent);
	TreeNode *root_;
};

BinarySearchTree::TreeNode::TreeNode(int node_key, TreeNode* left_node_ptr, TreeNode* right_node_ptr) {
	key = node_key;
	left = left_node_ptr;
	right = right_node_ptr;
}

bool BinarySearchTree::TreeNode::is_leaf() const {
	return left == nullptr && right == nullptr;
}

bool BinarySearchTree::TreeNode::is_full_node() const {
	return left != nullptr && right != nullptr;
}

BinarySearchTree::KeyReps::KeyReps(int key_i) {
	key = key_i;
	reps = 0;
}

BinarySearchTree::BinarySearchTree(TreeNode *root_node) {
	root_ = root_node;
}

BinarySearchTree::~BinarySearchTree() {
	deallocate_nodes();
}

BinarySearchTree::TreeNode* BinarySearchTree::find_parent(int key) const {
	TreeNode *parent = nullptr, *next = root_;
	while (next != nullptr && key != next->key) {
		parent = next;
		if (key < next->key) {
			next = next->left;
		}
		else {
			next = next->right;
		}
	}
	return parent;
}

/**
 * USED ONLY IN MAIN
 */
bool BinarySearchTree::search(int key) const {
	TreeNode *current = root_;
	while (current != nullptr && key != current->key) {
		if (key < current->key) {
			current = current->left;
		}
		else {
			current = current->right;
		}
	}
	return current != nullptr;
}

BinarySearchTree::KeyReps BinarySearchTree::get_max_key_reps() const {
	KeyReps max_reps, reps;
	std::stack<TreeNode *> s;
	if (root_ == nullptr) {
		return max_reps;
	}
	reps.key = root_->key;
	TreeNode *current = root_;
	while (current != nullptr) {
		s.push(current);
		current = current->left;
	}
	while (!s.empty()) {
		current = s.top();
		s.pop();
		if (reps.key != current->key) {
			if (reps.reps > max_reps.reps) {
				max_reps = reps;
			}
			reps.key = current->key;
			reps.reps = 0;
		}
		reps.reps++;

		current = current->right;
		while (current != nullptr) {
			s.push(current);
			current = current->left;
		}
		
		if (reps.reps > max_reps.reps) {
			max_reps = reps;
		}
	}
	return max_reps;
}

void BinarySearchTree::delete_key(int key) {
	TreeNode *parent = find_parent(key);
	TreeNode *to_delete;
	/**
	 *	EDGE CASE FOR DELETING ROOT NODE
	 */
	if (parent == nullptr) {
		to_delete = root_;
		if (to_delete->is_leaf()) {
			delete to_delete;
			root_ = nullptr;
		}
		else if (!to_delete->is_full_node()) {
			root_ = to_delete->left != nullptr ? to_delete->left : to_delete->right;
			delete to_delete;
		}
		else {
			TreeNode *pred_parent = inorder_pred_parent(to_delete);
			pred_parent->right->right = to_delete->right;
			TreeNode *temp = pred_parent->right->left;
			root_ = pred_parent->right;
			root_->left = to_delete->left;
			pred_parent->right = temp;
			delete to_delete;
		}
		return;
	}

	if (parent->is_leaf()) {
		// There is no node with key
		return;
	}

	if (parent->right->key == key) {
		to_delete = parent->right;
		if (to_delete->is_leaf()) {
			parent->right = nullptr;
			delete to_delete;
		}
		else if (!to_delete->is_full_node()) {
			parent->right = to_delete->left != nullptr ? to_delete->left : to_delete->right;
			delete to_delete;
		}
		else {
			TreeNode *pred_parent = inorder_pred_parent(to_delete);
			if (pred_parent != to_delete) {
				pred_parent->right->right = to_delete->right;
				TreeNode *temp = pred_parent->right->left;
				parent->right = pred_parent->right;
				parent->right->left = to_delete->left;
				pred_parent->right = temp;
			}
			else {
				pred_parent->left->right = to_delete->right;
				TreeNode *temp = pred_parent->left->left;
				parent->right = pred_parent->left;
				parent->right->left = to_delete->left;
				pred_parent->left = temp;
			}
			delete to_delete;
		}
	}
	else {
		to_delete = parent->left;
		if (to_delete->is_leaf()) {
			parent->left = nullptr;
			delete to_delete;
		}
		else if (!to_delete->is_full_node()) {
			parent->left = to_delete->left != nullptr ? to_delete->left : to_delete->right;
			delete to_delete;
		}
		else {
			TreeNode *pred_parent = inorder_pred_parent(to_delete);
			if (pred_parent != to_delete) {
				pred_parent->right->right = to_delete->right;
				TreeNode *temp = pred_parent->right->left;
				parent->left = pred_parent->right;
				parent->left->left = to_delete->left;
				pred_parent->right = temp;
			}
			else {
				pred_parent->left->right = to_delete->right;
				TreeNode *temp = pred_parent->left->left;
				parent->left = pred_parent->left;
				parent->left->left = to_delete->left->left;
				pred_parent->left = temp;
			}
			delete to_delete;
		}
	}
}

void BinarySearchTree::deallocate_nodes() {
	std::queue<TreeNode *> q;
	q.push(root_);
	while (!q.empty()) {
		TreeNode *elem = q.front();
		q.pop();
		if (elem != nullptr) {
			q.push(elem->left);
			q.push(elem->right);
			delete elem;
		}
	}
	root_ = nullptr;
}

/**	!!!!! WARNING !!!!!
 *	ONLY FINDS INORDER PRED BELOW CURRENT NODE
 */
BinarySearchTree::TreeNode* BinarySearchTree::inorder_pred(TreeNode* start_node) const {
	TreeNode *pred = start_node;
	if (start_node->left != nullptr) {
		pred = start_node->left;
		while (pred->right != nullptr) {
			pred = pred->right;
		}
	}
	return pred;
}

/**	!!!!! WARNING !!!!!
*	ONLY FINDS INORDER PRED BELOW CURRENT NODE
*/
BinarySearchTree::TreeNode* BinarySearchTree::inorder_pred_parent(TreeNode* start_node) const {
	TreeNode *pred = start_node;
	if (start_node->left != nullptr) {
		if (start_node->left->right == nullptr) {
			return start_node;
		}
		pred = start_node->left;
		if (pred->right != nullptr) {
			while (pred->right->right != nullptr) {
				pred = pred->right;
			}
		}
	}
	return pred;
}

/**	!!!!! WARNING !!!!!
 *	DOES NOT CHECK EDGE CASE parent == nullptr
 */
void BinarySearchTree::insert_below_parent(int key, TreeNode* parent) {
	auto new_node = new TreeNode(key);
	if (key <= parent->key) {
		new_node->left = parent->left;
		parent->left = new_node;
	}
	else {
		new_node->right = parent->right;
		parent->right = new_node;
	}
}

void BinarySearchTree::insert(int key) {
	TreeNode *parent = find_parent(key);
	if (parent == nullptr) {
		if (root_ == nullptr) {
			root_ = new TreeNode(key);
		}
		else {
			parent = inorder_pred(root_);
			insert_below_parent(key, parent);
		}
		return;
	}
	if (parent->left != nullptr && parent->left->key == key) {
		parent = inorder_pred(parent->left);
	}
	else if (parent->right != nullptr && parent->right->key == key) {
		parent = inorder_pred(parent->right);
	}
	insert_below_parent(key, parent);
}

void BinarySearchTree::level_order_to_cout() const {
	std::queue<TreeNode *> q;
	q.push(root_);
	while (!q.empty()) {
		TreeNode *elem = q.front();
		q.pop();
		if (elem != nullptr) {
			std::cout << elem->key << " ";
			q.push(elem->left);
			q.push(elem->right);
		}
	}
	std::cout << std::endl;
}


int main() {
	std::cout << "Hello, World!" << std::endl;

	int menu_option;
	bool is_running = true;
	BinarySearchTree bst;
	BinarySearchTree::KeyReps reps;
	int key;

	while (is_running) {
		std::cout << "1. Add nodes to BST" << std::endl;
		std::cout << "2. Add node to BST" << std::endl;
		std::cout << "3. Search BST for key" << std::endl;
		std::cout << "4. Output BST" << std::endl;
		std::cout << "5. Delete key from BST" << std::endl;
		std::cout << "6. Delete BST" << std::endl;
		std::cout << "7. Output max number of key reps" << std::endl;
		std::cout << "0. Exit program" << std::endl;
		std::cout << "Select menu option: ";

		std::cin >> menu_option;

		switch (menu_option) {
		case 1:	// N KEYS INSERT
			int n;
			std::cout << "Input number of keys to insert: ";
			std::cin >> n;
			for (int i = 0; i < n; i++) {
				std::cout << "Input key to insert: ";
				std::cin >> key;
				bst.insert(key);
			}
			break;
		case 2:	// KEY INSERT
			std::cout << "Input key to insert: ";
			std::cin >> key;
			bst.insert(key);
			break;
		case 3:	// KEY SEARCHING
			std::cout << "Input key to search for: ";
			std::cin >> key;
			if (bst.search(key)) {
				std::cout << "Succesful search, key: " << key << std::endl;
			}
			else {
				std::cout << "Unsuccesful search :(" << std::endl;
			}
			break;
		case 4:	// OUTPUT LEVEL ORDER
			std::cout << "Level order:" << std::endl;
			bst.level_order_to_cout();
			break;
		case 5:	// KEY DELETION
			std::cout << "Input key to delete: ";
			std::cin >> key;
			bst.delete_key(key);
			break;
		case 6:	// TREE DELETION
			bst.deallocate_nodes();
			break;
		case 7:	// MAX KEY REPS
			reps = bst.get_max_key_reps();
			std::cout << "Max reps (" << reps.reps << ") by key " << reps.key << std::endl;
			break;
		case 0:	// EXIT
			bst.deallocate_nodes();
			is_running = false;
			break;
		default: 
			break;
		}
		std::cout << std::endl;
	}

	system("pause");
	return 0;
}
