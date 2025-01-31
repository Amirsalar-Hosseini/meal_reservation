#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <vector>
#include <string>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <iomanip>

// data was added successfully -> users: 1,000, reservation: 20,000

using json = nlohmann::json;
using namespace std;

// load--------------------------------------------
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

// struct--------------------------------------------
struct User {
    string username;
    string password;
    string role;
    int balance;
};

struct Meal {
    int id;
    string day;
    string name;
    int price;
    string meal_type;
    string location;
};

struct Reservation {
    int id;
    int price;
    string username;
    string name;
    string meal_type;
    string location;
    string day;
};

// parse--------------------------------------------
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

vector<Meal> parseMeals(const json& data) {
    vector<Meal> meals;

    for (const auto& item : data) {
        Meal meal;
        meal.id = item.at("id");
        meal.day = item.at("day");
        meal.name = item.at("name");
        meal.price = item.at("price");
        meal.meal_type = item.at("meal_type");
        meal.location = item.at("location");

        meals.push_back(meal);
    }

    return meals;
}

vector<Reservation> parseReservations(const json& data) {
    vector<Reservation> reservations;

    for (const auto& item : data) {
        Reservation reservation;
        reservation.id = item.at("id");
        reservation.username = item.at("username");
        reservation.price = item.at("price");
        reservation.name = item.at("name");
        reservation.meal_type = item.at("meal_type");
        reservation.location = item.at("location");
        reservation.day = item.at("day");

        reservations.push_back(reservation);
    }

    return reservations;
}

// login--------------------------------------------
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

// save from json--------------------------------------------
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

void saveMealsToFile(const vector<Meal>& meals, const string& filename) {
    json jsonData;

    for (const auto& meal : meals) {
        json mealJson = {
            {"id", meal.id},
            {"day", meal.day},
            {"name", meal.name},
            {"price", meal.price},
            {"meal_type", meal.meal_type},
            {"location", meal.location}
        };

        jsonData.push_back(mealJson);
    }

    ofstream file(filename);
    if (file.is_open()) {
        file << jsonData.dump(4);
        file.close();
        cout << "Meals saved to file successfully!" << endl;
    } else {
        cerr << "Error: Could not open file " << filename << endl;
    }
}

void saveReservesToFile(const vector<Reservation>& reservations, const string& filename) {
    json jsonData;

    for (const auto& reservation : reservations) {
        json reservationJson = {
            {"id", reservation.id},
            {"username", reservation.username},
            {"name", reservation.name},
            {"price", reservation.price},
            {"meal_type", reservation.meal_type},
            {"location", reservation.location},
            {"day", reservation.day}
        };

        jsonData.push_back(reservationJson);
    }

    ofstream file(filename);
    if (file.is_open()) {
        file << jsonData.dump(4);
        file.close();
        cout << "Reservations saved to file successfully!" << endl;
    } else {
        cerr << "Error: Could not open file " << filename << endl;
    }
}

// create--------------------------------------------
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

void createReservation(vector<Reservation>& reservations, const string& reserveFile, User& me, Meal& meal, vector<User>& users, const string& filename) {
    Reservation newReservation;
    int maxID = 0;

    for (const auto& reservation : reservations) {
        if (reservation.id > maxID) {
            maxID = reservation.id;
        }
    }

    newReservation.id = maxID + 1;
    newReservation.username = me.username;
    newReservation.name = meal.name;
    newReservation.meal_type = meal.meal_type;
    newReservation.price = meal.price;
    newReservation.location = meal.location;
    newReservation.day = meal.day;

    for (auto& user : users) {
        if (user.username == me.username) {
            user.balance -= meal.price;
        }
    }

    saveUsersToFile(users, filename);

    reservations.push_back(newReservation);

    saveReservesToFile(reservations, reserveFile);

    cout << "Reserved successfully!" << endl;
}

// delete from json--------------------------------------------
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

void deleteReserveFromJSON(const int& id, vector<Reservation>& reservations, const string& reserveFile, vector<User>& users, User& me, const string& filename) {
    auto it = std::remove_if(reservations.begin(), reservations.end(),
                             [&id](const Reservation& reservation) {
                                 return reservation.id == id;
                             });
    if (it != reservations.end()) {
        reservations.erase(it, reservations.end());

        json jsonData = json::array();
        for (const auto& reservation : reservations) {
            json userData;
            userData["day"] = reservation.day;
            userData["id"] = reservation.id;
            userData["location"] = reservation.location;
            userData["price"] = reservation.price;
            userData["meal_type"] = reservation.meal_type;
            userData["name"] = reservation.name;
            userData["username"] = reservation.username;
            jsonData.push_back(userData);
        }
        for (auto& user : users) {
            if (user.username == me.username) {
                user.balance += it->price;
            }
        }

        saveUsersToFile(users, filename);

        ofstream outFile(reserveFile);
        if (outFile.is_open()) {
            outFile << jsonData.dump(4);
            outFile.close();
            cout << "Reservation with id '" << id << "' has been deleted from JSON file." << endl;
        } else {
            cout << "Error: Could not write to file " << reserveFile << endl;
        }
    } else {
        cout << "No reserve found with id '" << id << "'." << endl;
    }
}

