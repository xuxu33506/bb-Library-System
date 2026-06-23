// v3_oop_users.cpp
#include <iostream>
#include <vector>
#include <string>

using namespace std;

class User {
protected:
    string id, name;
public:
    User(string id, string name) : id(id), name(name) {}
    virtual ~User() {}
    string getId() const { return id; }
    virtual void displayMenu() const = 0; // 多型核心
};

class Admin : public User {
public:
    Admin(string id, string name) : User(id, name) {}
    void displayMenu() const override { cout << "\n--- 管理員選單 (" << name << ") ---\n1. 上架新書\n2. 登出\n"; }
};

class Member : public User {
public:
    Member(string id, string name) : User(id, name) {}
    void displayMenu() const override { cout << "\n--- 讀者選單 (" << name << ") ---\n1. 借閱書籍\n2. 登出\n"; }
};

class LibrarySystem {
private:
    vector<User*> users;
    User* currentUser = nullptr;
public:
    LibrarySystem() {
        users.push_back(new Admin("admin", "管理員"));
        users.push_back(new Member("b11", "學生"));
    }
    ~LibrarySystem() { for (auto u : users) delete u; }

    void run() {
        while (true) {
            if (!currentUser) {
                string acc;
                cout << "請輸入帳號登入 (輸入 exit 離開): ";
                cin >> acc;
                if (acc == "exit") break;
                for (auto u : users) if (u->getId() == acc) currentUser = u;
                continue;
            }
            currentUser->displayMenu();
            int choice; cout << "選擇: "; cin >> choice;
            if (choice == 2) currentUser = nullptr; // 登出
        }
    }
};

int main() {
    LibrarySystem sys;
    sys.run();
    return 0;
}