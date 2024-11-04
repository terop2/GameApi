

#ifndef MYMAP_HH
#define MYMAP_HH


#include <map>
#include <optional>


namespace std
{
  template<class A, class B>
  class mymap : public std::map<A,B>
  {
  public:
    std::optional<B&> index(A a) const
    {
      typename std::map<A,B>::const_iterator i = find(a);
      if (i!=std::map<A,B>::end()) return std::optional<B&>((*i)->second);
      return std::optional<B&>();
    }
  };
};

#endif
