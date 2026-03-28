#include <gtest/gtest.h>
#include "../src/common/Reflection.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace Metal;

class TestInspectable : public Reflection {
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
        registerEditableField(&floatVal, FLOAT, "Float", "Group");
        registerEditableField(&intVal, INT, "Int", "Group");
        registerEditableField(&stringVal, STRING, "String", "Group");
        registerEditableField(&boolVal, BOOLEAN, "Bool", "Group");
        registerEditableField(&vec2Val, VECTOR2, "Vec2", "Group");
        registerEditableField(&vec3Val, VECTOR3, "Vec3", "Group");
        registerEditableField(&vec4Val, VECTOR4, "Vec4", "Group");
        registerEditableField(&quatVal, QUAT, "Quat", "Group");
        registerEditableField(&colorVal, COLOR, "Color", "Group");
    }

    const char *getIcon() const override { return ""; }
    const char *getTitle() const override { return "Test"; }
};

class ReflectionTest : public ::testing::Test {
};

TEST_F(ReflectionTest, GetFieldByPointer) {
    TestInspectable reflectionInstance;
    auto &fields = reflectionInstance.getFields();
    ASSERT_EQ(fields.size(), 9);

    auto fFloat = reflectionInstance.getFieldByPointer(&reflectionInstance.floatVal);
    ASSERT_NE(fFloat, nullptr);
    EXPECT_EQ(fFloat->name, "Float");

    auto fInt = reflectionInstance.getFieldByPointer(&reflectionInstance.intVal);
    ASSERT_NE(fInt, nullptr);
    EXPECT_EQ(fInt->name, "Int");

    auto fString = reflectionInstance.getFieldByPointer(&reflectionInstance.stringVal);
    ASSERT_NE(fString, nullptr);
    EXPECT_EQ(fString->name, "String");

    auto fBool = reflectionInstance.getFieldByPointer(&reflectionInstance.boolVal);
    ASSERT_NE(fBool, nullptr);
    EXPECT_EQ(fBool->name, "Bool");

    auto fVec2 = reflectionInstance.getFieldByPointer(&reflectionInstance.vec2Val);
    ASSERT_NE(fVec2, nullptr);
    EXPECT_EQ(fVec2->name, "Vec2");

    auto fVec3 = reflectionInstance.getFieldByPointer(&reflectionInstance.vec3Val);
    ASSERT_NE(fVec3, nullptr);
    EXPECT_EQ(fVec3->name, "Vec3");

    auto fVec4 = reflectionInstance.getFieldByPointer(&reflectionInstance.vec4Val);
    ASSERT_NE(fVec4, nullptr);
    EXPECT_EQ(fVec4->name, "Vec4");

    auto fQuat = reflectionInstance.getFieldByPointer(&reflectionInstance.quatVal);
    ASSERT_NE(fQuat, nullptr);
    EXPECT_EQ(fQuat->name, "Quat");

    auto fColor = reflectionInstance.getFieldByPointer(&reflectionInstance.colorVal);
    ASSERT_NE(fColor, nullptr);
    EXPECT_EQ(fColor->name, "Color");

    EXPECT_EQ(reflectionInstance.getFieldByPointer(nullptr), nullptr);
    int dummy = 0;
    EXPECT_EQ(reflectionInstance.getFieldByPointer(&dummy), nullptr);
}

TEST_F(ReflectionTest, Transformer) {
    struct TransformerInspectable : Reflection {
        double val = 10.0;

        void registerFields() override {
            registerSerializableOnlyField(&val, FLOAT, "Val").setTransformer(
                [this] {
                    return val * 2.0;
                },
                [this](const nlohmann::json &j) {
                    val = j.get<double>() / 2.0;
                });
        }
    };

    TransformerInspectable reflectionInstance;
    nlohmann::json j = reflectionInstance.toJson();

    ASSERT_TRUE(j.contains("Val"));
    EXPECT_DOUBLE_EQ(j["Val"]["value"].get<double>(), 20.0);

    reflectionInstance.fromJson({{"Val", {{"value", 40.0}}}});
    EXPECT_DOUBLE_EQ(reflectionInstance.val, 20.0);
}

TEST_F(ReflectionTest, ReadAndWrite) {
    TestInspectable reflectionInstance;
    reflectionInstance.floatVal = 1.5f;
    reflectionInstance.intVal = 42;
    reflectionInstance.stringVal = "Hello";

    nlohmann::json j = reflectionInstance.toJson();

    TestInspectable other;
    other.fromJson(j);

    EXPECT_FLOAT_EQ(other.floatVal, 1.5f);
    EXPECT_EQ(other.intVal, 42);
    EXPECT_EQ(other.stringVal, "Hello");
}

TEST_F(ReflectionTest, ToJSON) {
    TestInspectable reflectionInstance;
    reflectionInstance.floatVal = 1.5f;
    reflectionInstance.intVal = 42;
    reflectionInstance.stringVal = "Hello";
    reflectionInstance.boolVal = true;
    reflectionInstance.vec2Val = {1.0f, 2.0f};
    reflectionInstance.vec3Val = {3.0f, 4.0f, 5.0f};
    reflectionInstance.vec4Val = {6.0f, 7.0f, 8.0f, 9.0f};
    reflectionInstance.quatVal = {10.0f, 11.0f, 12.0f, 13.0f};
    reflectionInstance.colorVal = {0.1f, 0.2f, 0.3f};

    nlohmann::json j = reflectionInstance.toJson();

    EXPECT_EQ(j["label"], "Test");

    EXPECT_FLOAT_EQ(j["Float"]["value"].get<float>(), 1.5f);
    EXPECT_EQ(j["Int"]["value"].get<int>(), 42);
    EXPECT_EQ(j["String"]["value"].get<std::string>(), "Hello");
    EXPECT_EQ(j["Bool"]["value"].get<bool>(), true);
    EXPECT_FLOAT_EQ(j["Vec2"]["value"]["x"], 1.0f);
    EXPECT_FLOAT_EQ(j["Vec2"]["value"]["y"], 2.0f);
    EXPECT_FLOAT_EQ(j["Vec3"]["value"]["x"], 3.0f);
    EXPECT_FLOAT_EQ(j["Vec3"]["value"]["y"], 4.0f);
    EXPECT_FLOAT_EQ(j["Vec3"]["value"]["z"], 5.0f);
    EXPECT_FLOAT_EQ(j["Vec4"]["value"]["x"], 6.0f);
    EXPECT_FLOAT_EQ(j["Vec4"]["value"]["y"], 7.0f);
    EXPECT_FLOAT_EQ(j["Vec4"]["value"]["z"], 8.0f);
    EXPECT_FLOAT_EQ(j["Vec4"]["value"]["w"], 9.0f);
    EXPECT_FLOAT_EQ(j["Quat"]["value"]["x"], 11.0f);
    EXPECT_FLOAT_EQ(j["Quat"]["value"]["y"], 12.0f);
    EXPECT_FLOAT_EQ(j["Quat"]["value"]["z"], 13.0f);
    EXPECT_FLOAT_EQ(j["Quat"]["value"]["w"], 10.0f);
    EXPECT_FLOAT_EQ(j["Color"]["value"]["x"], 0.1f);
    EXPECT_FLOAT_EQ(j["Color"]["value"]["y"], 0.2f);
    EXPECT_FLOAT_EQ(j["Color"]["value"]["z"], 0.3f);
}
