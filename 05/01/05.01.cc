#include <gtest/gtest.h>

#include <memory>

class Person {
  friend class Builder;

public:
  std::string m_name = "";
  int m_age = 0;
  int m_grade = 0;
};

class Builder {
public:
  Builder() { m_person = new Person; }
  auto name(const std::string &new_name) {
    m_person->m_name = new_name;
    return *this;
  }

  auto age(int new_age) {
    m_person->m_age = new_age;
    return *this;
  }

  auto grade(int new_grade) {
    m_person->m_grade = new_grade;
    return *this;
  }

  auto get() { return m_person; }

private:
  Person *m_person;
};

TEST(test, builder_pattern) {

  Builder builder;
  std::unique_ptr<Person> person(builder.name("Ivan").age(25).grade(10).get());

  EXPECT_EQ(person->m_name, "Ivan");
  EXPECT_EQ(person->m_age, 25);
  EXPECT_EQ(person->m_grade, 10);
}
