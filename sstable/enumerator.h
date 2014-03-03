#pragma once

namespace sst {

template<class T>
class Enumerator {
public:
    virtual ~Enumerator() {}
    virtual bool has_next() const = 0;
    operator bool() const {
        return this->has_next();
    }
    virtual T next() = 0;
    T operator() () {
        return this->next();
    }
};

} // namespace sst
