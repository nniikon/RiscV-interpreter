static unsigned int random_seed = 12345;

unsigned int simple_rand()
{
    random_seed = random_seed * 1103515245 + 12345;
    return random_seed >> 16;
}

typedef struct Node {
    int value;
    struct Node* left;
    struct Node* right;
    int height;
} Node;

#define NULL ((void*)0)

static Node nodes[50];
static int node_index = 0;

int height(Node* node) {
    return node ? node->height : 0;
}

int max(int a, int b) {
    return a > b ? a : b;
}

Node* create_node(int value) {
    if (node_index >= 50) {
        return NULL;
    }
    
    Node* node = &nodes[node_index++];
    node->value = value;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}

Node* right_rotate(Node* y) {
    Node* x = y->left;
    Node* T2 = x->right;
    
    x->right = y;
    y->left = T2;
    
    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;
    
    return x;
}

Node* left_rotate(Node* x) {
    Node* y = x->right;
    Node* T2 = y->left;
    
    y->left = x;
    x->right = T2;
    
    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;
    
    return y;
}

int get_balance(Node* node) {
    if (!node) return 0;
    return height(node->left) - height(node->right);
}

Node* insert(Node* node, int value) {
    if (!node) {
        return create_node(value);
    }
    
    if (value < node->value) {
        node->left = insert(node->left, value);
    } else {
        node->right = insert(node->right, value);
    }
    
    node->height = max(height(node->left), height(node->right)) + 1;
    
    int balance = get_balance(node);
    
    if (balance > 1 && value < node->left->value) {
        return right_rotate(node);
    }
    
    if (balance < -1 && value > node->right->value) {
        return left_rotate(node);
    }
    
    if (balance > 1 && value > node->left->value) {
        node->left = left_rotate(node->left);
        return right_rotate(node);
    }
    
    if (balance < -1 && value < node->right->value) {
        node->right = right_rotate(node->right);
        return left_rotate(node);
    }
    
    return node;
}

void in_order_traversal(Node* node, int arr[], int* index) {
    if (!node) return;
    
    in_order_traversal(node->left, arr, index);
    arr[(*index)++] = node->value;
    in_order_traversal(node->right, arr, index);
}

void tree_sort(int arr[], int n) {
    node_index = 0;
    Node* root = NULL;
    
    for (int i = 0; i < n; i++) {
        root = insert(root, arr[i]);
    }
    
    int index = 0;
    in_order_traversal(root, arr, &index);
}

int is_sorted(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        if (arr[i] > arr[i + 1]) {
            return 0;
        }
    }
    return 1;
}

int main() {
    int n = 50;
    int arr[n];

    for (int i = 0; i < n; i++) {
        arr[i] = simple_rand() % 1000;
    }

    tree_sort(arr, n);

    return is_sorted(arr, n) ? 0 : 1;
}