#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <memory>
#include <limits> // [優化] 用於極致防呆清空緩衝區

using namespace std;

// [優化] 強型別列舉 (Strongly Typed Enum)，避免與整數混淆
enum class Role {
    Admin,
    Member
};

// ==========================================
// 1. 資料核心：書籍類別 (Book Class)
// ==========================================
class Book {
private:
    string isbn;
    string title;
    string author;
    bool isBorrowed;
    string borrowerId;

public:
    Book(const string& isbn, const string& title, const string& author, bool isBorrowed = false, const string& borrowerId = "")
        : isbn(isbn), title(title), author(author), isBorrowed(isBorrowed), borrowerId(borrowerId) {
    }

    // [優化] [[nodiscard]] 確保呼叫者一定會處理回傳值 (C++17)
    [[nodiscard]] string getIsbn() const { return isbn; }
    [[nodiscard]] string getTitle() const { return title; }
    [[nodiscard]] string getAuthor() const { return author; }
    [[nodiscard]] bool getStatus() const { return isBorrowed; }
    [[nodiscard]] string getBorrowerId() const { return borrowerId; }

    void borrowBook(const string& userId) {
        isBorrowed = true;
        borrowerId = userId;
    }

    void returnBook() {
        isBorrowed = false;
        borrowerId = "";
    }

    // [優化] 運算子多載：讓 cout << book 直接印出格式化後的書籍資訊
    friend ostream& operator<<(ostream& os, const Book& b) {
        os << "[ISBN: " << b.isbn << "] 《" << b.title << "》 - 作者: " << b.author;
        if (b.isBorrowed) {
            os << " (🛑 已被借出, 借閱者 ID: " << b.borrowerId << ")";
        }
        else {
            os << " (🟢 可借閱)";
        }
        return os;
    }
};

// ==========================================
// 2. 核心技術：類別繼承 (User -> Admin / Member)
// ==========================================
class User {
protected:
    string id;
    string name;
    Role role;

public:
    User(const string& id, const string& name, Role role) : id(id), name(name), role(role) {}
    virtual ~User() = default;

    [[nodiscard]] string getId() const { return id; }
    [[nodiscard]] string getName() const { return name; }
    [[nodiscard]] Role getRole() const { return role; }

    virtual void displayMenu() const = 0;
};

class Admin : public User {
public:
    Admin(const string& id, const string& name) : User(id, name, Role::Admin) {}

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

class Member : public User {
public:
    Member(const string& id, const string& name) : User(id, name, Role::Member) {}

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
    vector<Book> books;
    vector<unique_ptr<User>> users;
    User* currentUser;

    void clearScreen() const {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }

    void waitForEnter() const {
        cout << "\n按 [Enter] 鍵繼續...";
        // [優化] 使用 numeric_limits 徹底清空緩衝區，取代魔法數字 1000
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }

    int getValidatedInput(int min, int max) const {
        int choice;
        while (true) {
            cout << "👉 請輸入操作選項 (" << min << "-" << max << "): ";
            if (cin >> choice && choice >= min && choice <= max) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return choice;
            }
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "⚠️ 輸入錯誤！請輸入數字，且範圍必須在 " << min << " 到 " << max << " 之間。\n";
        }
    }

public:
    LibrarySystem() : currentUser(nullptr) {
        users.push_back(make_unique<Admin>("admin", "林教授 (管理員)"));
        users.push_back(make_unique<Member>("b11001", "張同學"));
        users.push_back(make_unique<Member>("b11002", "李同學"));
    }

    void loadBooksFromFile() {
        books.clear();
        ifstream file("books.txt");
        if (!file.is_open()) return;

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string isbn, title, author, borrowedStr, borrowerId;

            getline(ss, isbn, ',');
            getline(ss, title, ',');
            getline(ss, author, ',');
            getline(ss, borrowedStr, ',');
            getline(ss, borrowerId, ',');

            bool isBorrowed = (borrowedStr == "1");
            books.emplace_back(isbn, title, author, isBorrowed, borrowerId);
        }
        file.close();
    }

    void saveBooksToFile() const {
        ofstream file("books.txt");
        if (!file.is_open()) {
            cerr << "⚠️ 嚴重錯誤：無法儲存資料至檔案！請檢查目錄權限。\n";
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

        for (const auto& u : users) {
            if (u->getId() == inputId) {
                currentUser = u.get();
                return true;
            }
        }

        cout << "❌ 查無此帳號，請按 Enter 後重新輸入。";
        waitForEnter();
        return true;
    }

    void run() {
        loadBooksFromFile();

        while (true) {
            if (currentUser == nullptr) {
                if (!loginScreen()) break;
                continue;
            }

            clearScreen();
            currentUser->displayMenu();

            int choice = getValidatedInput(1, 4);

            if (currentUser->getRole() == Role::Admin) {
                handleAdminActions(choice);
            }
            else {
                handleMemberActions(choice);
            }
        }

        saveBooksToFile();
        cout << "\n💾 系統資料已成功存檔，感謝您的使用！\n";
    }

private:
    void handleAdminActions(int choice) {
        switch (choice) {
        case 1: {
            clearScreen();
            string isbn, title, author;
            cout << "=== 功能：上架新書 ===\n";
            cout << "請輸入 ISBN: "; cin >> isbn;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "請輸入書名: "; getline(cin, title);
            cout << "請輸入作者: "; getline(cin, author);

            books.emplace_back(isbn, title, author);
            cout << "\n✅ 書籍《" << title << "》上架成功！\n";
            saveBooksToFile();
            waitForEnter();
            break;
        }
        case 2: {
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
        case 3: {
            clearScreen();
            cout << "=== 功能：瀏覽目前所有館藏 ===\n";
            if (books.empty()) {
                cout << "目前圖書館內沒有任何書籍。\n";
            }
            else {
                // [優化] 利用多載的 operator<<，程式碼瞬間變得極度乾淨
                for (const auto& b : books) {
                    cout << " " << b << "\n";
                }
            }
            waitForEnter();
            break;
        }
        case 4:
            currentUser = nullptr;
            break;
        }
    }

    void handleMemberActions(int choice) {
        switch (choice) {
        case 1: {
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
                    cout << " [" << resultsIdx.size() << "] " << books[i] << "\n";
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
        case 2: {
            clearScreen();
            cout << "=== 功能：歸還書籍 ===\n";
            vector<int> myBorrowedIdx;
            for (size_t i = 0; i < books.size(); ++i) {
                if (books[i].getStatus() && books[i].getBorrowerId() == currentUser->getId()) {
                    myBorrowedIdx.push_back(i);
                    cout << " [" << myBorrowedIdx.size() << "] " << books[i] << "\n";
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
        case 3: {
            clearScreen();
            cout << "=== 功能：我目前借閱的書籍 ===\n";
            int count = 0;
            for (const auto& b : books) {
                if (b.getStatus() && b.getBorrowerId() == currentUser->getId()) {
                    cout << " 📖 " << b << "\n";
                    count++;
                }
            }
            if (count == 0) cout << "您目前沒有借閱任何書籍。\n";
            waitForEnter();
            break;
        }
        case 4:
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