#include <cassert>
#include <iostream>
#include <memory>
#include <queue>
#include <vector>

class Tree
{
public:
    struct Node
    {
        explicit Node(int new_value)
            : value(new_value)
        {
            std::cout << "Node(" << value << ") constructed\n";
        }

        ~Node()
        {
            std::cout << "Node(" << value << ") destroyed\n";
        }

        int value = 0;
        std::shared_ptr<Node> left;
        std::shared_ptr<Node> right;
        std::weak_ptr<Node> parent;
    };

    void traverse_v1() const
    {
        if (!root)
        {
            return;
        }

        std::queue<std::shared_ptr<Node>> nodes;
        nodes.push(root);

        while (!nodes.empty())
        {
            auto node = nodes.front();
            nodes.pop();

            std::cout << node->value << ' ';

            if (node->left)
            {
                nodes.push(node->left);
            }

            if (node->right)
            {
                nodes.push(node->right);
            }
        }

        std::cout << '\n';
    }

    void traverse_v2() const
    {
        if (!root)
        {
            return;
        }

        std::vector<std::shared_ptr<Node>> nodes = {root};

        while (!nodes.empty())
        {
            auto node = nodes.back();
            nodes.pop_back();

            std::cout << node->value << ' ';

            if (node->right)
            {
                nodes.push_back(node->right);
            }

            if (node->left)
            {
                nodes.push_back(node->left);
            }
        }

        std::cout << '\n';
    }

    std::shared_ptr<Node> root;
};

int main()
{
    Tree tree;

    tree.root = std::make_shared<Tree::Node>(1);
    tree.root->left = std::make_shared<Tree::Node>(2);
    tree.root->right = std::make_shared<Tree::Node>(3);
    tree.root->left->left = std::make_shared<Tree::Node>(4);
    tree.root->left->right = std::make_shared<Tree::Node>(5);
    tree.root->right->left = std::make_shared<Tree::Node>(6);
    tree.root->right->right = std::make_shared<Tree::Node>(7);

    tree.root->left->parent = tree.root;
    tree.root->right->parent = tree.root;
    tree.root->left->left->parent = tree.root->left;
    tree.root->left->right->parent = tree.root->left;
    tree.root->right->left->parent = tree.root->right;
    tree.root->right->right->parent = tree.root->right;

    assert(tree.root->left->parent.lock() == tree.root);
    assert(tree.root->right->right->parent.lock() == tree.root->right);

    std::cout << "breadth-first: ";
    tree.traverse_v1();

    std::cout << "depth-first: ";
    tree.traverse_v2();

    std::cout << "reset root\n";
    tree.root.reset();
}
