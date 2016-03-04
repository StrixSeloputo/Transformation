
template <typename T, long ord>
class MatrixT
{
private:
    T m[ord*ord];
    long size = ord*ord;
public:
    MatrixT/*<T, Ord>*/ inverse();
    friend MatrixT operator*(const MatrixT &, const MatrixT &);
};

template<typename T, long ord>
MatrixT MatrixT<T, ord>::inverse()
{

}

template<typename T, long ord>
MatrixT operator*(const MatrixT<T1, ord1> &left, const MatrixT<T2, ord2> &right)
{
    Matrix
}
