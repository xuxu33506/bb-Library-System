#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <algorithm>

using namespace std;

// ==========================================
// 1. 資料核心：書籍類別 (Book Class)
// ==========================================
class Book {
private:
    string isbn;
    string title;
    string author;
    bool isBorrowed;
    string borrowerId; // 記錄是誰借的

public:
    Book(string isbn, string title, string author, bool isBorrowed = false, string borrowerId = "")
        : isbn(isbn), title(title), author(author), isBorrowed(isBorrowed), borrowerId(borrowerId) {
    }

    string getIsbn() const { return isbn; }
    string getTitle() const { return title; }
    string getAuthor() const { return author; }
    bool getStatus() const { return isBorrowed; }
    string getBorrowerId() const { return borrowerId; }

    void borrowBook(string userId) {
        isBorrowed = true;
        borrowerId = userId;
    }

    void returnBook() {
        isBorrowed = false;
        borrowerId = "";
    }
};

// ==========================================
// 2. 核心技術：類別繼承 (User -> Admin / Member)
// ==========================================
class User {
protected:
    string id;
    string name;
    int role; // 1: Admin, 2: Member

public:
    User(string id, string name, int role) : id(id), name(name), role(role) {}
    virtual ~User() {}

    string getId() const { return id; }
    string getName() const { return name; }
    int getRole() const { return role; }

    // 純虛擬函式：多型（Polymorphism）的展現，用於顯示不同身份的選單
    virtual void displayMenu() const = 0;
};

// 子類別：管理員
class Admin : public User {
public:
    Admin(string id, string name) : User(id, name, 1) {}

    void displayMenu() const override {
        cout << "=========================================\n";
        cout << "  管理者模式 - 歡迎您，" << name << " \n";
        cout << "=========================================\n";
        cout << "  [1] 上架新書 (新增書籍)\n";
        cout << "  [2] 下架舊書 (刪除書籍)\n";
        cout << "  [3] 瀏覽目前館藏所有書籍\n";
        cout << "  [4] 登出系統\n";
        cout << "=========================================\n";
    }
};

// 子類別：一般會員 (讀者)
class Member : public User {
public:
    Member(string id, string name) : User(id, name, 2) {}

    void displayMenu() const override {
        cout << "=========================================\n";
        cout << "  讀者模式 - 歡迎您，" << name << " \n";
        cout << "=========================================\n";
        cout << "  [1] 關鍵字查詢與借閱書籍\n";
        cout << "  [2] 歸還書籍\n";
        cout << "  [3] 查看我目前借閱的書籍\n";
        cout << "  [4] 登出系統\n";
        cout << "=========================================\n";
    }
};

// ==========================================
// 3. 系統核心控制器 (LibrarySystem Class)
// ==========================================
class LibrarySystem {
private:
    vector<Book> books;         // STL Vector 儲存書籍
    vector<User*> users;        // STL Vector 儲存使用者指標（展現多型）
    User* currentUser;          // 目前登入的使用者

    // 內部輔助：清屏與暫停
    void clearScreen() const {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }

    void waitForEnter() const {
        cout << "\n按 [Enter] 鍵繼續...";
        cin.ignore(1000, '\n');
        cin.get();
    }

    // 防呆輸入
    int getValidatedInput(int min, int max) const {
        int choice;
        while (true) {
            cout << "👉 請輸入操作選項 (" << min << "-" << max << "): ";
            if (cin >> choice && choice >= min && choice <= max) {
                cin.ignore(1000, '\n');
                return choice;
            }
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "⚠️ 輸入錯誤！請重新輸入。\n";
        }
    }

public:
    LibrarySystem() : currentUser(nullptr) {
        // 初始化一些預設的使用者帳號
        users.push_back(new Admin("admin", "林教授 (管理員)"));
        users.push_back(new Member("b11001", "張同學"));
        users.push_back(new Member("b11002", "李同學"));
    }

    ~LibrarySystem() {
        for (User* u : users) {
            delete u;
        }
    }

