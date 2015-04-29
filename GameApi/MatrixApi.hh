
class MatrixInterface
{
public:
  virtual Matrix get_matrix() const=0;
  virtual ~MatrixInterface() {}
};

class SimpleMatrix : public MatrixInterface
{
public:
  SimpleMatrix(Matrix m) : m(m) { }
  virtual Matrix get_matrix() const { return m; }  
private:
  Matrix m;
};
