#include <iostream>
#include <iostream>
#include <nlohmann/json.hpp>


using json = nlohmann::json;
int main() {
    json person = {
        {"name", "Alice"},
        {"age", 30},
        {"is_student", false},
        {"skills", {"C++", "Python", "JSON"}}
    };

    // Output the JSON object
    std::cout << "Original JSON:\n" << person.dump(4) << "\n\n";

    // Access elements
    std::string name = person["name"];
    int age = person["age"];
    bool isStudent = person["is_student"];

    std::cout << "Name: " << name << "\n";
    std::cout << "Age: " << age << "\n";
    std::cout << "Is student: " << std::boolalpha << isStudent << "\n\n";

    // Modify elements
    person["age"] = 31;  // Increment age
    person["is_student"] = true;  // Update student status
    person["skills"].push_back("JavaScript");  // Add a new skill

    // Add a new key-value pair
    person["address"] = {
        {"city", "New York"},
        {"zip", "10001"}
    };

    // Output the modified JSON object
    std::cout << "Modified JSON:\n" << person.dump(4) << "\n\n";

    // Serialize to a string
    std::string jsonString = person.dump();
    std::cout << "Serialized JSON string:\n" << jsonString << "\n\n";

    // Deserialize from a string
    std::string inputJson = R"({"name":"Bob","age":25,"is_student":true})";
    json newPerson = json::parse(inputJson);

    // Output the deserialized object
    std::cout << "Deserialized JSON:\n" << newPerson.dump(4) << "\n";

    return 0;
}

// TIP See CLion help at <a
// href="https://www.jetbrains.com/help/clion/">jetbrains.com/help/clion/</a>.
//  Also, you can try interactive lessons for CLion by selecting
//  'Help | Learn IDE Features' from the main menu.