#include "Vector2.hpp"

template <class T> Vector2<T>::Vector2(T x, T y) {
  this->x = x;
  this->y = y;
}

template <class T> Vector2<T>::Vector2() {
  this->x = 0;
  this->y = 0;
}

template <class T> Vector2<T> Vector2<T>::operator*(const double &d) {
  return Vector2<T>(this->x * d, this->y * d);
}

template <class T> Vector2<T> Vector2<T>::operator*(const Vector2<T> &v) {
  return Vector2<T>(this->x * v.x, this->y * v.y);
}

template <class T> Vector2<T> Vector2<T>::operator/(const double &d) {
  return Vector2<T>(this->x / d, this->y / d);
}

template <class T> Vector2<T> Vector2<T>::operator/(const Vector2<T> &v) {
  return Vector2<T>(this->x / v.x, this->y / v.y);
}

template <class T> Vector2<T> Vector2<T>::operator+(const T &n) {
  return Vector2<T>(this->x + n, this->y + n);
}

template <class T> Vector2<T> Vector2<T>::operator+(const Vector2<T> &v) {
  return Vector2<T>(this->x * v.x, this->y * v.x);
}