    // ==========================================
    // 4. 核心技術：檔案讀寫功能 (File I/O)
    // ==========================================
    void loadBooksFromFile() {
        books.clear();
        ifstream file("books.txt");
        if (!file.is_open()) return; // 若檔案不存在則跳過（第一次執行）

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string isbn, title, author, borrowedStr, borrowerId;

            // 解析以逗號(,)分隔的資料
            getline(ss, isbn, ',');
            getline(ss, title, ',');
            getline(ss, author, ',');
            getline(ss, borrowedStr, ',');
            getline(ss, borrowerId, ',');

            bool isBorrowed = (borrowedStr == "1");
            books.push_back(Book(isbn, title, author, isBorrowed, borrowerId));
        }
        file.close();
    }

    void saveBooksToFile() const {
        ofstream file("books.txt");
        if (!file.is_open()) {
            cout << "⚠️ 無法儲存資料至檔案！\n";
            return;
        }
        for (const auto& book : books) {
            file << book.getIsbn() << ","
                << book.getTitle() << ","
                << book.getAuthor() << ","
                << (book.getStatus() ? "1" : "0") << ","
                << book.getBorrowerId() << "\n";
        }
        file.close();
    }

    // ==========================================
    // 5. UI 介面與功能邏輯實作
    // ==========================================

    // 系統登入畫面
    bool loginScreen() {
        clearScreen();
        cout << "=========================================\n";
        cout << "      歡迎使用 C++ 期末專題圖書系統     \n";
        cout << "=========================================\n";
        cout << " 測試帳號提示：\n";
        cout << " 1. 管理者帳號：admin\n";
        cout << " 2. 一般學生帳號：b11001 或 b11002\n";
        cout << "=========================================\n";
        cout << "👉 請輸入使用者帳號 (輸入 exit 離開)：";
        string inputId;
        cin >> inputId;

        if (inputId == "exit" || inputId == "EXIT") return false;

        // 在 STL Vector 中尋找使用者
        for (User* u : users) {
            if (u->getId() == inputId) {
                currentUser = u;
                return true;
            }
        }

        cout << "❌ 查無此帳號，請按 Enter 後重新輸入。";
        waitForEnter();
        return true;
    }

    // 啟動系統主迴圈
    void run() {
        loadBooksFromFile(); // 啟動時讀檔

        while (true) {
            if (currentUser == nullptr) {
                if (!loginScreen()) break; // 使用者選擇離開
                continue;
            }

            clearScreen();
            currentUser->displayMenu(); // 多型呼叫：根據身份顯示不同選單

            int choice = getValidatedInput(1, 4);
            if (currentUser->getRole() == 1) {
                handleAdminActions(choice);
            }
            else {
                handleMemberActions(choice);
            }
        }

        saveBooksToFile(); // 離開時寫檔
        cout << "\n💾 系統資料已成功存檔，感謝您的使用！\n";
    }