// asset--------------------------------------------
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

// CSV-------------------------------------------------
void writeCSV(const std::string& filename, const json& data) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open CSV file!");
    }

    if (!data.is_array()) {
        throw std::runtime_error("JSON data is not an array!");
    }

    if (!data.empty()) {
        for (auto it = data[0].items().begin(); it != data[0].items().end(); ++it) {
            file << it.key();
            if (std::next(it) != data[0].items().end()) {
                file << ",";
            }
        }
        file << "\n";
    }

    for (const auto& item : data) {
        for (auto it = item.items().begin(); it != item.items().end(); ++it) {
            file << it.value();
            if (std::next(it) != item.items().end()) {
                file << ",";
            }
        }
        file << "\n";
    }

    file.close();
}

void clearJSON(const std::string& filename) {
    std::ofstream file(filename, std::ios::trunc); // باز کردن فایل برای نوشتن و پاک کردن محتوای قبلی
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open JSON file to clear!");
    }
    file << "[]"; // فایل JSON خالی
    file.close();
}

// menu--------------------------------------------
void STmenu(User& me, vector<User>& users, const string& filename, vector<Meal>& meals, vector<Reservation>& reservations, const string& reserveFile) {
    while (1) {
        sleep(2);
        system("clear");

        int choice, amount, meal_id, reserve_id;
        string m, loc, d;

        cout << "what do you want?" << endl;
        cout << "1. reserve" << endl;
        cout << "2. Deposit" << endl;
        cout << "3. List Reservation" << endl;
        cout << "4. delete Reservation" << endl;
        cin >> choice;
        system("clear");

        switch (choice) {
            case 1:
                cout << "witch day?: ";
            cin >> d;
            cout << "witch meal?: ";
            cin >> m;
            cout << "self or buffet?: ";
            cin >> loc;
            for (auto& meal : meals) {
                if (meal.day == d && meal.location == loc && meal.meal_type == m) {
                    cout << meal.name << " - price: " << meal.price  << " - id: " << meal.id << endl;
                }
            }
            cout << "witch one?(id): ";
            cin >> meal_id;
            for (auto& meal : meals) {
                if (meal.id == meal_id) {
                    if (me.balance >= meal.price) {
                        createReservation(reservations, reserveFile, me, meal, users, filename);
                    }
                    else {
                        cout << "Error: your balance not enough!" << endl;
                    }
                }
            }

            break;
            case 2:
                cout << "Enter amount for deposit: ";
            cin >> amount;
            deposit(me, users, amount, filename);
            break;
            case 3:
                for (auto& reservation : reservations) {
                    if (reservation.username == me.username) {
                        cout << "day: " << reservation.day << " meal: " << reservation.meal_type <<  " location: " << reservation.location <<  " name: " << reservation.name <<   " price: " << reservation.price << endl;
                    }
                }
            break;
            case 4:
                for (auto& reservation : reservations) {
                    if (reservation.username == me.username) {
                        cout << "day: " << reservation.day << " meal: " << reservation.meal_type <<  " location: " << reservation.location <<  " name: " << reservation.name <<   " price: " << reservation.price <<   " id: " << reservation.id << endl;
                    }
                }
            cout << "witch one?(id): ";
            cin >> reserve_id;
            for (auto& reservation : reservations) {
                if (reservation.id == reserve_id) {
                    deleteReserveFromJSON(reserve_id, reservations, reserveFile, users, me, filename);
                }
            }
            break;
        }
    }
}

