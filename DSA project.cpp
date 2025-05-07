#include <iostream>
#include <string>

using namespace std;

const int MAX_PRODUCTS = 100;
const int MAX_CART_SIZE = 50;
const int MAX_ORDERS = 50;
const int MAX_ORDER_ITEMS = 50;

// Product structure
struct Product {
    int id;
    string name;
    double price;
    int quantity;
};

// Order structure
struct Order {
    int orderId;
    Product items[MAX_ORDER_ITEMS];
    int itemCount;
    double totalAmount;
};

// Stack implementation
class ProductStack {
public:
    Product data[MAX_CART_SIZE];
    int top;

    ProductStack() : top(-1) {}

    void push(Product p) {
        if (top < MAX_CART_SIZE - 1) {
            data[++top] = p;
        }
    }

    void pop() {
        if (top >= 0) top--;
    }

    Product peek() {
        return data[top];
    }

    bool isEmpty() {
        return top == -1;
    }

    void clear() {
        top = -1;
    }
};

// Queue implementation
class OrderQueue {
public:
    Order data[MAX_ORDERS];
    int front, rear;

    OrderQueue() : front(0), rear(0) {}

    void enqueue(Order o) {
        if ((rear + 1) % MAX_ORDERS != front) {
            data[rear] = o;
            rear = (rear + 1) % MAX_ORDERS;
        }
    }

    Order dequeue() {
        Order o = data[front];
        front = (front + 1) % MAX_ORDERS;
        return o;
    }

    bool isEmpty() {
        return front == rear;
    }
};

// Global variables
Product inventory[MAX_PRODUCTS];
int inventoryCount = 0;
Product shoppingCart[MAX_CART_SIZE];
int cartSize = 0;
ProductStack undoStack;
OrderQueue orderQueue;
int orderCounter = 1;

// Merge sort for inventory
void merge(Product arr[], int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;

    Product L[n1], R[n2];

    for (int i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (L[i].id <= R[j].id) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
    }
    while (i < n1)
        arr[k++] = L[i++];
    while (j < n2)
        arr[k++] = R[j++];
}

void mergeSort(Product arr[], int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

// Utility: Find product index
int findProductIndex(int id) {
    for (int i = 0; i < inventoryCount; ++i) {
        if (inventory[i].id == id)
            return i;
    }
    return -1;
}

// Add product
void addProductToInventory(int id, string name, double price, int quantity) {
    int index = findProductIndex(id);
    if (index == -1 && inventoryCount < MAX_PRODUCTS) {
        inventory[inventoryCount++] = {id, name, price, quantity};
        mergeSort(inventory, 0, inventoryCount - 1);
    }
}

// Display products
void displayProducts() {
    cout << "Available Products:\n";
    for (int i = 0; i < inventoryCount; ++i) {
        cout << "ID: " << inventory[i].id << ", Name: " << inventory[i].name
             << ", Price: " << inventory[i].price << ", Quantity: " << inventory[i].quantity << endl;
    }
}

// Add to cart
void addToCart(int productId) {
    int index = findProductIndex(productId);
    if (index != -1 && inventory[index].quantity > 0 && cartSize < MAX_CART_SIZE) {
        shoppingCart[cartSize++] = inventory[index];
        undoStack.push(inventory[index]);
        cout << inventory[index].name << " added to cart.\n";
    } else {
        cout << "Product not available or cart full.\n";
    }
}

// Undo
void undoLastAction() {
    if (!undoStack.isEmpty()) {
        Product lastProduct = undoStack.peek();
        undoStack.pop();
        for (int i = cartSize - 1; i >= 0; --i) {
            if (shoppingCart[i].id == lastProduct.id) {
                for (int j = i; j < cartSize - 1; ++j) {
                    shoppingCart[j] = shoppingCart[j + 1];
                }
                cartSize--;
                cout << "Removed " << lastProduct.name << " from cart (undo).\n";
                return;
            }
        }
    } else {
        cout << "No action to undo.\n";
    }
}

// Checkout
void checkout() {
    if (cartSize == 0) {
        cout << "Shopping cart is empty.\n";
        return;
    }

    bool stockAvailable = true;
    double totalAmount = 0;

    for (int i = 0; i < cartSize; ++i) {
        int index = findProductIndex(shoppingCart[i].id);
        if (index == -1 || inventory[index].quantity <= 0) {
            cout << "Item out of stock: " << shoppingCart[i].name << endl;
            stockAvailable = false;
        }
    }
    if (!stockAvailable) {
        cout << "Checkout failed.\n";
        return;
    }

    for (int i = 0; i < cartSize; ++i) {
        int index = findProductIndex(shoppingCart[i].id);
        inventory[index].quantity--;
        totalAmount += shoppingCart[i].price;
    }

    Order newOrder;
    newOrder.orderId = orderCounter++;
    newOrder.itemCount = cartSize;
    newOrder.totalAmount = totalAmount;
    for (int i = 0; i < cartSize; ++i) {
        newOrder.items[i] = shoppingCart[i];
    }
    orderQueue.enqueue(newOrder);
    cartSize = 0;
    undoStack.clear();
    cout << "Order placed. ID: " << newOrder.orderId << endl;
}

// Process order
void processOrder() {
    if (!orderQueue.isEmpty()) {
        Order currentOrder = orderQueue.dequeue();
        cout << "\nProcessing Order ID: " << currentOrder.orderId << endl;
        for (int i = 0; i < currentOrder.itemCount; ++i) {
            cout << "- " << currentOrder.items[i].name << " : $" << currentOrder.items[i].price << endl;
        }
        cout << "Total: $" << currentOrder.totalAmount << endl;
        cout << "Order Completed!\n\n";
    } else {
        cout << "No orders to process.\n";
    }
}

// Main
int main() {
    addProductToInventory(1, "Laptop", 999.99, 5);
    addProductToInventory(2, "Phone", 499.99, 10);
    addProductToInventory(3, "Headphones", 149.99, 15);

    int choice, id;
    bool running = true;

    while (running) {
        cout << "\n=== Online Ordering System ===\n";
        cout << "1. Display Products\n";
        cout << "2. Add Product to Cart\n";
        cout << "3. Undo Last Action\n";
        cout << "4. Checkout\n";
        cout << "5. Process Next Order\n";
        cout << "6. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                displayProducts();
                break;
            case 2:
                cout << "Enter Product ID to add to cart: ";
                cin >> id;
                addToCart(id);
                break;
            case 3:
                undoLastAction();
                break;
            case 4:
                checkout();
                break;
            case 5:
                processOrder();
                break;
            case 6:
                running = false;
                break;
            default:
                cout << "Invalid choice.\n";
        }
    }

    cout << "Exiting the system. Thank you!\n";
    return 0;
}