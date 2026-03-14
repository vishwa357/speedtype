#pragma once
#include <string>
#include <vector>

struct LessonLine {
    std::string text;
    std::string tip;
};

struct LessonData {
    std::string id;
    std::string name;
    std::string description;
    std::vector<LessonLine> content;
};

class Lesson {
public:
    explicit Lesson(const std::string& path);
};