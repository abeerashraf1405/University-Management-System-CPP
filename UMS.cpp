#include <iostream>
#include <string>
using namespace std;

// ==================== BASE LINKED LIST NODE ====================
template <typename T>
struct ListNode {
    T data;
    ListNode<T>* next;

    ListNode(T val) : data(val), next(nullptr) {}
};

// ==================== HASH TABLE MODULE ====================
struct User {
    string userName;
    string password;
    string role; // student | teacher | admin
    string department;
    string email;
    string lastBookedRoom;
    User* next; // for chaining

    User(string uname, string pwd, string r, string dept, string mail) : userName(uname), password(pwd), role(r), department(dept),  email(mail), lastBookedRoom(""), next(nullptr) {}
};

class HashTable {
private:
    int capacity;
    int size;
    User** table;

    int hashFunction(string key) {
        int hash = 0;
        for (char c : key) {
            hash = (hash * 31 + c) % capacity;
        }
        return hash;
    }

    void resizeTable() {
        int oldCapacity = capacity;
        capacity *= 2;
        User** newTable = new User * [capacity]();

        for (int i = 0; i < oldCapacity; i++) {
            User* entry = table[i];
            while (entry) {
                User* next = entry->next;
                int index = hashFunction(entry->userName);
                entry->next = newTable[index];
                newTable[index] = entry;
                entry = next;
            }
        }
        delete[] table;
        table = newTable;
    }

public:
    HashTable(int initialCapacity = 10) : capacity(initialCapacity), size(0) {
        table = new User * [capacity]();
    }

    ~HashTable() {
        for (int i = 0; i < capacity; i++) {
            User* entry = table[i];
            while (entry) {
                User* temp = entry;
                entry = entry->next;
                delete temp;
            }
        }
        delete[] table;
    }

    bool registerUser(string userName, string password, string role,
        string department, string email) {
        if (searchUser(userName)) {
            cout << "User already exists!" << endl;
            return false;
        }

        if ((float)size / capacity > 0.7) {
            resizeTable();
        }

        int index = hashFunction(userName);
        User* newUser = new User(userName, password, role, department, email);
        newUser->next = table[index];
        table[index] = newUser;
        size++;
        return true;
    }

    User* login(string userName, string password) {
        int index = hashFunction(userName);
        User* user = table[index];

        while (user) {
            if (user->userName == userName && user->password == password) {
                return user;
            }
            user = user->next;
        }
        return nullptr;
    }

    User* searchUser(string userName) {
        int index = hashFunction(userName);
        User* user = table[index];

        while (user) {
            if (user->userName == userName) {
                return user;
            }
            user = user->next;
        }
        return nullptr;
    }

    bool updateUser(string userName, string newPassword,
        string newDepartment, string newEmail) {
        User* user = searchUser(userName);
        if (user) {
            if (!newPassword.empty()) 
                user->password = newPassword;
            if (!newDepartment.empty()) 
                user->department = newDepartment;
            if (!newEmail.empty()) 
                user->email = newEmail;
            return true;
        }
        return false;
    }

    bool deleteUser(string userName) {
        int index = hashFunction(userName);
        User* user = table[index];
        User* prev = nullptr;

        while (user) {
            if (user->userName == userName) {
                if (prev) {
                    prev->next = user->next;
                }
                else {
                    table[index] = user->next;
                }
                delete user;
                size--;
                return true;
            }
            prev = user;
            user = user->next;
        }
        return false;
    }

    void displayAllUsers() {
        for (int i = 0; i < capacity; i++) {
            cout << "Bucket " << i << ": ";
            User* user = table[i];
            if (!user) {
                cout << "Empty" << endl;
                continue;
            }
            while (user) {
                cout << user->userName << "(" << user->role << ")";
                if (user->next) cout << " -> ";
                user = user->next;
            }
            cout << endl;
        }
    }

    void updateLastBookedRoom(string userName, string roomID) {
        User* user = searchUser(userName);
        if (user) {
            user->lastBookedRoom = roomID;
        }
    }
};

// ==================== GRAPH/CAMPUS MAP MODULE ====================
struct Edge {
    string destBuilding;
    int distance;
    string pathType;
    Edge* next;

    Edge(string dest, int dist, string type = "road") : destBuilding(dest), distance(dist), pathType(type), next(nullptr) {}
};

struct Building {
    string name;
    Edge* edges;
    Building* next;

    Building(string n) : name(n), edges(nullptr), next(nullptr) {}
};

class CampusGraph {
private:
    Building* buildings;
    int buildingCount;