private:
    // 管理者功能實作
    void handleAdminActions(int choice) {
        switch (choice) {
        case 1: { // 上架新書
            clearScreen();
            string isbn, title, author;
            cout << "=== 功能：上架新書 ===\n";
            cout << "請輸入 ISBN: "; cin >> isbn;
            cin.ignore(); // 清除換行符
            cout << "請輸入書名: "; getline(cin, title);
            cout << "請輸入作者: "; getline(cin, author);

            books.push_back(Book(isbn, title, author));
            cout << "\n✅ 書籍《" << title << "》上架成功！\n";
            saveBooksToFile(); // 即時存檔
            waitForEnter();
            break;
        }
        case 2: { // 下架舊書
            clearScreen();
            cout << "=== 功能：下架舊書 ===\n";
            cout << "請輸入要下架的書籍 ISBN: ";
            string isbn;
            cin >> isbn;

            auto it = find_if(books.begin(), books.end(), [&isbn](const Book& b) {
                return b.getIsbn() == isbn;
                });

            if (it != books.end()) {
                cout << "❌ 已成功下架書籍：《" << it->getTitle() << "》\n";
                books.erase(it);
                saveBooksToFile();
            }
            else {
                cout << "⚠️ 找不到該 ISBN 的書籍。\n";
            }
            waitForEnter();
            break;
        }
        case 3: { // 瀏覽所有書籍
            clearScreen();
            cout << "=== 功能：瀏覽目前所有館藏 ===\n";
            if (books.empty()) {
                cout << "目前圖書館內沒有任何書籍。\n";
            }
            else {
                for (const auto& b : books) {
                    cout << " [ISBN: " << b.getIsbn() << "] 《" << b.getTitle() << "》 - " << b.getAuthor();
                    if (b.getStatus()) {
                        cout << " (🛑 已被借出, 借閱者 ID: " << b.getBorrowerId() << ")\n";
                    }
                    else {
                        cout << " (🟢 可借閱)\n";
                    }
                }
            }
            waitForEnter();
            break;
        }
        case 4: // 登出
            currentUser = nullptr;
            break;
        }
    }

    // 讀者功能實作
    void handleMemberActions(int choice) {
        switch (choice) {
        case 1: { // 查詢與借閱
            clearScreen();
            cout << "=== 功能：查詢與借閱書籍 ===\n";
            cout << "請輸入想搜尋的書名關鍵字: ";
            string keyword;
            cin >> keyword;

            cout << "\n🔍 搜尋結果：\n";
            vector<int> resultsIdx;
            for (size_t i = 0; i < books.size(); ++i) {
                if (books[i].getTitle().find(keyword) != string::npos) {
                    resultsIdx.push_back(i);
                    cout << " [" << resultsIdx.size() << "] ISBN: " << books[i].getIsbn()
                        << " 《" << books[i].getTitle() << "》 [" << (books[i].getStatus() ? "已借出" : "可借閱") << "]\n";
                }
            }

            if (resultsIdx.empty()) {
                cout << "沒有找到符合關鍵字的書籍。\n";
            }
            else {
                cout << "\n👉 輸入上方編號進行借閱 (輸入 0 取消): ";
                int borrowChoice = getValidatedInput(0, resultsIdx.size());
                if (borrowChoice != 0) {
                    int targetIdx = resultsIdx[borrowChoice - 1];
                    if (books[targetIdx].getStatus()) {
                        cout << "⚠️ 很抱歉，這本書已經被別人借走了！\n";
                    }
                    else {
                        books[targetIdx].borrowBook(currentUser->getId());
                        cout << "🎉 借閱成功！請記得歸還。\n";
                        saveBooksToFile();
                    }
                }
            }
            waitForEnter();
            break;
        }
        case 2: { // 歸還書籍
            clearScreen();
            cout << "=== 功能：歸還書籍 ===\n";
            vector<int> myBorrowedIdx;
            for (size_t i = 0; i < books.size(); ++i) {
                if (books[i].getStatus() && books[i].getBorrowerId() == currentUser->getId()) {
                    myBorrowedIdx.push_back(i);
                    cout << " [" << myBorrowedIdx.size() << "] ISBN: " << books[i].getIsbn() << " 《" << books[i].getTitle() << "》\n";
                }
            }

            if (myBorrowedIdx.empty()) {
                cout << "您目前沒有借閱任何書籍。\n";
            }
            else {
                cout << "\n👉 請輸入要歸還的書籍編號 (輸入 0 取消): ";
                int returnChoice = getValidatedInput(0, myBorrowedIdx.size());
                if (returnChoice != 0) {
                    int targetIdx = myBorrowedIdx[returnChoice - 1];
                    books[targetIdx].returnBook();
                    cout << "✅ 歸還成功！謝謝您。\n";
                    saveBooksToFile();
                }
            }
            waitForEnter();
            break;
        }
        case 3: { // 查看我借的書
            clearScreen();
            cout << "=== 功能：我目前借閱的書籍 ===\n";
            int count = 0;
            for (const auto& b : books) {
                if (b.getStatus() && b.getBorrowerId() == currentUser->getId()) {
                    cout << " 📖 ISBN: " << b.getIsbn() << " 《" << b.getTitle() << "》 - 作者: " << b.getAuthor() << "\n";
                    count++;
                }
            }
            if (count == 0) cout << "您目前沒有借閱任何書籍。\n";
            waitForEnter();
            break;
        }
        case 4: // 登出
            currentUser = nullptr;
            break;
        }
    }
};

// ==========================================
// 6. 程式入口點
// ==========================================
int main() {
    LibrarySystem system;
    system.run();
    return 0;
}