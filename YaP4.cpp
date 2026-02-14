#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <cstring>
#include <cmath>
#include <iomanip>

using namespace std;

// 1. Наивный алгоритм поиска
vector<size_t> naiveSearch(const string& text, const string& pattern) {
    vector<size_t> positions;
    size_t n = text.length();
    size_t m = pattern.length();

    for (size_t i = 0; i <= n - m; ++i) {
        size_t j;
        for (j = 0; j < m; ++j) {
            if (text[i + j] != pattern[j])
                break;
        }
        if (j == m) {
            positions.push_back(i);
        }
    }

    return positions;
}

// 2. Поиск с использованием string::find
vector<size_t> stdStringFind(const string& text, const string& pattern) {
    vector<size_t> positions;
    size_t pos = text.find(pattern, 0);

    while (pos != string::npos) {
        positions.push_back(pos);
        pos = text.find(pattern, pos + 1);
    }

    return positions;
}

// 3. Поиск с использованием strstr
vector<size_t> strstrSearch(const string& text, const string& pattern) {
    vector<size_t> positions;
    const char* txt = text.c_str();
    const char* pat = pattern.c_str();
    const char* ptr = strstr(txt, pat);

    while (ptr != nullptr) {
        positions.push_back(ptr - txt);
        ptr = strstr(ptr + 1, pat);
    }

    return positions;
}

// 4. Алгоритм Кнута-Морриса-Пратта (KMP)
vector<int> computePrefixFunction(const string& pattern) {
    int m = pattern.length();
    vector<int> prefix(m, 0);
    int k = 0;

    for (int q = 1; q < m; ++q) {
        while (k > 0 && pattern[k] != pattern[q])
            k = prefix[k - 1];

        if (pattern[k] == pattern[q])
            ++k;

        prefix[q] = k;
    }

    return prefix;
}

vector<size_t> kmpSearch(const string& text, const string& pattern) {
    vector<size_t> positions;
    int n = text.length();
    int m = pattern.length();
    vector<int> prefix = computePrefixFunction(pattern);
    int q = 0;

    for (int i = 0; i < n; ++i) {
        while (q > 0 && pattern[q] != text[i])
            q = prefix[q - 1];

        if (pattern[q] == text[i])
            ++q;

        if (q == m) {
            positions.push_back(i - m + 1);
            q = prefix[q - 1];
        }
    }

    return positions;
}

// 5. Алгоритм Рабина-Карпа
const int PRIME = 257;
const int MOD = 1000000007; // коснтанта для операции взятия остатка

long long createHash(const std::string& str, int length) {
    long long hash = 0;
    for (int i = 0; i < length; ++i) {
        hash = (hash * PRIME + str[i]) % MOD;
    }
    return hash;
}

std::vector<size_t> rabinKarpSearch(const std::string& text, const std::string& pattern) {
    std::vector<size_t> positions;
    int n = text.size();
    int m = pattern.size();
    if (m > n) return positions;

    long long patternHash = createHash(pattern, m);
    long long textHash = createHash(text, m);

    long long power = 1;
    for (int i = 1; i < m; ++i) {
        power = (power * PRIME) % MOD;
    }

    for (int i = 0; i <= n - m; ++i) {
        if (patternHash == textHash) { //&& text.substr(i, m) == pattern) {
            positions.push_back(i);
        }
        if (i < n - m) {
            textHash = (textHash - text[i] * power % MOD + MOD) % MOD;
            textHash = (textHash * PRIME + text[i + m]) % MOD;
        }
    }
    return positions;
}
// Обработка книг и поиск подстрок
void processBook(const string& filename,
    const vector<string>& patterns,
    const vector<pair<string, vector<size_t>(*)(const string&, const string&)>>& algorithms) {

    ifstream file(filename);
    if (!file) {
        cerr << "Не удалось открыть файл: " << filename << endl;
        return;
    }

    string text((istreambuf_iterator<char>(file)),
        istreambuf_iterator<char>());
    file.close();

    cout << "\nОбработка: " << filename;

    for (const auto& pattern : patterns) {
        cout << "\nПодстрока: \"" << pattern << "\"\n";

        for (const auto& algorithm : algorithms) {
            const string& name = algorithm.first;
            auto func = algorithm.second;

            auto start = chrono::high_resolution_clock::now();
            auto positions = func(text, pattern);
            auto end = chrono::high_resolution_clock::now();

            chrono::duration<double> duration = end - start;

            cout << "  " << name << ": "
                << positions.size() << " совпадений, "
                << fixed << setprecision(5)
                << duration.count() << " сек\n";
        }
    }
}

int main() {
    setlocale(LC_ALL, "");
    // Список книг для обработки
    vector<string> books = {
        "The Great Gatsby.txt",
        "Harry Potter and the Chamber of Secrets.txt",
        "Harry Potter and the Prisoner of Azkaban.txt",
        "Harry Potter and the Goblet of Fire.txt",
        "Harry Potter and the Order of the Phoenix.txt",
        "Harry Potter and The Half-Blood Prince.txt",
        "Harry Potter and the Deathly Hallows.txt",
        "Harry Potter and the Sorcerer's Stone.txt"
    };

    // Паттерны для поиска
    vector<string> patterns = {
        "Harry",
        "I dunno",
        "What's up?",
        "Hermione and Ron",
        "said Professor McGonagall"
    };

    // Алгоритмы для сравнения
    vector<pair<string, vector<size_t>(*)(const string&, const string&)>> algorithms = {
        {"Naive", naiveSearch},
        {"std::find", stdStringFind},
        {"strstr", strstrSearch},
        {"KMP", kmpSearch},
        {"Rabin-Karp", rabinKarpSearch}
    };

    // Обработка всех книг
    cout << "Сравнение алгоритмов поиска подстрок\n";
    cout << "Обработка " << books.size() << " книг с помощью " << patterns.size() << " шаблонов: \n";

    for (const auto& book : books) {
        ifstream test(book);
        if (test.good()) {
            test.close();
            processBook(book, patterns, algorithms);
        }
        else {
            cerr << "Файл не найден: " << book << " (пропуск)\n";
        }
    }
    return 0;
}