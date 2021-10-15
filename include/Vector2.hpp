#ifndef VECTOR2_HPP
#define VECTOR2_HPP

template <class T> class Vector2 {
public:
  Vector2(T x, T y);
  Vector2();
  T x;
  T y;

  Vector2<T> operator*(const double &d);
  Vector2<T> operator*(const Vector2<T> &v);
  Vector2<T> operator/(const double &d);
  Vector2<T> operator/(const Vector2<T> &v);
  Vector2<T> operator+(const T &n);
  Vector2<T> operator+(const Vector2<T> &v);
};

template class Vector2<int>;
template class Vector2<unsigned int>;
template class Vector2<double>;

#endif