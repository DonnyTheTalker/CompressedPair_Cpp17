#include "compressed_pair.h"

#include <type_traits>
#include <memory>
#include <mutex>


struct Empty {
};

struct NonEmpty {
    int field = 0;
};

struct EmptyButFinal final {
};

template<class V>
inline constexpr bool is_compressed_v = std::is_empty_v<V> && !std::is_final_v<V>;

template<class U, class V>
void CheckSize() {
    if constexpr (is_compressed_v<U> && is_compressed_v<V>) {
        if constexpr (std::is_base_of_v<U, V> || std::is_base_of_v<V, U>) {
            static_assert(sizeof(compressed_pair<U, V>) == 2);
        } else {
            static_assert(sizeof(compressed_pair<U, V>) == 1);
        }
    } else if constexpr (is_compressed_v<V> && !std::is_base_of_v<V, U>) {
        static_assert(sizeof(compressed_pair<U, V>) == sizeof(U));
    } else if constexpr (is_compressed_v<U> && !std::is_base_of_v<U, V>) {
        static_assert(sizeof(compressed_pair<U, V>) == sizeof(V));
    } else {
        static_assert(sizeof(compressed_pair<U, V>) == sizeof(std::pair<U, V>));
    }
};

struct Empty2 {
};

struct Empty3 : Empty2 {
};

struct NonEmptyDescendant : Empty {
    int field;
};

union EmptyUnion {
};

struct Empty4 {
};
struct Empty5 {
};
struct Empty6 {
};

struct EmptyBase {
};
struct EmptyDerived1 : EmptyBase {
};
struct EmptyDerived2 : EmptyBase {
};
struct NonEmptyDerived : NonEmpty, EmptyBase {
};

int main() {
    {
        CheckSize<int, Empty>();
        CheckSize<Empty, long double>();
        CheckSize<std::string, EmptyButFinal>();
        CheckSize<Empty, Empty>();
        CheckSize<std::unique_ptr<std::string>, Empty>();
        CheckSize<Empty, std::mutex>();
        CheckSize<Empty, Empty2>();
        CheckSize<Empty2, Empty3>();
        CheckSize<Empty, NonEmptyDescendant>();
        CheckSize<EmptyUnion, int>();
        static_assert(std::is_empty_v<compressed_pair<Empty4, Empty5>>);
        static_assert(
                sizeof(compressed_pair<compressed_pair<Empty4, Empty5>, compressed_pair<Empty6, char>>) ==
                sizeof(char));
        static_assert(sizeof(compressed_pair<EmptyDerived1, EmptyDerived2>) == 2);
        static_assert(sizeof(compressed_pair<NonEmptyDerived, EmptyDerived1>) == 8);
    }
}