    Building* findBuilding(string name) {
        Building* current = buildings;
        while (current) {
            if (current->name == name)
                return current;
            current = current->next;
        }
        return nullptr;
    }

public:
    CampusGraph() : buildings(nullptr), buildingCount(0) {}

    ~CampusGraph() {
        Building* current = buildings;
        while (current) {
            Edge* edge = current->edges;
            while (edge) {
                Edge* temp = edge;
                edge = edge->next;
                delete temp;
            }
            Building* temp = current;
            current = current->next;
            delete temp;
        }
    }

    bool addBuilding(string name) {
        if (findBuilding(name)) {
            cout << "Building already exists!" << endl;
            return false;
        }

        Building* newBuilding = new Building(name);
        newBuilding->next = buildings;
        buildings = newBuilding;
        buildingCount++;
        return true;
    }

    bool removeBuilding(string name) {
        Building* current = buildings;
        Building* prev = nullptr;

        while (current) {
            if (current->name == name) {
                // Remove building from adjacency lists
                Building* temp = buildings;
                while (temp) {
                    removePath(temp->name, name);
                    temp = temp->next;
                }

                // Remove building's edges
                Edge* edge = current->edges;
                while (edge) {
                    Edge* tempEdge = edge;
                    edge = edge->next;
                    delete tempEdge;
                }

                // Remove building node
                if (prev) {
                    prev->next = current->next;
                }
                else {
                    buildings = current->next;
                }
                delete current;
                buildingCount--;
                return true;
            }
            prev = current;
            current = current->next;
        }
        return false;
    }

    bool addPath(string src, string dest, int distance = 1) {
        Building* srcBuilding = findBuilding(src);
        Building* destBuilding = findBuilding(dest);

        if (!srcBuilding || !destBuilding) {
            cout << "One or both buildings not found!" << endl;
            return false;
        }

        // Add edge from src to dest
        Edge* newEdge1 = new Edge(dest, distance);
        newEdge1->next = srcBuilding->edges;
        srcBuilding->edges = newEdge1;

        // Add edge from dest to src (undirected)
        Edge* newEdge2 = new Edge(src, distance);
        newEdge2->next = destBuilding->edges;
        destBuilding->edges = newEdge2;

        return true;
    }

    bool removePath(string src, string dest) {
        Building* srcBuilding = findBuilding(src);
        Building* destBuilding = findBuilding(dest);

        if (!srcBuilding || !destBuilding) return false;

        // Remove edge from src to dest
        Edge* edge = srcBuilding->edges;
        Edge* prev = nullptr;
        while (edge) {
            if (edge->destBuilding == dest) {
                if (prev) {
                    prev->next = edge->next;
                }
                else {
                    srcBuilding->edges = edge->next;
                }
                delete edge;
                break;
            }
            prev = edge;
            edge = edge->next;
        }

        // Remove edge from dest to src
        edge = destBuilding->edges;
        prev = nullptr;
        while (edge) {
            if (edge->destBuilding == src) {
                if (prev) {
                    prev->next = edge->next;
                }
                else {
                    destBuilding->edges = edge->next;
                }
                delete edge;
                break;
            }
            prev = edge;
            edge = edge->next;
        }

        return true;
    }

    void BFS(string start) {
        Building* startBuilding = findBuilding(start);
        if (!startBuilding) {
            cout << "Building not found!" << endl;
            return;
        }

        // Custom queue using array
        Building* queue[100];
        bool visited[100] = { false };
        int front = 0, rear = 0;

        queue[rear++] = startBuilding;
        visited[getIndex(start)] = true;

        cout << "BFS from " << start << ": ";

        while (front < rear) {
            Building* current = queue[front++];
            cout << current->name << " ";

            Edge* edge = current->edges;
            while (edge) {
                Building* adj = findBuilding(edge->destBuilding);
                int idx = getIndex(adj->name);
                if (!visited[idx]) {
                    queue[rear++] = adj;
                    visited[idx] = true;
                }
                edge = edge->next;
            }
        }
        cout << endl;
    }

    void DFS(string start) {
        Building* startBuilding = findBuilding(start);
        if (!startBuilding) {
            cout << "Building not found!" << endl;
            return;
        }

        // Custom stack using array
        Building* stack[100];
        bool visited[100] = { false };
        int top = 0;

        stack[top++] = startBuilding;
        cout << "DFS from " << start << ": ";

        while (top > 0) {
            Building* current = stack[--top];

            if (!visited[getIndex(current->name)]) {
                cout << current->name << " ";
                visited[getIndex(current->name)] = true;
            }

            Edge* edge = current->edges;
            while (edge) {
                Building* adj = findBuilding(edge->destBuilding);
                int idx = getIndex(adj->name);
                if (!visited[idx]) {
                    stack[top++] = adj;
                }
                edge = edge->next;
            }
        }
        cout << endl;
    }

