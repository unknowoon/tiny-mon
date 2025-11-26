//
// Created by b532882 on 11/26/25.
//
#include <gtest/gtest.h>
#include <functional>
#include <iostream>
#include <unordered_map>
#include <string>
#include <tuple>
#include <future>
#include <thread>

#include "mdb.hpp"

// ------------------ 테스트 예시 ------------------

TEST(cache_db, usage) {
    using Key1 = primarykey<int>;
    // 테이블의 columns 구성하기 위해서 records을 타입을 정합니다.
    using Columns1 = columns<Key1, int, int, char, std::string>;

    using Key2 = primarykey<std::string>;
    using Columns2 = columns<Key2, double, double>;

    mdb db;

    // CRUD가 행위가 이루어지면, cb1 콜백함수가 처리 됩니다.
    auto callback = [](const Key1 &key,
                  CRUD op,
                  const Columns1::value_type &before,
                  const Columns1::value_type &after) {
        switch (op) {
            case CRUD::CREATE:
                std::cout << "!!!!!!!!!!!!!!! CREATE\n";
                break;
        }
        std::cout << "[users] key=" << key.getKey() << "\n";
    };

    // users 라는 테이블을 만듭니다.
    auto &usersTable = db.getTable<Columns1>("users", callback);

    // column 만들고 추가하고, update 및 insert를 한다.
    // 그럼 callback가 호출된다.
    Key1 k1{1};
    Columns1::value_type v1{10, 20, 'A', std::string("hello")};
    auto f1 = usersTable.upsert(k1, v1);
    f1.get();

    // 두 번째 타입의 테이블: "prices"
    auto cb2 = [](const Key2 &key,
                  CRUD op,
                  const Columns2::value_type &before,
                  const Columns2::value_type &after) {
        std::cout << "[prices] key=" << key.getKey() << "\n";
    };

    auto &pricesTable = db.getTable<Columns2>("prices", cb2);

    Key2 k2{"samsung"};
    Columns2::value_type v2{123.45, 678.90};
    auto f2 = pricesTable.upsert(k2, v2);
    f2.get();

    // 같은 이름 + 다른 타입을 요청하면 예외 발생
    EXPECT_THROW(
        db.getTable<Columns2>("users"),
        std::runtime_error
    );
}
