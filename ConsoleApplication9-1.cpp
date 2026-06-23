// v1_basic_book.cpp
#include <iostream>
#include <string>

using namespace std;

class Book {
private:
    string isbn;
    string title;
    string author;
    bool isBorrowed;
    string borrowerId;

public:
    Book(string isbn, string title, string author)
        : isbn(isbn), title(title), author(author), isBorrowed(false), borrowerId("") {
    }

    string getTitle() const { return title; }
    bool getStatus() const { return isBorrowed; }

    void printInfo() const {
        cout << "[ISBN: " << isbn << "] 《" << title << "》 - 作者: " << author
            << " | 狀態: " << (isBorrowed ? "已借出" : "在館內") << endl;
    }
};

int main() {
    cout << "=== V1 系統測試：建立書籍 ===" << endl;
    Book book1("978-1234", "C++ 程式設計", "林教授");
    book1.printInfo();
    return 0;
}