//
// Created by Shinnosuke Kawai on 2/25/26.
//
#include <thread>
#include <trantor/utils/Logger.h>
#include <core/ref.h>
#include <barrier>

static inline std::atomic count{0};
using namespace Core;
class Resource : public RefCounted<Resource> {
public:
    Resource() {
        LOG_INFO << "Resource is created";
    }

    ~Resource() override {
        count.fetch_add(1, std::memory_order_relaxed);
        LOG_INFO << "Resource is destroyed";
    }
private:
    int data =  0;
};

int main() {
    {
        constexpr int threads = 1000;
        auto resource = std_ex::make_intrusive<Resource>();


        std::barrier ready(threads + 1);
        std::barrier release(threads + 1);

        std::vector<std::thread> workers;
        workers.reserve(threads);

        for (int i = 0; i < threads; ++i) {
            workers.emplace_back([i, resource, &ready, &release]() mutable {
                ready.arrive_and_wait();   // everyone now holds exactly one ref
                // PrintResource(i, resource);
                release.arrive_and_wait(); // keep holding r until main says go
            });
        }

        ready.arrive_and_wait(); // all workers are definitely holding refs *now*
        assert(resource->ref_count() == threads + 1);
        release.arrive_and_wait(); // let workers exit (and drop refs)

        for (auto& t : workers)
            t.join();

        assert(resource->ref_count() == 1);
    }
    assert(count.load(std::memory_order_relaxed) == 1);
}
