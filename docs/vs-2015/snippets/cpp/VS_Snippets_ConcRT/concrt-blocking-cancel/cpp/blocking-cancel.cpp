// <snippet1>
// blocking-cancel.cpp
// compile with: /c /EHsc
#include <windows.h>
#include <ppl.h>

using namespace concurrency;

// Encapsulates the result of a search operation.
template<typename T>
class Answer
{
public:
   explicit Answer(const T& data)
      : _data(data)
   {
   }

   T get_data() const
   {
      return _data;
   }

   // TODO: Add other methods as needed.

private:
   T _data;

   // TODO: Add other data members as needed.
};

// Searches for an element of the provided array that satisfies the provided
// predicate function.
template<typename T, class Predicate>
Answer<T>* parallel_find_answer(const T a[], size_t count, const Predicate& pred)
{
   // The result of the search.
   Answer<T>* answer = nullptr;
   // Ensures that only one task produces an answer.
   volatile long first_result = 0;

   // Use parallel_for and a task group to search for the element.
   structured_task_group tasks;
   tasks.run_and_wait([&]
   {
      // Declare the type alias for use in the inner lambda function.
      typedef T T;

      parallel_for<size_t>(0, count, [&](const T& n) {
         if (pred(a[n]) && InterlockedExchange(&first_result, 1) == 0)
         {
            // Create an object that holds the answer.
            answer = new Answer<T>(a[n]);
            // Cancel the overall task.
            tasks.cancel();
         }
      });
   });

   return answer;
}
// </snippet1>

namespace B
{
// <snippet2>
// Searches for an element of the provided array that satisfies the provided
// predicate function.
template<typename T, class Predicate>
Answer<T>* parallel_find_answer(const T a[], size_t count, const Predicate& pred)
{
   // The result of the search.
   Answer<T>* answer = nullptr;
   // Ensures that only one task produces an answer.
   volatile long first_result = 0;

   // Use parallel_for and a task group to search for the element.
   structured_task_group tasks;
   tasks.run_and_wait([&]
   {
      // Declare the type alias for use in the inner lambda function.
      typedef T T;

      parallel_for<size_t>(0, count, [&](const T& n) {
         if (pred(a[n]) && InterlockedExchange(&first_result, 1) == 0)
         {
            // Cancel the overall task.
            tasks.cancel();
            // Create an object that holds the answer.
            answer = new Answer<T>(a[n]);            
         }
      });
   });

   return answer;
}
// </snippet2>
} // namespace B

int wmain()
{
   const size_t count = 10000;
   int values[count];

   for (size_t i = 0; i < count; ++i)
   {
      values[i] = i;
   }
 
   Answer<int>* a = nullptr;
   
   a = parallel_find_answer<int>(values, count, [](int n) -> bool {
      return n == 9999;
   });
   printf_s("%d\n", a->get_data());


   a = B::parallel_find_answer<int>(values, count, [](int n) -> bool {
      return n == 9999;
   });
   printf_s("%d\n", a->get_data());
}