void ADmenu(vector<User>& users, const string& filename, vector<Meal>& meals, vector<Reservation>& reservations, const string& reserveFile, const json& data, const string& mealFile) {

    string usr;
    int choice;
    int meal_id, reserve_id;
    string m, loc, d;
    User sel_user;
    const std::string csvFile = "reservation.csv";

    while (true) {
        cout << "what do you want?" << endl;
        cout << "1. add student" << endl;
        cout << "2. delete student" << endl;
        cout << "3. add reserve student" << endl;
        cout << "4. delete reserve student" << endl;
        cout << "5. all reservation data(Exel(CSV))" << endl;
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
                cout << "Enter student username: ";
                cin >> usr;

                for (auto& user : users) {
                    if (user.username == usr) {
                        sel_user = user;
                    }
                }

                cout << "witch day?: ";
                cin >> d;
                cout << "witch meal?: ";
                cin >> m;
                cout << "self or buffet?: ";
                cin >> loc;
                for (auto& meal : meals) {
                    if (meal.day == d && meal.location == loc && meal.meal_type == m) {
                        cout << meal.name << " - price: " << meal.price  << " - id: " << meal.id << endl;
                    }
                }
                cout << "witch one?(id): ";
                cin >> meal_id;
                for (auto& meal : meals) {
                    if (meal.id == meal_id) {
                        if (sel_user.balance >= meal.price) {
                            createReservation(reservations, reserveFile, sel_user, meal, users, filename);
                        }
                        else {
                            cout << "Error: student balance not enough!" << endl;
                        }
                    }
                }
            break;
            case 4:
                cout << "Enter student username: ";
            cin >> usr;

            for (auto& user : users) {
                if (user.username == usr) {
                    sel_user = user;
                }
            }

            for (auto& reservation : reservations) {
                if (reservation.username == sel_user.username) {
                    cout << "day: " << reservation.day << " meal: " << reservation.meal_type <<  " location: " << reservation.location <<  " name: " << reservation.name <<   " price: " << reservation.price <<   " id: " << reservation.id << endl;
                }
            }
            cout << "witch one?(id): ";
            cin >> reserve_id;
            for (auto& reservation : reservations) {
                if (reservation.id == reserve_id) {
                    deleteReserveFromJSON(reserve_id, reservations, reserveFile, users, sel_user, filename);
                }
            }
            break;
            case 5:
                try {
                    writeCSV(csvFile, data);

                    clearJSON(reserveFile);
                    clearJSON(mealFile);


                    std::vector<std::string> daysOfWeek = {"sunday", "monday", "tuesday", "wednesday", "saturday"};
                    std::vector<std::string> self_food = {"kabab kobide", "qorme", "qeyme", "joje kabab", "mahi", "akbar joje"};
                    std::vector<std::string> buffet_food = {"kalbas", "chizberger", "kabab loqme", "makaroni", "koktel", "hotdog", "felafel", "naget"};
                    std::vector<std::string> meals = {"breakfast", "lunch", "dinner"};

                    std::map<std::string, int> food_prices = {
                        {"kabab kobide", 15000}, {"qorme", 12000}, {"qeyme", 12000}, {"joje kabab", 15000},
                        {"mahi", 15000}, {"akbar joje", 15000}, {"kalbas", 12000}, {"chizberger", 12000},
                        {"kabab loqme", 12000}, {"makaroni", 12000}, {"koktel", 12000}, {"hotdog", 12000},
                        {"felafel", 12000}, {"naget", 12000}
                    };

                    std::srand(static_cast<unsigned>(std::time(nullptr)));

                    json reservations_meals = json::array();
                    int id_counter = 1;

                    for (const auto& day : daysOfWeek) {
                        for (const auto& meal : meals) {
                            bool is_self = std::rand() % 2;
                            std::string selected_food;

                            if (is_self) {
                                selected_food = self_food[std::rand() % self_food.size()];
                            } else {
                                selected_food = buffet_food[std::rand() % buffet_food.size()];
                            }

                            json reservation = {
                                {"id", id_counter++},
                                {"day", day},
                                {"name", selected_food},
                                {"price", food_prices[selected_food]},
                                {"meal_type", meal},
                                {"location", is_self ? "self" : "buffet"}
                            };

                            reservations_meals.push_back(reservation);
                        }
                    }
                    std::ofstream outFile("meals.json");
                    if (outFile.is_open()) {
                        outFile << std::setw(4) << reservations_meals << std::endl;
                        outFile.close();
                        std::cout << "Reservations saved to 'reservation.json'!" << std::endl;
                    } else {
                        std::cerr << "Error: Unable to open file for writing." << std::endl;
                    }

                    std::cout << "Data exported to " << csvFile << " and JSON file cleared." << std::endl;
                } catch (const std::exception& e) {
                    std::cerr << "Error: " << e.what() << std::endl;
                }
            break;
        }
    }
}

// main--------------------------------------------
int main() {
    string filename = "users.json";
    string mealFile = "meals.json";
    string reserveFile = "reservation.json";

    json data = loadJsonFromFile(filename);
    json mealData = loadJsonFromFile(mealFile);
    json reserveData = loadJsonFromFile(reserveFile);

    vector<User> users = parseUsers(data);
    vector<Meal> meals = parseMeals(mealData);
    vector<Reservation> reservations = parseReservations(reserveData);

    if (!users.empty() && !meals.empty() && !reservations.empty()) {
        cout << "Data Loaded" << endl;
    }
    User me = login(users);
    if (me.role == "student") {
        STmenu(me, users, filename, meals, reservations, reserveFile);
    }
    else if (me.role == "admin") {
        ADmenu(users, filename, meals, reservations, reserveFile, reserveData, mealFile);
    }
    return 0;
}