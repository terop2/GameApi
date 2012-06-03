
class DataBase
{
public:

};

enum Type
  {
    DTDataType,
    DTRef
  };

class DataBaseElement
{
public:
  virtual int Type() const=0;
  virtual int CountX() const=0;
  virtual int CountY() const=0;
  virtual DataBaseElement *GetElem(int x, int y) const=0;
  virtual char *Buffer() const=0;
  virtual int BufferSize() const=0;
};

template<class T>
class DataType : public DataBaseElement
{
public:
  virtual int Type() const { return DTDataType; }
  virtual int CountX() const { return 0; }
  virtual int CountY() const { return 0; }
  virtual DataBaseElement *GetElem(int x, int y) const { return 0; }
  virtual char *Buffer() const { return (char*) &t; }
  virtual int BufferSize() const { return sizeof(t); }
public:
  void SetT(T tt) { t=tt; }
private:
  T t;
};

template<class T>
class DataTypeFactory
{
public:
  DataType<T> &CreateDataType(char *buffer, int size)
private:
  DataType<T> dt;
};

struct PathElem
{
  int x; int y;
};

struct RefI
{
  std::vector<PathElem> vec;
};

class Ref : public DataBaseElement
{
public:
  Ref(RefI &e) : e(e) { }
  virtual int Type() const { return DTRef; }
  int CountX() const { return e.size(); }
  int CountY() const { return 1; }
  DataBaseElement *GetElem(int x, int y) const
  {
    if (y!=0) return 0;
    dt.SetT(e.vec[x]);
    return &dt;
  }
private:
  DataType<PathElem> dt;
  RefI &e;
};
