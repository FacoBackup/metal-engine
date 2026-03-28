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
        registerEditableField<FLOAT>(&floatVal).setName("Float").setGroup("Group");
        registerEditableField<INT>(&intVal).setName("Int").setGroup("Group");
        registerEditableField<STRING>(&stringVal).setName("String").setGroup("Group");
        registerEditableField<BOOLEAN>(&boolVal).setName("Bool").setGroup("Group");
        registerEditableField<VECTOR2>(&vec2Val).setName("Vec2").setGroup("Group");
        registerEditableField<VECTOR3>(&vec3Val).setName("Vec3").setGroup("Group");
        registerEditableField<VECTOR4>(&vec4Val).setName("Vec4").setGroup("Group");
        registerEditableField<QUAT>(&quatVal).setName("Quat").setGroup("Group");
        registerEditableField<COLOR>(&colorVal).setName("Color").setGroup("Group");
    }

    const char *getIcon() const override { return ""; }
    const char *getTitle() const override { return "Test"; }
};

class ReflectionTest : public ::testing::Test {
};

TEST_F(ReflectionTest, GetFieldByPointer) {
    TestInspectable reflectionInstance;
    auto fields = reflectionInstance.getFields();
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
            registerGenericField(
                [this] { return val * 2.0; },
                [this](const nlohmann::json &j) { val = j.get<double>() / 2.0; }
            ).setName("Val");
        }
    };

    TransformerInspectable reflectionInstance;
    nlohmann::json j = reflectionInstance.toJson(DEFAULT, true);

    ASSERT_EQ(j["fields"].size(), 1);
    EXPECT_DOUBLE_EQ(j["fields"][0]["value"].get<double>(), 20.0);

    reflectionInstance.fromJson({{"Val", 40.0}});
    EXPECT_DOUBLE_EQ(reflectionInstance.val, 20.0);
}

TEST_F(ReflectionTest, ReadAndWrite) {
    TestInspectable reflectionInstance;
    reflectionInstance.floatVal = 1.5f;
    reflectionInstance.intVal = 42;
    reflectionInstance.stringVal = "Hello";

    nlohmann::json j = reflectionInstance.toJson(SERIALIZABLE, false);

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

    nlohmann::json j = reflectionInstance.toJson(DEFAULT, true);

    EXPECT_EQ(j["label"], "Test");

    auto getField = [&](const std::string& name) -> nlohmann::json {
        for (auto& f : j["fields"]) {
            if (f["name"] == name) return f;
        }
        return {};
    };

    EXPECT_FLOAT_EQ(getField("Float")["value"].get<float>(), 1.5f);
    EXPECT_EQ(getField("Int")["value"].get<int>(), 42);
    EXPECT_EQ(getField("String")["value"].get<std::string>(), "Hello");
    EXPECT_EQ(getField("Bool")["value"].get<bool>(), true);
    EXPECT_FLOAT_EQ(getField("Vec2")["value"]["x"], 1.0f);
    EXPECT_FLOAT_EQ(getField("Vec2")["value"]["y"], 2.0f);
    EXPECT_FLOAT_EQ(getField("Vec3")["value"]["x"], 3.0f);
    EXPECT_FLOAT_EQ(getField("Vec3")["value"]["y"], 4.0f);
    EXPECT_FLOAT_EQ(getField("Vec3")["value"]["z"], 5.0f);
    EXPECT_FLOAT_EQ(getField("Vec4")["value"]["x"], 6.0f);
    EXPECT_FLOAT_EQ(getField("Vec4")["value"]["y"], 7.0f);
    EXPECT_FLOAT_EQ(getField("Vec4")["value"]["z"], 8.0f);
    EXPECT_FLOAT_EQ(getField("Vec4")["value"]["w"], 9.0f);
    EXPECT_FLOAT_EQ(getField("Quat")["value"]["x"], 11.0f);
    EXPECT_FLOAT_EQ(getField("Quat")["value"]["y"], 12.0f);
    EXPECT_FLOAT_EQ(getField("Quat")["value"]["z"], 13.0f);
    EXPECT_FLOAT_EQ(getField("Quat")["value"]["w"], 10.0f);
    EXPECT_FLOAT_EQ(getField("Color")["value"]["x"], 0.1f);
    EXPECT_FLOAT_EQ(getField("Color")["value"]["y"], 0.2f);
    EXPECT_FLOAT_EQ(getField("Color")["value"]["z"], 0.3f);
}
