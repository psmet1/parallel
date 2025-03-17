#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cstdio>
#include <chrono>
#include <atomic>

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

using namespace std;

// Worker function that takes commands from the shared queue and executes them.
void worker_thread(queue<string>& tasks,
                   mutex& queue_mutex,
                   condition_variable& cv,
                   const string& log_file,
                   mutex& log_mutex,
                   mutex& console_mutex,
                   atomic<int>& jobs_processed,
                   bool& done)
{
    while (true) {
        string command;
        {
            unique_lock<mutex> lock(queue_mutex);
            // Wait until there is a task or we are done.
            cv.wait(lock, [&]{ return !tasks.empty() || done; });
            if (tasks.empty() && done)
                break; // No more tasks and done flag set.
            // Get a command from the queue.
            command = tasks.front();
            tasks.pop();
        }

        // Append "2>&1" to redirect stderr to stdout.
        string full_command = command + " 2>&1";
        FILE* pipe = popen(full_command.c_str(), "r");
        string result;
        if (pipe) {
            char buffer[128];
            while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                result += buffer;
            }
            pclose(pipe);
        }

        // Write only the command's output to the log file.
        {
            lock_guard<mutex> log_lock(log_mutex);
            ofstream ofs(log_file, ios::app);
            if (ofs) {
                ofs << result;
            } else {
                cerr << "Error opening log file: " << log_file << endl;
            }
        }
        
        // Update the jobs processed counter and print to console.
        int count = ++jobs_processed;
        {
            lock_guard<mutex> console_lock(console_mutex);
            cout << "\rProcessed jobs: " << count << flush;
        }
    }
}

int main(int argc, char* argv[])
{
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <commands_file> <max_parallel_jobs> <log_file>\n";
        return 1;
    }

    string commands_file = argv[1];
    int max_parallel = stoi(argv[2]);
    string log_file = argv[3];

    // Open and read the commands file.
    ifstream ifs(commands_file);
    if (!ifs) {
        cerr << "Error opening commands file: " << commands_file << endl;
        return 1;
    }

    queue<string> tasks;
    string line;
    while (getline(ifs, line)) {
        if (!line.empty())
            tasks.push(line);
    }

    // Synchronization primitives.
    mutex queue_mutex;
    condition_variable cv;
    mutex log_mutex;
    mutex console_mutex;
    atomic<int> jobs_processed{0};
    bool done = false;

    // Create worker threads.
    vector<thread> workers;
    for (int i = 0; i < max_parallel; ++i) {
        workers.emplace_back(worker_thread,
                             ref(tasks),
                             ref(queue_mutex),
                             ref(cv),
                             ref(log_file),
                             ref(log_mutex),
                             ref(console_mutex),
                             ref(jobs_processed),
                             ref(done));
    }

    // Notify all workers that tasks are available.
    cv.notify_all();

    // Wait until the tasks queue is empty.
    while (true) {
        {
            lock_guard<mutex> lock(queue_mutex);
            if (tasks.empty())
                break;
        }
        this_thread::sleep_for(chrono::milliseconds(100));
    }

    // Set the done flag and notify all workers to exit.
    {
        lock_guard<mutex> lock(queue_mutex);
        done = true;
    }
    cv.notify_all();

    // Join all worker threads.
    for (auto& worker : workers) {
        if (worker.joinable())
            worker.join();
    }

    cout << "All jobs processed: " << jobs_processed.load() << endl;
    return 0;
}
