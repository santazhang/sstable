#pragma once

#include "base/all.h"

namespace sstable {

using base::i32;
using base::i64;
using base::SparseInt;
using base::NoCopy;
using base::Log;

template<class K, class V>
inline void insert_into_map(std::map<K, V>& dict, const K& key, const V& value) {
    dict.insert(typename std::map<K, V>::value_type(key, value));
}

}
