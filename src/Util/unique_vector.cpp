//
// Created by Let'sBlend on 05/07/2024.
//

#include "unique_vector.h"

namespace std
{
    template<typename T>
    unique_vector<T>::unique_vector(int elements, T arg)
    {
        vec = std::vector<T>{elements, arg};
        set = std::unordered_set<T>{elements, arg};
    }

    template<typename T>
    void unique_vector<T>::add(const T &value)
    {
        if(set.insert(value).second)
            vec.push_back(value);
    }

    template<typename T>
    void unique_vector<T>::erase(const T &value)
    {
        set.erase(value);
        vec.erase(std::remove(vec.begin(), vec.end(), value), vec.end());
    }

    template<typename T>
    void unique_vector<T>::erase(const int &index)
    {
        set.erase(vec[index]);
        vec.erase(index, index + 1);
    }
}