    void displayAdjacencyList() {
        Building* current = buildings;
        while (current) {
            cout << current->name << " -> ";
            Edge* edge = current->edges;
            while (edge) {
                cout << edge->destBuilding << "(" << edge->distance << ")";
                if (edge->next) cout << " -> ";
                edge = edge->next;
            }
            cout << endl;
            current = current->next;
        }
    }

private:
    int getIndex(string name) {
        Building* current = buildings;
        int index = 0;
        while (current) {
            if (current->name == name) return index;
            current = current->next;
            index++;
        }
        return -1;
    }

    Building* getBuildingByIndex(int index) {
        Building* current = buildings;
        int i = 0;
        while (current && i < index) {
            current = current->next;
            i++;
        }
        return current;
    }
};

// ==================== AVL TREE/ROOMS MODULE ====================
struct Room {
    string id;
    int floor;
    string type; // classroom | lab | office
    bool reserved;
    string reservedBy;

    Room(string i, int f, string t) : id(i), floor(f), type(t), reserved(false), reservedBy("") {}

    bool operator<(const Room& other) const { 
        return id < other.id; 
    }
    bool operator>(const Room& other) const { 
        return id > other.id; 
    }
    bool operator==(const Room& other) const { 
        return id == other.id; 
    }
};

struct AVLNode {
    Room data;
    AVLNode* left;
    AVLNode* right;
    int height;

    AVLNode(Room r) : data(r), left(nullptr), right(nullptr), height(1) {}
};

class AVLTree {
private:
    AVLNode* root;

    int height(AVLNode* node) { 
        return node ? node->height : 0; 
    }
    int max(int a, int b) { 
        return (a > b) ? a : b; 
    }

    int balanceFactor(AVLNode* node) {
        return node ? height(node->left) - height(node->right) : 0;
    }

    AVLNode* rotateRight(AVLNode* y) {
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;

        x->right = y;
        y->left = T2;

        y->height = max(height(y->left), height(y->right)) + 1;
        x->height = max(height(x->left), height(x->right)) + 1;

        return x;
    }

    AVLNode* rotateLeft(AVLNode* x) {
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;

        y->left = x;
        x->right = T2;

        x->height = max(height(x->left), height(x->right)) + 1;
        y->height = max(height(y->left), height(y->right)) + 1;

        return y;
    }

    AVLNode* insert(AVLNode* node, Room room) {
        if (!node) return new AVLNode(room);

        if (room < node->data) {
            node->left = insert(node->left, room);
        }
        else if (room > node->data) {
            node->right = insert(node->right, room);
        }
        else {
            return node; 
        }

        node->height = 1 + max(height(node->left), height(node->right));
        int balance = balanceFactor(node);

        // Left Left
        if (balance > 1 && room < node->left->data) {
            return rotateRight(node);
        }
        // Right Right
        if (balance < -1 && room > node->right->data) {
            return rotateLeft(node);
        }
        // Left Right
        if (balance > 1 && room > node->left->data) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        // Right Left
        if (balance < -1 && room < node->right->data) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    AVLNode* search(AVLNode* node, string roomID) {
        if (!node || node->data.id == roomID) 
            return node;

        if (roomID < node->data.id) {
            return search(node->left, roomID);
        }
        return search(node->right, roomID);
    }

    void inorder(AVLNode* node) {
        if (node) {
            inorder(node->left);
            cout << "Room: " << node->data.id << " (Floor: " << node->data.floor << ", Type: " << node->data.type << ", Reserved: " << (node->data.reserved ? "Yes" : "No") << ")" << endl;
            inorder(node->right);
        }
    }

    void searchByType(AVLNode* node, string type, int& count) {
        if (node) {
            if (node->data.type == type) {
                count++;
                cout << "Room: " << node->data.id << " (Floor: " << node->data.floor << ")" << endl;
            }
            searchByType(node->left, type, count);
            searchByType(node->right, type, count);
        }
    }

public:
    AVLTree() : root(nullptr) {}

    void insertRoom(Room room) {
        root = insert(root, room);
    }

    Room* searchRoom(string roomID) {
        AVLNode* node = search(root, roomID);
        return node ? &(node->data) : nullptr;
    }

    bool reserveRoom(string roomID, string userName) {
        Room* room = searchRoom(roomID);
        if (room && !room->reserved) {
            room->reserved = true;
            room->reservedBy = userName;
            return true;
        }
        return false;
    }

    bool cancelReservation(string roomID) {
        Room* room = searchRoom(roomID);
        if (room && room->reserved) {
            room->reserved = false;
            room->reservedBy = "";
            return true;
        }
        return false;
    }

    void displayInorder() {
        cout << "=== Rooms (Inorder) ===" << endl;
        inorder(root);
    }

    void displayByType(string type) {
        cout << "=== Rooms of type: " << type << " ===" << endl;
        int count = 0;
        searchByType(root, type, count);
        if (count == 0) {
            cout << "No rooms found of this type." << endl;
        }
    }

    bool isEmpty() { return root == nullptr; }
};

// ==================== QUEUE/COMPLAINTS MODULE ====================
struct Complaint {
    int id;
    string raisedBy;
    string building;
    string room;
    string description;
    string time;

