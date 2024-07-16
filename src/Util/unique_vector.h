//
// Created by Let'sBlend on 05/07/2024.
//

#ifndef XPLORE_UNIQUE_VECTOR_H
#define XPLORE_UNIQUE_VECTOR_H

namespace std
{
    template<typename T>
    class unique_vector
    {
    public:
        unique_vector(int elements, T arg);
        ~unique_vector() = default;

        std::unordered_set<T> set;
        std::vector<T> vec;

        void add(const T& value);
        void erase(const T& value);
        void erase(const int& index);

        inline const T& operator[](size_t index) const
        {
            return vec[index];
        }

        inline size_t size() const
        {
            return vec.size();
        }
    };
}


#endif //XPLORE_UNIQUE_VECTOR_H
