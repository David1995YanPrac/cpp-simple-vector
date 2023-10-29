#pragma once

#include <iostream>
#include <cassert>
#include <algorithm>
#include <initializer_list>
#include <stdexcept>
#include <utility>

class ReserveProxyObj 
{
public:
    explicit ReserveProxyObj(size_t capacity_to_reserve) :proxy_capacity_(capacity_to_reserve) {}

    size_t GetCapacity() {
        return proxy_capacity_;
    }

private:
    size_t proxy_capacity_;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    explicit SimpleVector(size_t size) : SimpleVector(size, Type{}) {}

    SimpleVector(size_t size, const Type& value) : size_(size), capacity_(size) {
        data = new Type[size];

        FillVectorDef(begin(), end()); // Заполняем значениями по умолчанию
        Fill(begin(), end(), value);
    }

    SimpleVector(std::initializer_list<Type> init) : size_(init.size()), capacity_(init.size()) {

        data = new Type[init.size()];
        std::copy(init.begin(), init.end(), begin());
    }

    // Конструктор копирования
    SimpleVector(const SimpleVector& other) noexcept {
        SimpleVector tmp;
        tmp.size_ = 0;
        tmp.capacity_ = other.capacity_;
        tmp.data = new Type[other.size_];
        int counter = 0;
        for (auto it = other.begin(); it != other.end(); ++it) {
            tmp[counter] = *it;
            ++counter;
            ++tmp.size_;
        }
        swap(tmp);
    }

    // Оператор присваивания для копирования
    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this != &rhs) {
            SimpleVector tmp(rhs);
            swap(tmp);
        }
        return *this;
    }

    // Конструктор перемещения
    SimpleVector(SimpleVector&& moved) noexcept :
        data(moved.data),
        size_(moved.size_),
        capacity_(moved.capacity_)
    {
        moved.data = nullptr;
        moved.size_ = 0;
        moved.capacity_ = 0;
    }

    // Оператор присваивания для перемещения
    SimpleVector& operator=(SimpleVector&& rhs) noexcept {
        delete[] data;

        data = rhs.data;
        size_ = rhs.size_;
        capacity_ = rhs.capacity_;

        rhs.data = nullptr;
        rhs.size_ = 0;
        rhs.capacity_ = 0;

        return *this;
    }

    SimpleVector(ReserveProxyObj capacity_to_reserve) {
        Reserve(capacity_to_reserve.GetCapacity());
    }

    ~SimpleVector() {
        delete[]data;
    }

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    //Добавляет элемент в конец вектора
    //При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& value) {
        auto pos = end();
        if (capacity_ == size_) {
            capacity_ == 0 ? capacity_ = 1 : capacity_ = capacity_ * 2;
            Type* temp_data = new Type[capacity_];
            Iterator new_elem = std::move(begin(), pos, temp_data);
            *(new_elem) = std::move(value);
            delete[] data;
            data = temp_data;
            ++size_;
        }
        else {
            *(pos) = std::move(value);
            ++size_;
        }
    }

    void PushBack(Type&& value) {
        auto pos = end();
        if (capacity_ == size_)
        {
            capacity_ == 0 ? capacity_ = 1 : capacity_ = capacity_ * 2;
            Type* temp_data = new Type[capacity_];
            Iterator new_elem = std::move(begin(), pos, temp_data);
            *(new_elem) = std::move(value);
            delete[] data;
            data = temp_data;
            ++size_;
        }
        else {
            *(pos) = std::move(value);
            ++size_;
        }
    }

    //Вставляет значение value в позицию pos.
    //Возвращает итератор на вставленное значение
    //Если перед вставкой значения вектор был заполнен полностью,
    //вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        if (capacity_ == size_) {
            capacity_ == 0 ? capacity_ = 1 : capacity_ = capacity_ * 2;
            Type* temp_data = new Type[capacity_];
            Iterator new_elem = std::move(begin(), Iterator(pos), temp_data);
            *(new_elem) = std::move(value);
            std::move(Iterator(pos), end(), std::next(new_elem));
            delete[] data;
            data = temp_data;
            ++size_;
            return new_elem;
        }
        else {
            std::move_backward(Iterator(pos), end(), std::next(end()));
            *(Iterator(pos)) = std::move(value);
            ++size_;
            return Iterator(pos);
        }
    }

    Iterator Insert(ConstIterator pos, Type&& value) {
        if (capacity_ == size_) {
            capacity_ == 0 ? capacity_ = 1 : capacity_ = capacity_ * 2;
            Type* temp_data = new Type[capacity_];
            Iterator new_elem = std::move(begin(), Iterator(pos), temp_data);
            *(new_elem) = std::move(value);
            std::move(Iterator(pos), end(), std::next(new_elem));
            delete[] data;
            data = temp_data;
            ++size_;
            return new_elem;
        }
        else {
            std::move_backward(Iterator(pos), end(), std::next(end()));
            *(Iterator(pos)) = std::move(value);
            ++size_;
            return Iterator(pos);
        }
    }

    void PopBack() noexcept {
        assert(size_ != 0);
        --size_;
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        assert(pos != end());
        std::move(std::next(Iterator(pos)), end(), Iterator(pos));
        --size_;
        return Iterator(pos);
    }

    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        std::swap(data, other.data);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        return *(data + index);
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        return *(data + index);
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (index >= size_)
            throw std::out_of_range("simple_vector subscript out of range");
        else
            return *(data + index);
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (index >= size_)
            throw std::out_of_range("simple_vector subscript out of range");
        else
            return *(data + index);
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(const size_t new_size) {
        
        if (new_size > size_ && new_size < capacity_) {
            FillVectorDef(begin() + size_, begin() + new_size); // Заполняем новые элементы значениями по умолчанию
        }

        if (new_size > capacity_) {
            capacity_ = new_size;
            Type* buffer_data = new Type[capacity_];
            auto it_end = std::move(begin(), end(), buffer_data);
            FillVectorDef(it_end, it_end + (new_size - size_)); // Заполняем новые элементы значениями по умолчанию
            delete[] data;
            data = buffer_data;
        }
        
        size_ = new_size;
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity > 0 && new_capacity > capacity_) {
            Type* buffer_data = new Type[new_capacity];
            std::move(begin(), end(), buffer_data);
            delete[] data;
            data = buffer_data;
            capacity_ = new_capacity;
        }
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return data;
    }

    // Возвращает итератор на элемент, следующий за последним
    Iterator end() noexcept {
        return data + size_;
    }

    // Возвращает константный итератор на начало массива
    ConstIterator begin() const noexcept {
        return data;
    }

    // Возвращает итератор на элемент, следующий за последним
    ConstIterator end() const noexcept {
        return data + size_;
    }

    // Возвращает константный итератор на начало массива
    ConstIterator cbegin() const noexcept {
        return begin();
    }

    // Возвращает итератор на элемент, следующий за последним
    ConstIterator cend() const noexcept {
        return end();
    }
private:
    Type* data = nullptr;
    size_t size_ = 0;
    size_t capacity_ = 0;

    void FillVectorDef(Iterator first, Iterator last) {
        while (first != last) {
            *first = Type{};
            first++;
        }
    }
    
    void Fill(Iterator first, Iterator last, Type value) {
        while (first != last) {
            *first = std::move(value);
            first++;
        }
    }
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {

    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (lhs < rhs) || (lhs == rhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}