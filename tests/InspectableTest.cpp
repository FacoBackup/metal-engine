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

    const char* getIcon() override { return ""; }
    const char* getTitle() override { return "Test"; }
};

TEST(InspectableTest, GetFieldByPointer) {
    TestInspectable inspectable;
    auto& fields = inspectable.getFields();
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

