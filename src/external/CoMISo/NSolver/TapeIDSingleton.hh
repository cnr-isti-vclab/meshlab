/*
 * TapeIDSingleton.hpp
 *
 *  Created on: Jan 4, 2013
 *      Author: kremer
 */

#ifndef TAPEIDSINGLETON_HPP_
#define TAPEIDSINGLETON_HPP_

#include <vector>
#include <cassert>
#include <cstring>

class TapeIDSingleton {
public:
    static TapeIDSingleton* Instance() {

        if(reference_ == NULL) {
            reference_ = new TapeIDSingleton();
        }
        return reference_;
    }

    size_t requestId() {

        // Test if previously requested id is available again
        const size_t n = ids_.size();
        for(size_t i = 0; i < n; ++i) {
            if(!ids_[i]) {
                ids_[i] = true;
                return i;
            }
        }

        // Request new id at the end of array
        size_t id = ids_.size();
        ids_.push_back(true);
        return id;
    }

    void releaseId(const size_t _i) {

        assert(_i < ids_.size());
        ids_[_i] = false;
    }

private:

    TapeIDSingleton() {}
    TapeIDSingleton(const TapeIDSingleton&) {}
    ~TapeIDSingleton() {}
    static TapeIDSingleton* reference_;
    std::vector<bool> ids_;
};

#endif /* TAPEIDSINGLETON_HPP_ */
