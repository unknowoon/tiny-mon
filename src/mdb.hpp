#ifndef MDB_H
#define MDB_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <tuple>
#include <thread>
#include <future>
#include <memory>
#include <functional>


class mdb_ITable {
public:
    virtual ~mdb_ITable() = default;
};

template<typename T>
class primarykey;

// cache_primarykey 클래스 정의
template<typename T>
class primarykey {
public:
    explicit primarykey(const T &val) : _key(val) {
    }

    const T &getKey() const {
        return _key;
    }

    bool operator==(const primarykey<T> &other) const {
        return _key == other._key;
    }

private:
    T _key;
};

namespace std {

    template<typename T>
    struct hash<primarykey<T>> {
        size_t operator()(const primarykey<T>& k) const;
    };

    template<typename T>
    size_t hash<primarykey<T>>::operator()(const primarykey<T>& k) const {
        return std::hash<T>()(k.getKey());
    }

}

enum class CRUD {
    CREATE,
    UPDATE,
    DELETE
};

template<typename KeyType, typename... Args>
class columns {
public:
    using key_type = KeyType;
    using value_type = std::tuple<Args...>;
    using map_type = std::unordered_map<key_type, value_type>;

    bool has(const key_type &key) const {
        return _data.find(key) != _data.end();
    }

    bool get(const key_type &key, value_type &out) const {
        auto it = _data.find(key);
        if (it == _data.end()) return false;
        out = it->second; // 복사
        return true;
    }

    // upsert
    //  - isCreate = true 생성
    //  - isCreate = false 업데이트
    //  - before 에는 연산 전 값 (create면 default 값)
    bool upsert(const key_type &key,
                const value_type &newValue,
                bool &isCreate,
                value_type &before) {
        auto it = _data.find(key);
        if (it == _data.end()) {
            isCreate = true;
            before = value_type{};
            _data.emplace(key, newValue);
        } else {
            isCreate = false;
            before = it->second;
            it->second = newValue;
        }
        return true;
    }

    // 삭제
    bool erase(const key_type &key, value_type &before) {
        auto it = _data.find(key);
        if (it == _data.end()) return false;
        before = it->second;
        _data.erase(it);
        return true;
    }

private:
    map_type _data;
};


template<typename Columns>
class table : public mdb_ITable {
public:
    using key_type = typename Columns::key_type;
    using value_type = typename Columns::value_type;

    using callback_type = std::function<void(const key_type &,
                                             CRUD,
                                             const value_type &,
                                             const value_type &)>;

    explicit table(callback_type cb = nullptr)
        : _callback(std::move(cb)) {
    }

    // upsert(update, insert)
    // - key와 value를 넣으면,
    //   * key가 없으면 Create
    //   * key가 있으면 Update
    std::future<void> upsert(const key_type &key,
                             const value_type &value) {
        bool isCreate = false;
        value_type before{};
        _record.upsert(key, value, isCreate, before);

        CRUD op = isCreate ? CRUD::CREATE : CRUD::UPDATE;
        value_type after = value; // 연산 후 값

        return dispatch_callback_async(key, op, before, after);
    }

    // Delete
    std::future<void> erase(const key_type &key) {
        value_type before{};
        if (!_record.erase(key, before)) {
            std::promise<void> p;
            p.set_value();
            return p.get_future();
        }

        value_type after{};
        return dispatch_callback_async(key, CRUD::DELETE, before, after);
    }

    bool get(const key_type &key, value_type &out) const {
        return _record.get(key, out);
    }

private:
    Columns _record;
    callback_type _callback;

    std::future<void> dispatch_callback_async(const key_type &key,
                                              CRUD op,
                                              const value_type &before,
                                              const value_type &after) {
        if (!_callback) {
            std::promise<void> p;
            p.set_value();
            return p.get_future();
        }

        using task_type = std::packaged_task<void()>;

        task_type task([cb = _callback, key, op, before, after]() {
            cb(key, op, before, after);
        });

        std::future<void> fut = task.get_future();
        std::thread t(std::move(task));
        t.detach();

        return fut;
    }
};


class mdb {
public:
    mdb() = default;

    template<typename RecordType>
    table<RecordType> &getTable(
        const std::string &name,
        typename table<RecordType>::callback_type cb = nullptr) {
        auto it = _tables.find(name);
        if (it == _tables.end()) {
            // 새 테이블 생성
            auto new_table = std::make_unique<table<RecordType>>(std::move(cb));
            auto *raw = new_table.get();
            _tables.emplace(name, std::move(new_table));
            return *raw;
        }

        auto *typed = dynamic_cast<table<RecordType> *>(it->second.get());
        if (!typed) {
            throw std::runtime_error("Table with same name exists but with different RecordType");
        }
        return *typed;
    }

    bool hasTable(const std::string &name) const {
        return _tables.find(name) != _tables.end();
    }

    bool dropTable(const std::string &name) {
        return _tables.erase(name) > 0;
    }

private:
    std::unordered_map<std::string, std::unique_ptr<mdb_ITable> > _tables;
};


#endif
