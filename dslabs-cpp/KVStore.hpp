#pragma once
#include "Meta.hpp"
#include <string>
#include <concepts>
#include <unordered_map>

namespace daniel
{
    // give a name to a concept std::convertible_to<T, std::string>
    template <typename T>
    concept StringConvertible = std::convertible_to<T, std::string>;

    class KVStoreCommand : public Command, public Comparable<KVStoreCommand>
    {
    };

    class SingleKeyCommand : public KVStoreCommand
    {
        virtual const std::string &Key() = 0;
    };

    class Get : public SingleKeyCommand
    {
    private:
        std::string key;

    public:
        template <StringConvertible T>
        Get(T &&key) : key(std::forward<T>(key)) {}

        const std::string &Key() override
        {
            return key;
        }

        bool operator==(const Get &other)
        {
            return key == other.key;
        }
    };

    class Put : public SingleKeyCommand
    {
    private:
        std::string key;
        std::string value;

    public:
        template <StringConvertible T, StringConvertible U>
        Put(T &&key, U &&value) : key(std::forward<T>(key)), value(std::forward<U>(value)) {}

        const std::string &Key() override
        {
            return key;
        }

        const std::string &Value()
        {
            return value;
        }

        bool operator==(const Put &other)
        {
            return key == other.key && value == other.value;
        }
    };

    class Append : public SingleKeyCommand
    {
    private:
        std::string key;
        std::string value;

    public:
        template <StringConvertible T, StringConvertible U>
        Append(T &&key, U &&value) : key(std::forward<T>(key)), value(std::forward<U>(value)) {}

        const std::string &Key() override
        {
            return key;
        }

        const std::string &Value()
        {
            return value;
        }

        bool operator==(const Append &other)
        {
            return key == other.key && value == other.value;
        }
    };

    class KVStoreResult : public Result
    {
    };

    class GetResult : public KVStoreResult, public Comparable<KVStoreResult>
    {
    private:
        std::string value;

    public:
        template <StringConvertible T>
        GetResult(T &&value) : value(std::forward<T>(value)) {}

        const std::string &Value()
        {
            return value;
        }

        bool operator==(const GetResult &other) const
        {
            return value == other.value;
        }
    };

    class PutOk : public KVStoreResult, public Comparable<KVStoreResult>
    {
    public:
        bool operator==(const PutOk &other) const
        {
            return true;
        }
    };

    class AppendResult : public KVStoreResult, public Comparable<KVStoreResult>
    {
    private:
        std::string value;

    public:
        template <StringConvertible T>
        AppendResult(T &&value) : value(std::forward<T>(value)) {}

        const std::string &Value()
        {
            return value;
        }

        bool operator==(const AppendResult &other) const
        {
            return value == other.value;
        }
    };

    class KeyNotFound : public KVStoreResult, public Comparable<KVStoreResult>
    {
    public:
        bool operator==(const KeyNotFound &other) const
        {
            return true;
        }
    };

    class KVStore : public Application
    {
    private:
        std::unordered_map<std::string, std::string> store_;

    public:
        KVStore() : store_() {}

        Result Execute(Command command) override
        {
            return Execute(command);
        }

        KVStoreResult Execute(Get command)
        {
            auto it = store_.find(command.Key());
            if (it == store_.end())
            {
                return KeyNotFound();
            }
            else
            {
                return GetResult(it->second);
            }
        }

        KVStoreResult Execute(Put command)
        {
            store_[command.Key()] = command.Value();
            return PutOk();
        }

        KVStoreResult Execute(Append command)
        {
            auto it = store_.find(command.Key());
            if (it == store_.end())
            {
                return KeyNotFound();
            }
            else
            {
                it->second += command.Value();
                return AppendResult(it->second);
            }
        }
    };
}