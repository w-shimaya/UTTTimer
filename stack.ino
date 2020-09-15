#define STACK_MAX_SIZE 10

template <typename T>
class stack {
private:
  T container[STACK_MAX_SIZE];
  int size;
public:
  stack() {
    size = 0;
  }

  bool empty() {
    return size == 0;
  }

  void push(const T &x) {
    if (size >= STACK_MAX_SIZE) {
        return;
    }
  }

  T top() {
    return container[size - 1];
  }

  void pop() {
    if (size > 0) {
      size--;
    }
  }
};