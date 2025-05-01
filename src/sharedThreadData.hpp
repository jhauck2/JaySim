#include "shotData.hpp"
#include <mutex>

// Data shared between the main thread and the boost tcp thread;

typedef struct shared_data {
    t_ball_data *ball_data;
    bool *should_close;
    std::mutex *ball_mtx;
    std::mutex *close_mtx;

} t_shared_data;