    Complaint(int i, string user, string bldg, string rm, string desc, string t)
        : id(i), raisedBy(user), building(bldg), room(rm), description(desc), time(t) {
    }

    void display() {
        cout << "Complaint #" << id << endl;
        cout << "From: " << raisedBy << " | Building: " << building
            << " | Room: " << room << endl;
        cout << "Issue: " << description << endl;
        cout << "Time: " << time << endl;
        cout << "------------------------" << endl;
    }
};

struct ComplaintNode {
    Complaint data;
    ComplaintNode* next;

    ComplaintNode(Complaint c) : data(c), next(nullptr) {}
};

class ComplaintQueue {
private:
    ComplaintNode* front;
    ComplaintNode* rear;
    int nextID;
    int size;

public:
    ComplaintQueue() : front(nullptr), rear(nullptr), nextID(1), size(0) {}

    ~ComplaintQueue() {
        while (!isEmpty()) {
            dequeue();
        }
    }

    void enqueue(string user, string bldg, string room, string desc, string time) {
        Complaint newComplaint(nextID++, user, bldg, room, desc, time);
        ComplaintNode* newNode = new ComplaintNode(newComplaint);

        if (isEmpty()) {
            front = rear = newNode;
        }
        else {
            rear->next = newNode;
            rear = newNode;
        }
        size++;
    }

    Complaint dequeue() {
        if (isEmpty()) {
            throw "Queue is empty!";
        }

        ComplaintNode* temp = front;
        Complaint data = temp->data;
        front = front->next;

        if (!front) rear = nullptr;

        delete temp;
        size--;
        return data;
    }

    Complaint peek() {
        if (isEmpty()) {
            throw "Queue is empty!";
        }
        return front->data;
    }

    void displayAll() {
        if (isEmpty()) {
            cout << "No pending complaints." << endl;
            return;
        }

        cout << "=== PENDING COMPLAINTS (" << size << ") ===" << endl;
        ComplaintNode* current = front;
        while (current) {
            current->data.display();
            current = current->next;
        }
    }

    bool isEmpty() { return front == nullptr; }
    int getSize() { return size; }
};

// ==================== STACK/MESSAGING MODULE ====================
struct Message {
    string from;
    string to;
    string text;
    string time;

    Message(string f, string t, string txt, string tm) : from(f), to(t), text(txt), time(tm) {
    }

    void display() {
        cout << "[" << time << "] " << from << " -> " << to << ": " << text << endl;
    }
};

struct MessageNode {
    Message data;
    MessageNode* next;

    MessageNode(Message m) : data(m), next(nullptr) {}
};

class MessageStack {
private:
    MessageNode* top;
    int size;

public:
    MessageStack() : top(nullptr), size(0) {}

    ~MessageStack() {
        while (!isEmpty()) {
            pop();
        }
    }

    void push(Message msg) {
        MessageNode* newNode = new MessageNode(msg);
        newNode->next = top;
        top = newNode;
        size++;
    }

    Message pop() {
        if (isEmpty()) {
            throw "Stack is empty!";
        }

        MessageNode* temp = top;
        Message data = temp->data;
        top = top->next;
        delete temp;
        size--;
        return data;
    }

    Message peek() {
        if (isEmpty()) {
            throw "Stack is empty!";
        }
        return top->data;
    }

    void display() {
        if (isEmpty()) {
            cout << "No messages." << endl;
            return;
        }

        cout << "=== MESSAGES (" << size << ") ===" << endl;
        MessageNode* current = top;
        while (current) {
            current->data.display();
            current = current->next;
        }
    }

