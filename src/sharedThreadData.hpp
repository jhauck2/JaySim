#include "shotData.hpp"
#include <mutex>

// Data shared between the main thread and the boost tcp thread;

typedef enum LM_status{
    NOT_CONNECTED,
    CONNECTED,
    READY
} LM_status;

typedef struct shared_data {
    t_ball_data *ball_data;
    bool *should_close;
    LM_status *lm_status;
    std::mutex *ball_mtx;
    std::mutex *close_mtx;
    std::mutex *status_mtx;

} t_shared_data;