#include <condition_variable>
#include <mutex>

namespace Nico
{
    namespace RtspAnalyser
    {
        namespace Libs
        {
            class ConditionalVariable
            {
            public:
                ConditionalVariable();
                ~ConditionalVariable() = default;
                void notify();
                void wait();
            private:
                std::mutex lock;
                std::condition_variable cond;
                bool status;
            };
        }
    }
}
