#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <vector>
#include <string>

using json = nlohmann::json;
using namespace std;

json loadJsonFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        exit(1);
    }

    json data;
    try {
        file >> data;
    } catch (const json::parse_error& e) {
        cerr << "Error: Failed to parse JSON in file " << filename << endl;
        exit(1);
    }

    file.close();
    return data;
}

struct User {
    string username;
    string password;
    string role;
    int balance;
};

vector<User> parseUsers(const json& data) {
    vector<User> users;

    for (const auto& item : data) {
        User user;
        user.username = item.at("username");
        user.password = item.at("password");
        user.role = item.at("role");
        if (user.role == "student") {
            user.balance = item.at("balance");
        }
        users.push_back(user);
    }

    return users;
}

User login(const vector<User>& users) {
    string username, password;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    for (const auto& user : users) {
        if (user.username == username && user.password == password) {
            system("clear");
            cout << "Logged in successfully!" << endl;
            return user;
        }
    }
    cout << "Login failed!" << endl;
    return {};
}

void saveUsersToFile(const vector<User>& users, const string& filename) {
    json jsonData;

    for (const auto& user : users) {
        json userJson = {
            {"username", user.username},
            {"password", user.password},
            {"role", user.role},
            {"balance", user.balance}
        };

        if (user.role == "student") {
            userJson["balance"] = user.balance;
        }

        jsonData.push_back(userJson);
    }

    ofstream file(filename);
    if (file.is_open()) {
        file << jsonData.dump(4);
        file.close();
        cout << "Users saved to file successfully!" << endl;
    } else {
        cerr << "Error: Could not open file " << filename << endl;
    }
}

void createStudent(vector<User>& users, const string& filename) {
    User newUser;

    cout << "Enter username: ";
    cin >> newUser.username;
    cout << "Enter password: ";
    cin >> newUser.password;

    newUser.role = "student";
    newUser.balance = 0;

    users.push_back(newUser);

    saveUsersToFile(users, filename);

    cout << "Student created successfully!" << endl;
}

void deleteUserFromJSON(const string& username, vector<User>& users, const string& filename) {
    auto it = std::remove_if(users.begin(), users.end(),
                             [&username](const User& user) {
                                 return user.username == username;
                             });
    if (it != users.end()) {
        users.erase(it, users.end());

        json jsonData = json::array();
        for (const auto& user : users) {
            json userData;
            userData["username"] = user.username;
            userData["password"] = user.password;
            userData["role"] = user.role;
            if (user.role == "student") {
                userData["balance"] = user.balance;
            }
            jsonData.push_back(userData);
        }

        ofstream outFile(filename);
        if (outFile.is_open()) {
            outFile << jsonData.dump(4);
            outFile.close();
            cout << "User with username '" << username << "' has been deleted from JSON file." << endl;
        } else {
            cout << "Error: Could not write to file " << filename << endl;
        }
    } else {
        cout << "No user found with username '" << username << "'." << endl;
    }
}

void deposit(User& me, vector<User>& users, int amount, const string& filename) {
    me.balance += amount;

    for (auto& user : users) {
        if (user.username == me.username) {
            user.balance = me.balance;
            break;
        }
    }

    saveUsersToFile(users, filename);

    cout << "Balance updated successfully! New balance: " << me.balance << endl;
}

void STmenu(User& me, vector<User>& users, const string& filename) {
    int choice, amount;

    cout << "what do you want?" << endl;
    cout << "1. reserve" << endl;
    cout << "2. Deposit" << endl;
    cout << "3. List Reservation" << endl;
    cout << "4. delete Reservation" << endl;
    cin >> choice;

    switch (choice) {
        case 1:
            cout << "Enter username to delete: ";
        break;
        case 2:
            cout << "Enter amount for deposit: ";
            cin >> amount;
            deposit(me, users, amount, filename);
        break;
        case 3:
            cout << "delete student reserve" << endl;
        break;
        case 4:
            cout << "delete student reserve" << endl;
        break;
    }
}

void ADmenu(vector<User>& users, const string& filename) {
    string usr;
    int choice;

    cout << "what do you want?" << endl;
    cout << "1. add student" << endl;
    cout << "2. delete student" << endl;
    cout << "3. delete student reserve" << endl;
    cin >> choice;
    switch (choice) {
        case 1:
            createStudent(users, filename);
        break;
        case 2:
            cout << "Enter username to delete: ";
            cin >> usr;
            deleteUserFromJSON(usr, users, "users.json");
        break;
        case 3:
            cout << "delete student reserve" << endl;
        break;
    }
}

int main() {
    string filename = "users.json";
    json data = loadJsonFromFile(filename);
    vector<User> users = parseUsers(data);
    if (!users.empty()) {
        cout << "Data Loaded" << endl;
    }
    User me = login(users);
    if (me.role == "student") {
        STmenu(me, users, filename);
    }
    else if (me.role == "admin") {
        ADmenu(users, filename);
    }
    return 0;
}