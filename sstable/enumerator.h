#pragma once

namespace sst {

template<class T>
class Enumerator {
public:
    virtual ~Enumerator() {}
    virtual bool has_next() = 0;
    operator bool() {
        return this->has_next();
    }
    virtual T next() = 0;
    T operator() () {
        return this->next();
    }
};

} // namespace sst
