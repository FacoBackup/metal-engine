#include <gtest/gtest.h>
#include "../src/common/Inspectable.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace Metal;

class TestInspectable : public Inspectable {
public:
    float floatVal = 0.0f;
    int intVal = 0;
    std::string stringVal = "";
    bool boolVal = false;
    glm::vec2 vec2Val = {0, 0};
    glm::vec3 vec3Val = {0, 0, 0};
    glm::vec4 vec4Val = {0, 0, 0, 0};
    glm::quat quatVal = {1, 0, 0, 0};
    glm::vec3 colorVal = {1, 1, 1};

    void registerFields() override {
        registerFloat(floatVal, "Group", "Float");
        registerInt(intVal, "Group", "Int");
        registerText(stringVal, "Group", "String");
        registerBool(boolVal, "Group", "Bool");
        registerVec2(vec2Val, "Group", "Vec2");
        registerVec3(vec3Val, "Group", "Vec3");
        registerVec4(vec4Val, "Group", "Vec4");
        registerQuat(quatVal, "Group", "Quat");
        registerColor(colorVal, "Group", "Color");
    }

    const char *getIcon() const override { return ""; }
    const char *getTitle() const override { return "Test"; }
};

TEST(InspectableTest, GetFieldByPointer) {
    TestInspectable inspectable;
    auto &fields = inspectable.getFields();
    ASSERT_EQ(fields.size(), 9);

    auto fFloat = inspectable.getFieldByPointer(&inspectable.floatVal);
    ASSERT_NE(fFloat, nullptr);
    EXPECT_EQ(fFloat->name, "Float");

    auto fInt = inspectable.getFieldByPointer(&inspectable.intVal);
    ASSERT_NE(fInt, nullptr);
    EXPECT_EQ(fInt->name, "Int");

    auto fString = inspectable.getFieldByPointer(&inspectable.stringVal);
    ASSERT_NE(fString, nullptr);
    EXPECT_EQ(fString->name, "String");

    auto fBool = inspectable.getFieldByPointer(&inspectable.boolVal);
    ASSERT_NE(fBool, nullptr);
    EXPECT_EQ(fBool->name, "Bool");

    auto fVec2 = inspectable.getFieldByPointer(&inspectable.vec2Val);
    ASSERT_NE(fVec2, nullptr);
    EXPECT_EQ(fVec2->name, "Vec2");

    auto fVec3 = inspectable.getFieldByPointer(&inspectable.vec3Val);
    ASSERT_NE(fVec3, nullptr);
    EXPECT_EQ(fVec3->name, "Vec3");

    auto fVec4 = inspectable.getFieldByPointer(&inspectable.vec4Val);
    ASSERT_NE(fVec4, nullptr);
    EXPECT_EQ(fVec4->name, "Vec4");

    auto fQuat = inspectable.getFieldByPointer(&inspectable.quatVal);
    ASSERT_NE(fQuat, nullptr);
    EXPECT_EQ(fQuat->name, "Quat");

    auto fColor = inspectable.getFieldByPointer(&inspectable.colorVal);
    ASSERT_NE(fColor, nullptr);
    EXPECT_EQ(fColor->name, "Color");

    EXPECT_EQ(inspectable.getFieldByPointer(nullptr), nullptr);
    int dummy = 0;
    EXPECT_EQ(inspectable.getFieldByPointer(&dummy), nullptr);
}

TEST(InspectableTest, ToJSON) {
    TestInspectable inspectable;
    inspectable.floatVal = 1.5f;
    inspectable.intVal = 42;
    inspectable.stringVal = "Hello";
    inspectable.boolVal = true;
    inspectable.vec2Val = {1.0f, 2.0f};
    inspectable.vec3Val = {3.0f, 4.0f, 5.0f};
    inspectable.vec4Val = {6.0f, 7.0f, 8.0f, 9.0f};
    inspectable.quatVal = {10.0f, 11.0f, 12.0f, 13.0f};
    inspectable.colorVal = {0.1f, 0.2f, 0.3f};

    nlohmann::json j = inspectable.toJSON();

    EXPECT_EQ(j["label"], "Test");
    EXPECT_EQ(j["fields"].size(), 9);

    for (const auto &field: j["fields"]) {
        std::string name = field["name"];
        if (name == "Float") {
            EXPECT_FLOAT_EQ(field["value"].get<float>(), 1.5f);
        } else if (name == "Int") {
            EXPECT_EQ(field["value"].get<int>(), 42);
        } else if (name == "String") {
            EXPECT_EQ(field["value"].get<std::string>(), "Hello");
        } else if (name == "Bool") {
            EXPECT_EQ(field["value"].get<bool>(), true);
        } else if (name == "Vec2") {
            EXPECT_FLOAT_EQ(field["value"]["x"], 1.0f);
            EXPECT_FLOAT_EQ(field["value"]["y"], 2.0f);
        } else if (name == "Vec3") {
            EXPECT_FLOAT_EQ(field["value"]["x"], 3.0f);
            EXPECT_FLOAT_EQ(field["value"]["y"], 4.0f);
            EXPECT_FLOAT_EQ(field["value"]["z"], 5.0f);
        } else if (name == "Vec4") {
            EXPECT_FLOAT_EQ(field["value"]["x"], 6.0f);
            EXPECT_FLOAT_EQ(field["value"]["y"], 7.0f);
            EXPECT_FLOAT_EQ(field["value"]["z"], 8.0f);
            EXPECT_FLOAT_EQ(field["value"]["w"], 9.0f);
        } else if (name == "Quat") {
            // Note: glm::quat {w, x, y, z} or {x, y, z, w} depends on constructor, 
            // but in toJSON we mapped q.x, q.y, q.z, q.w to x, y, z, w
            EXPECT_FLOAT_EQ(field["value"]["x"], 11.0f);
            EXPECT_FLOAT_EQ(field["value"]["y"], 12.0f);
            EXPECT_FLOAT_EQ(field["value"]["z"], 13.0f);
            EXPECT_FLOAT_EQ(field["value"]["w"], 10.0f);
        } else if (name == "Color") {
            EXPECT_FLOAT_EQ(field["value"]["x"], 0.1f);
            EXPECT_FLOAT_EQ(field["value"]["y"], 0.2f);
            EXPECT_FLOAT_EQ(field["value"]["z"], 0.3f);
        }
    }
}
