class Vector {
public:
  Vector();
  Vector(int);
  ~Vector();
  Vector(Vector&);
  int length();
  double &operator[](int);
  Vector operator()();
  Vector operator()(int);
  Vector operator()(int, int);
  Vector &operator=(Vector&);
  void map(double (*)(double));
  double fold(double (*)(double), double (*)(double, double), double);
  Vector aggregate(int);
  void dump();

  double sum_ident();
  double sum_square();

private:
  int size, ref, *refptr;
  double *ptr;
};

extern double f_identity(double);
extern double f_square(double);
extern double f_add(double, double);
