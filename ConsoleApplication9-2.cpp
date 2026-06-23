// v2_system_ui.cpp
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>

using namespace std;

class Book {
    // (同 V1 的 Book 類別，此處簡略，請包含完整 isbn, title, author 等屬性)
public:
    string isbn, title, author;
    Book(string i, string t, string a) : isbn(i), title(t), author(a) {}
};

class LibrarySystem {
private:
    vector<Book> books;

    void clearScreen() {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }

public:
    void run() {
        books.push_back(Book("111", "C++入門", "陳大文"));
        int choice = 0;
        while (choice != 3) {
            clearScreen();
            cout << "=== 圖書館系統 V2 ===\n";
            cout << "1. 新增書籍\n2. 瀏覽書籍\n3. 離開\n請選擇: ";
            cin >> choice;

            if (choice == 1) {
                string i, t, a;
                cout << "輸入 ISBN, 書名, 作者 (空白隔開): ";
                cin >> i >> t >> a;
                books.push_back(Book(i, t, a));
            }
            else if (choice == 2) {
                for (const auto& b : books) cout << "書名: " << b.title << "\n";
                cout << "按Enter繼續..."; cin.ignore(); cin.get();
            }
        }
    }
};

int main() {
    LibrarySystem sys;
    sys.run();
    return 0;
}