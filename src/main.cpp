#include <map>
#include <iostream>
#include <algorithm>
#include <functional>
#include <utility>
#include <cstring>

namespace otus
{

  template <typename T>
  struct allocator
  {
    using value_type = T;

    using pointer = T *;
    using const_pointer = const pointer;

    using reference = T &;
    using const_reference = const reference;

    template <typename U>
    struct rebind
    {
      using other = allocator<U>;
    };

    T *allocate(std::size_t n)
    {
      auto p = std::malloc(n * sizeof(T));
      if (p)
      {
        return reinterpret_cast<T *>(p);
      }

      return nullptr;
    }

    void deallocate(T *p, std::size_t /*n*/)
    {
      std::free(p);
    }

    template <typename... Args>
    void construct(T *p, Args &&...args)
    {
      new (p) T(std::forward<Args>(args)...);
    }

    void destruct(T *p)
    {
      p->~T();
    }
  };

  template <typename T, std::size_t N>
  struct fixed_linear_allocator
  {
  public:
    using value_type = T;

    using pointer = T *;
    using const_pointer = const pointer;

    using reference = T &;
    using const_reference = const reference;

    template <typename U>
    struct rebind
    {
      using other = fixed_linear_allocator<U, N>;
    };

  public:
    fixed_linear_allocator() : arena_(nullptr), current_(nullptr), free_(N)
    {
      auto arena = std::calloc(N, sizeof(T));
      if (arena)
      {
        arena_ = reinterpret_cast<T *>(arena);
        current_ = arena_;
      }
      else
      {
        throw std::bad_alloc();
      }
    }

    ~fixed_linear_allocator()
    {
      if (arena_)
      {
        std::free(arena_);
      }
    }

    /**
     * @brief
     *
     * @param n - elements count
     * @return T*
     */
    T *allocate(std::size_t n)
    {
      if (n > free_)
      {
        throw std::bad_alloc();
      }

      free_ -= n;
      auto free_zone_start = current_;
      current_ += n;
      return free_zone_start;
    }

    /**
     * @brief
     *
     * @param p
     * @param n - elements count
     */
    void deallocate(T *p, std::size_t n)
    {
      if (free_ + n > N)
      {
        throw std::bad_alloc();
      }
      memset(p, 0, n * sizeof(T));
      free_ += n;
      current_ -= n;
    }

    template <typename... Args>
    void construct(T *p, Args &&...args)
    {
      new (p) T(std::forward<Args>(args)...);
    }

    void destruct(T *p)
    {
      p->~T();
    }

  private:
    T *arena_;
    T *current_;
    std::size_t free_;
  };

  int factorial(int n)
  {
    int f = 1;
    for (int i = 1; i <= n; ++i)
    {
      f *= i;
    }

    return f;
  }

  template <typename T>
  void print_map(const T &map)
  {
    std::for_each(map.cbegin(), map.cend(),
                  [](const auto &element)
                  { std::cout << element.first << '-' << element.second << '\n'; });
  }

  template <class T, class Allocator = allocator<T>>
  struct custom_container
  {
  };
}

int main()
{
  std::cout << "---- map with std::allocator ----\n";
  auto m1 = std::map<int, int>{};
  for (int i = 0; i < 10; i++)
  {
    m1.insert(std::pair<int, int>(i, otus::factorial(i)));
  }
  otus::print_map(m1);

  std::cout << "\n---- map with custom allocator ----\n";
  auto m2 = std::map<int, int, std::less<int>, otus::fixed_linear_allocator<std::pair<const int, int>, 20>>{};
  for (int i = 0; i < 10; i++)
  {
    m2.insert(std::pair<int, int>(i, otus::factorial(i)));
  }
  otus::print_map(m2);

  std::cout << "\n---- custom container with std::allocator ----\n";

  std::cout << "\n---- custom container with custom allocator ----\n";

  return 0;
}