    bool isEmpty() { return top == nullptr; }
    int getSize() { return size; }
};

// ==================== MAIN SYSTEM CLASS ====================
class UniversitySystem {
private:
    HashTable users;
    CampusGraph campus;
    ComplaintQueue complaints;

    struct BuildingRooms {
        string name;
        AVLTree* rooms;
        BuildingRooms* next;

        BuildingRooms(string n) : name(n), rooms(new AVLTree()), next(nullptr) {}
        ~BuildingRooms() { 
            delete rooms; 
        }
    };

    BuildingRooms* buildingRooms;

    struct UserMessages {
        string userName;
        MessageStack* messages;
        UserMessages* next;

        UserMessages(string name) : userName(name), messages(new MessageStack()), next(nullptr) {}
        ~UserMessages() { 
            delete messages; 
        }
    };

    UserMessages* userMessages;

    string getCurrentTime() {
        static int counter = 1;
        string time = "Timestamp_" + to_string(counter);
        counter++;
        return time;
    }

    BuildingRooms* findBuildingRooms(string name) {
        BuildingRooms* current = buildingRooms;
        while (current) {
            if (current->name == name) 
                return current;
            current = current->next;
        }
        return nullptr;
    }

    UserMessages* findUserMessages(string userName) {
        UserMessages* current = userMessages;
        while (current) {
            if (current->userName == userName) 
                return current;
            current = current->next;
        }
        return nullptr;
    }

public:
    UniversitySystem() : buildingRooms(nullptr), userMessages(nullptr) {
        users.registerUser("admin", "admin123", "admin", "Administration", "admin@nu.edu.pk");
        users.registerUser("Abeer", "abeer123", "student", "Computer Science", "abeer@nu.edu.pk");
        users.registerUser("Prof. Ayesha", "pass123", "teacher", "DS", "teacher1@nu.edu.pk");

        campus.addBuilding("Main Building");
        campus.addBuilding("CS Block");
        campus.addBuilding("Library");
        campus.addBuilding("Hostel");

        campus.addPath("Main Building", "CS Block", 5);
        campus.addPath("Main Building", "Library", 3);
        campus.addPath("CS Block", "Hostel", 10);

        // Add rooms to buildings
        BuildingRooms* mb = new BuildingRooms("Main Building");
        mb->rooms->insertRoom(Room("MB101", 1, "classroom"));
        mb->rooms->insertRoom(Room("MB102", 1, "classroom"));
        mb->rooms->insertRoom(Room("MB201", 2, "lab"));
        mb->next = buildingRooms;
        buildingRooms = mb;

        BuildingRooms* sb = new BuildingRooms("CS Block");
        sb->rooms->insertRoom(Room("CS101", 1, "lab"));
        sb->rooms->insertRoom(Room("CS102", 1, "lab"));
        sb->rooms->insertRoom(Room("CS201", 2, "classroom"));
        sb->next = buildingRooms;
        buildingRooms = sb;
    }

    ~UniversitySystem() {
        // Clean up building rooms
        BuildingRooms* currentBR = buildingRooms;
        while (currentBR) {
            BuildingRooms* temp = currentBR;
            currentBR = currentBR->next;
            delete temp;
        }

        // Clean up user messages
        UserMessages* currentUM = userMessages;
        while (currentUM) {
            UserMessages* temp = currentUM;
            currentUM = currentUM->next;
            delete temp;
        }
    }

