#pragma once
#ifndef MYVECTOR_H
#define MYVECTOR_H

#include <cstddef> // for size_t
#include <iostream> // for std::cout

template <typename T>
class MyVector {
public:
    // Constructor
    MyVector() : data(nullptr), capacity(0), size(0) {}

    // Constructor with size and initial value
    MyVector(size_t initial_capacity, const T& value) : capacity(initial_capacity), size(initial_capacity) {
        data = new T[capacity];
        for (size_t i = 0; i < size; ++i) {
            data[i] = value;
        }
    }

    // Destructor
    ~MyVector() {
        delete[] data;
    }

    // Push back function
    void push_back(const T& value) {
        if (size == capacity) {
            resize();
        }
        data[size++] = value;
    }

    // Access elements with operator[]
    T& operator[](size_t index) {
        return data[index];
    }

    const T& operator[](size_t index) const {
        return data[index];
    }

    // Get the size of the vector
    size_t get_size() const {
        return size;
    }

    // Iterator support
    T* begin() {
        return data;
    }

    T* end() {
        return data + size;
    }

    const T* begin() const {
        return data;
    }

    const T* end() const {
        return data + size;
    }

private:
    T* data;
    size_t capacity;
    size_t size;

    // Resize function to increase capacity
    void resize() {
        capacity = capacity ? capacity * 2 : 1;
        T* new_data = new T[capacity];
        for (size_t i = 0; i < size; ++i) {
            new_data[i] = data[i];
        }
        delete[] data;
        data = new_data;
    }
};

#endif // MYVECTOR_H