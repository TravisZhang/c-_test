#include <iostream>
#include <thread>
#include <future>
#include <cmath>
#include <memory>

double divideByNumber(double num, double denom)
{
    // print system id of worker thread
    std::cout << "Worker thread id = " << std::this_thread::get_id() << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // simulate work

    if (denom == 0)
        throw std::runtime_error("Exception from thread#: Division by zero!");

    return num / denom;
}


int main()
{
    // print system id of worker thread
    std::cout << "Main thread id = " << std::this_thread::get_id() << std::endl;

    // use async to start a task
    double num = 42.0, denom = 2.0;
    // std::future<double> ftr = std::async(std::launch::deferred, divideByNumber, num, denom);
    std::future<double> ftr = std::async(std::launch::async, divideByNumber, num, denom);
    // std::future<double> ftr = std::async(std::launch::any, divideByNumber, num, denom);

    // retrieve result within try-catch-block
    try
    {
        double result = ftr.get();
        std::cout << "Result = " << result << std::endl;
    }
    catch (std::runtime_error e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}

/*
When we execute the code in the previous example, the output is identical to before, so we seemingly have the same functionality as before - or do we? When we use the std::this_thread::get_id() to print the system thread ids of the main and of the worker thread, we get the following command line output:

Image

As expected, the ids between the two threads differ from each other - they are running in parallel. However, one of the major differences between std::thread and std::async is that with the latter, the system decides wether the associated function should be run asynchronously or synchronously. By adjusting the launch parameters of std::async manually, we can directly influence wether the associated thread function will be executed synchronously or asynchronously.

The line

Image

enforces the synchronous execution of divideByNumber, which results in the following output, where the thread ids for main and worker thread are identical.

Image

If we were to use the launch option "async" instead of "deferred", we would enforce an asynchronous execution whereas the option "any" would leave it to the system to decide - which is the default.

At this point, let us compare std::thread with std::async: Internally, std::async creates a promise, gets a future from it and runs a template function that takes the promise, calls our function and then either sets the value or the exception of that promise - depending on function behavior. The code used internally by std::async is more or less identical to the code we used in the previous example, except that this time it has been generated by the compiler and it is hidden from us - which means that the code we write appears much cleaner and leaner. Also, std::async makes it possible to control the amount of concurrency by passing an optional launch parameter, which enforces either synchronous or asynchronous behavior. This ability, especially when left to the system, allows us to prevent an overload of threads, which would eventually slow down the system as threads consume resources for both management and communication. If we were to use too many threads, the increased resource consumption would outweigh the advantages of parallelism and slow down the program. By leaving the decision to the system, we can ensure that the number of threads is chosen in a carefully balanced way that optimizes runtime performance by looking at the current workload of the system and the multi-core architecture of the system.

*/