    void run() {
        User* currentUser = nullptr;
        int choice;

        do {
            cout << "\n===== UNIVERSITY MANAGEMENT SYSTEM =====" << endl;
            if (currentUser) {
                cout << "Logged in as: " << currentUser->userName
                    << " (" << currentUser->role << ")" << endl;
            }
            cout << "1. User & Authentication" << endl;
            cout << "2. Campus Map" << endl;
            cout << "3. Rooms & Scheduling" << endl;
            cout << "4. Complaints" << endl;
            cout << "5. Messaging" << endl;
            cout << "6. Exit" << endl;
            cout << "Choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
            case 1:
                userMenu(currentUser);
                break;
            case 2: 
                campusMenu(); 
                break;
            case 3:
                roomsMenu(currentUser); 
                break;
            case 4: 
                complaintsMenu(currentUser); 
                break;
            case 5: 
                messagingMenu(currentUser);
                break;
            case 6: 
                cout << "Goodbye!" << endl; 
                break;
            default: 
                cout << "Invalid choice!" << endl;
            }
        } while (choice != 6);
    }

private:
    void userMenu(User*& currentUser) {
        int choice;
        do {
            cout << "\n===== USER DIRECTORY =====" << endl;
            cout << "1. Register New User" << endl;
            cout << "2. Login" << endl;
            cout << "3. Update Profile" << endl;
            cout << "4. Delete User" << endl;
            cout << "5. Search User" << endl;
            cout << "6. Display All Users" << endl;
            cout << "7. Logout" << endl;
            cout << "8. Back to Main Menu" << endl;
            cout << "Choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
            case 1: 
                registerUser(); 
                break;
            case 2: 
                loginUser(currentUser); 
                break;
            case 3:
                updateProfile(currentUser);
                break;
            case 4: 
                deleteUser(); 
                break;
            case 5: 
                searchUser(); 
                break;
            case 6: 
                users.displayAllUsers(); 
                break;
            case 7:
                currentUser = nullptr; 
                cout << "Logged out." << endl; 
                break;
            case 8: 
                return;
            default: 
                cout << "Invalid choice!" << endl;
            }
        } while (true);
    }

    void registerUser() {
        string uname, pwd, role, dept, email;

        cout << "Username: "; getline(cin, uname);
        cout << "Password: "; getline(cin, pwd);
        cout << "Role (student/teacher/admin): "; getline(cin, role);
        cout << "Department: "; getline(cin, dept);
        cout << "Email: "; getline(cin, email);

        if (users.registerUser(uname, pwd, role, dept, email)) {
            cout << "Registration successful!" << endl;
        }
        else {
            cout << "Registration failed!" << endl;
        }
    }

    void loginUser(User*& currentUser) {
        if (currentUser) {
            cout << "Already logged in as " << currentUser->userName << endl;
            return;
        }

        string uname, pwd;
        cout << "Username: "; getline(cin, uname);
        cout << "Password: "; getline(cin, pwd);

        currentUser = users.login(uname, pwd);
        if (currentUser) {
            cout << "Login successful! Welcome " << currentUser->userName << endl;
        }
        else {
            cout << "Invalid credentials!" << endl;
        }
    }

    void updateProfile(User* currentUser) {
        if (!currentUser) {
            cout << "Please login first!" << endl;
            return;
        }

        string pwd, dept, email;
        cout << "Leave blank to keep current value" << endl;
        cout << "New Password: "; getline(cin, pwd);
        cout << "New Department: "; getline(cin, dept);
        cout << "New Email: "; getline(cin, email);

        if (users.updateUser(currentUser->userName, pwd, dept, email)) {
            cout << "Profile updated successfully!" << endl;
        }
        else {
            cout << "Update failed!" << endl;
        }
    }

    void deleteUser() {
        string uname;
        cout << "Username to delete: "; getline(cin, uname);

        if (users.deleteUser(uname)) {
            cout << "User deleted successfully!" << endl;
        }
        else {
            cout << "User not found!" << endl;
        }
    }

    void searchUser() {
        string uname;
        cout << "Username to search: "; getline(cin, uname);

        User* user = users.searchUser(uname);
        if (user) {
            cout << "User found:" << endl;
            cout << "Username: " << user->userName << endl;
            cout << "Role: " << user->role << endl;
            cout << "Department: " << user->department << endl;
            cout << "Email: " << user->email << endl;
            if (!user->lastBookedRoom.empty()) {
                cout << "Last booked room: " << user->lastBookedRoom << endl;
            }
        }
        else {
            cout << "User not found!" << endl;
        }
    }

    void campusMenu() {
        int choice;
        do {
            cout << "\n===== CAMPUS MAP =====" << endl;
            cout << "1. Add Building" << endl;
            cout << "2. Remove Building" << endl;
            cout << "3. Add Path" << endl;
            cout << "4. Remove Path" << endl;
            cout << "5. BFS Traversal" << endl;
            cout << "6. DFS Traversal" << endl;
            cout << "7. Display Map" << endl;
            cout << "8. Back" << endl;
            cout << "Choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
            case 1: {
                string name;
                cout << "Building name: "; getline(cin, name);
                if (campus.addBuilding(name)) {
                    BuildingRooms* newBR = new BuildingRooms(name);
                    newBR->next = buildingRooms;
                    buildingRooms = newBR;
                    cout << "Building added!" << endl;
                }
                break;
            }
            case 2: {
                string name;
                cout << "Building name to remove: "; getline(cin, name);
                if (campus.removeBuilding(name)) {
                    BuildingRooms* current = buildingRooms;
                    BuildingRooms* prev = nullptr;
                    while (current) {
                        if (current->name == name) {
                            if (prev) {
                                prev->next = current->next;
                            }
                            else {
                                buildingRooms = current->next;
                            }
                            delete current;
                            cout << "Building and its rooms removed!" << endl;
                            break;
                        }
                        prev = current;
                        current = current->next;
                    }
                }
                break;
            }
            case 3: {
                string src, dest;
                int dist;
                cout << "Source building: "; getline(cin, src);
                cout << "Destination building: "; getline(cin, dest);
                cout << "Distance: "; cin >> dist; cin.ignore();
                campus.addPath(src, dest, dist);
                break;
            }
            case 4: {
                string src, dest;
                cout << "Source building: "; getline(cin, src);
                cout << "Destination building: "; getline(cin, dest);
                campus.removePath(src, dest);
                break;
            }
            case 5: {
                string start;
                cout << "Start building: "; getline(cin, start);
                campus.BFS(start);
                break;
            }
            case 6: {
                string start;
                cout << "Start building: "; getline(cin, start);
                campus.DFS(start);
                break;
            }
            case 7: 
                campus.displayAdjacencyList(); 
                break;
            case 8: 
                return;
            default: 
                cout << "Invalid choice!" << endl;
            }
        } while (true);
    }

    void roomsMenu(User* currentUser) {
        if (!currentUser) {
            cout << "Please login first!" << endl;
            return;
        }

        int choice;
        BuildingRooms* selectedBuilding = nullptr;

        do {
            cout << "\n===== ROOMS & SCHEDULING =====" << endl;
            if (selectedBuilding) {
                cout << "Selected building: " << selectedBuilding->name << endl;
            }
            cout << "1. Select Building" << endl;
            cout << "2. Insert Room" << endl;
            cout << "3. Search Room" << endl;
            cout << "4. Reserve Room" << endl;
            cout << "5. Cancel Reservation" << endl;
            cout << "6. Display Rooms" << endl;
            cout << "7. Search by Type" << endl;
            cout << "8. Back" << endl;
            cout << "Choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
            case 1: {
                string name;
                cout << "Building name: "; 
                getline(cin, name);
                selectedBuilding = findBuildingRooms(name);
                if (selectedBuilding) {
                    cout << "Building selected!" << endl;
                }
                else {
                    cout << "Building not found!" << endl;
                }
                break;
            }
            case 2: {
                if (!selectedBuilding) {
                    cout << "Please select a building first!" << endl;
                    break;
                }
                string id, type;
                int floor;
                cout << "Room ID: "; getline(cin, id);
                cout << "Floor: "; cin >> floor; cin.ignore();
                cout << "Type (classroom/lab/office): "; getline(cin, type);
                selectedBuilding->rooms->insertRoom(Room(id, floor, type));
                cout << "Room added!" << endl;
                break;
            }
            case 3: {
                if (!selectedBuilding) {
                    cout << "Please select a building first!" << endl;
                    break;
                }
                string id;
                cout << "Room ID to search: "; getline(cin, id);
                Room* room = selectedBuilding->rooms->searchRoom(id);
                if (room) {
                    cout << "Room found:" << endl;
                    cout << "ID: " << room->id << endl;
                    cout << "Floor: " << room->floor << endl;
                    cout << "Type: " << room->type << endl;
                    cout << "Reserved: " << (room->reserved ? "Yes" : "No") << endl;
                    if (room->reserved) {
                        cout << "Reserved by: " << room->reservedBy << endl;
                    }
                }
                else {
                    cout << "Room not found!" << endl;
                }
                break;
            }
            case 4: {
                if (!selectedBuilding) {
                    cout << "Please select a building first!" << endl;
                    break;
                }
                string roomID;
                cout << "Room ID to reserve: ";
                getline(cin, roomID);
                if (selectedBuilding->rooms->reserveRoom(roomID, currentUser->userName)) {
                    users.updateLastBookedRoom(currentUser->userName, roomID);
                    cout << "Room reserved successfully!" << endl;
                }
                else {
                    cout << "Reservation failed! Room might be already reserved or not found." << endl;
                }
                break;
            }
            case 5: {
                if (!selectedBuilding) {
                    cout << "Please select a building first!" << endl;
                    break;
                }
                string roomID;
                cout << "Room ID to cancel: "; 
                getline(cin, roomID);
                if (selectedBuilding->rooms->cancelReservation(roomID)) {
                    cout << "Reservation cancelled!" << endl;
                }
                else {
                    cout << "Cancellation failed! Room might not be reserved." << endl;
                }
                break;
            }
            case 6: {
                if (!selectedBuilding) {
                    cout << "Please select a building first!" << endl;
                    break;
                }
                selectedBuilding->rooms->displayInorder();
                break;
            }
            case 7: {
                if (!selectedBuilding) {
                    cout << "Please select a building first!" << endl;
                    break;
                }
                string type;
                cout << "Room type to search: "; getline(cin, type);
                selectedBuilding->rooms->displayByType(type);
                break;
            }
            case 8:
                return;
            default: 
                cout << "Invalid choice!" << endl;
            }
        } while (true);
    }

    void complaintsMenu(User* currentUser) {
        if (!currentUser) {
            cout << "Please login first!" << endl;
            return;
        }

        int choice;
        do {
            cout << "\n===== COMPLAINT SYSTEM =====" << endl;
            cout << "Pending complaints: " << complaints.getSize() << endl;
            cout << "1. Submit Complaint" << endl;
            cout << "2. Process Next Complaint" << endl;
            cout << "3. View Next Complaint" << endl;
            cout << "4. Display All Complaints" << endl;
            cout << "5. Back" << endl;
            cout << "Choice: ";
            cin >> choice;
            cin.ignore();

            try {
                switch (choice) {
                case 1: {
                    string bldg, room, desc;
                    cout << "Building: "; getline(cin, bldg);
                    cout << "Room: "; getline(cin, room);
                    cout << "Description: "; getline(cin, desc);
                    complaints.enqueue(currentUser->userName, bldg, room, desc, getCurrentTime());
                    cout << "Complaint submitted!" << endl;
                    break;
                }
                case 2: {
                    Complaint c = complaints.dequeue();
                    cout << "Processing complaint:" << endl;
                    c.display();
                    break;
                }
                case 3: {
                    Complaint c = complaints.peek();
                    cout << "Next complaint to process:" << endl;
                    c.display();
                    break;
                }
                case 4: 
                    complaints.displayAll(); 
                    break;
                case 5: 
                    return;
                default: 
                    cout << "Invalid choice!" << endl;
                }
            }
            catch (const char* msg) {
                cout << "Error: " << msg << endl;
            }
        } while (true);
    }

    void messagingMenu(User* currentUser) {
        if (!currentUser) {
            cout << "Please login first!" << endl;
            return;
        }

        int choice;
        do {
            cout << "\n===== MESSAGING SYSTEM =====" << endl;
            cout << "Logged in as: " << currentUser->userName << endl;
            cout << "1. Send Message" << endl;
            cout << "2. View My Messages" << endl;
            cout << "3. Check New Messages" << endl;
            cout << "4. Back" << endl;
            cout << "Choice: ";
            cin >> choice;
            cin.ignore();

            try {
                switch (choice) {
                case 1: {
                    string to, text;
                    cout << "Recipient username: "; getline(cin, to);
                    cout << "Message: "; getline(cin, text);

                    if (!users.searchUser(to)) {
                        cout << "Recipient not found!" << endl;
                        break;
                    }

                    UserMessages* senderMsgs = findUserMessages(currentUser->userName);
                    if (!senderMsgs) {
                        senderMsgs = new UserMessages(currentUser->userName);
                        senderMsgs->next = userMessages;
                        userMessages = senderMsgs;
                    }

                    Message msg(currentUser->userName, to, text, getCurrentTime());
                    senderMsgs->messages->push(msg);

                    UserMessages* recipientMsgs = findUserMessages(to);
                    if (!recipientMsgs) {
                        recipientMsgs = new UserMessages(to);
                        recipientMsgs->next = userMessages;
                        userMessages = recipientMsgs;
                    }
                    recipientMsgs->messages->push(msg);

                    cout << "Message sent!" << endl;
                    break;
                }
                case 2: {
                    UserMessages* myMsgs = findUserMessages(currentUser->userName);
                    if (myMsgs && !myMsgs->messages->isEmpty()) {
                        myMsgs->messages->display();
                    }
                    else {
                        cout << "No messages!" << endl;
                    }
                    break;
                }
                case 3: {
                    UserMessages* myMsgs = findUserMessages(currentUser->userName);
                    if (myMsgs && !myMsgs->messages->isEmpty()) {
                        cout << "Latest message:" << endl;
                        Message msg = myMsgs->messages->peek();
                        msg.display();
                    }
                    else {
                        cout << "No messages!" << endl;
                    }
                    break;
                }
                case 4: 
                    return;
                default: 
                    cout << "Invalid choice!" << endl;
                }
            }
            catch (const char* msg) {
                cout << "Error: " << msg << endl;
            }
        } while (true);
    }
};
int main() {
    UniversitySystem sys;
    sys.run();
    system("pause");
    return 0;
}