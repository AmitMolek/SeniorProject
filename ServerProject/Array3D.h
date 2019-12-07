#pragma once
#include <exception>
#include <string>
#include <cstdint>

template <typename T>
class Array3D {
private:
	T* arr;

	size_t height;
	size_t width;
	size_t depth;
public:
	Array3D() = delete;

	Array3D(size_t _width, size_t _height, size_t _depth) :
		arr(new T[(size_t)(_height * _width * _depth)]),
		height(_height),
		width(_width),
		depth(_depth) {
	}

	Array3D(size_t _width, size_t _height, size_t _depth, T initial) :
		Array3D(_width, _height, _depth) {
		for (size_t i = 0; i < GetLength(); i++)
			arr[i] = initial;
	}

	Array3D(Array3D& other) :
		arr(other.arr),
		height(other.height),
		width(other.width),
		depth(other.depth) {
	}

	Array3D(Array3D&& other) :
		arr(std::exchange(other.arr, nullptr)),
		height(std::exchange(other.height, 0)),
		width(std::exchange(other.width, 0)),
		depth(std::exchange(other.depth, 0)) {
	}

	~Array3D() {
		if (arr)
			delete[] arr;
	}

	T& operator()(size_t x, size_t y, size_t z) {
		if (x >= width || y >= height || z >= depth) {
			throw std::invalid_argument("Array3D argument out of range");
		}
		try {
			return arr[(height * depth * x) + (depth * y) + z];
		} catch (std::exception& ex) {
			throw ex;
		}
	}

	T& operator[](size_t i) {
		try {
			return arr[i];
		} catch (std::exception& ex) {
			throw std::out_of_range("Array3D failed access at ("
									+ std::to_string(i) + ")");
		}
	}

	Array3D<T>& operator=(Array3D<T>& other) {
		if (arr)
			delete[] arr;

		arr = new T[other.GetLength()];
		height = other.GetHeight();
		width = other.GetWidth();
		depth = other.GetDepth();
		for (size_t i = 0; i < GetLength(); i++)
			arr[i] = other[i];

		return *this;
	}

	Array3D<T>& operator=(Array3D<T>&& other) {
		if (arr)
			delete[] arr;

		arr = std::exchange(other.arr, nullptr);
		height = std::exchange(other.height, 0);
		width = std::exchange(other.width, 0);
		depth = std::exchange(other.depth, 0);

		return *this;
	}

	size_t GetHeight() {
		return height;
	}

	size_t GetWidth() {
		return width;
	}

	size_t GetDepth() {
		return depth;
	}

	size_t GetLength() {
		return (height * width * depth);
	}

};