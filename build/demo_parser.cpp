// --- Generated C++ Code ---
#include <iostream>
#include <string>
#include <vector>

double add(double left, double right) {
    return (left + right);
}

struct Student {
    decltype(20) age = 20;
    double getAge() {
    return this->age;
}

};

int main() {
    auto score = 72.5;
    const auto passed = ((score >= 50) && true);
    std::cout << std::string("Score:") << " " << score << " " << passed << std::endl;
    auto result = add(3, 4);
    auto nums = std::vector<double>{1, 2, 3};
    nums[0] = 50;
    std::cout << result << " " << nums[0] << " " << nums.size() << std::endl;
    auto student = Student{21};
    std::cout << student.getAge() << std::endl;
    if ((score >= 80)) {
    std::cout << std::string("HD") << std::endl;
} else {
    std::cout << std::string("Pass") << std::endl;
}

    auto count = 0;
    while (true) {
    count = (count + 1);
    if ((count >= 2)) { break; }
}

    return 0;
}
